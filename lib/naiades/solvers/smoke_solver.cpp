/* Copyright (c) 2025, FilipeCN.
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/// \file   smoke_solver.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07

#include <naiades/solvers/smoke_solver.h>

#include <naiades/core/blas.h>
#include <naiades/core/operators.h>
#include <naiades/sampling/sampler.h>
#include <naiades/solvers/convection.h>

namespace naiades::solvers {

#define INDEX(F, IJ)                                                           \
  grid_.safeFlatIndex(F##_element, IJ) - grid_.flatIndexOffset(F##_element)

#define FIELD(F, IJ)                                                           \
  F[grid_.safeFlatIndex(F##_element, IJ) - grid_.flatIndexOffset(F##_element)]

SmokeSolver2::Config &SmokeSolver2::Config::setGrid(const geo::Grid2 &grid) {
  grid_ = grid;
  return *this;
}

Result<SmokeSolver2> SmokeSolver2::Config::build() const {
  SmokeSolver2 solver;
  solver.grid_ = grid_;
  for (int i = 0; i < 2; ++i) {
    solver.dynamic_fields_[i].add<f32>(core::Element::Type::U_FACE_CENTER,
                                       {"u"});
    solver.dynamic_fields_[i].add<f32>(core::Element::Type::V_FACE_CENTER,
                                       {"v"});
    solver.dynamic_fields_[i].add<f32>(core::Element::Type::CELL_CENTER,
                                       {"density"});
    solver.dynamic_fields_[i].setElementCountFrom(&solver.grid_);
  }
  solver.static_fields_.add<f32>(core::Element::Type::CELL_CENTER,
                                 {"p", "div"});
  solver.static_fields_.add<hermes::geo::vec2>(core::Element::Type::CELL_CENTER,
                                               {"fc"});
  solver.static_fields_.add<f32>(core::Element::Type::U_FACE_CENTER, {"fu"});
  solver.static_fields_.add<f32>(core::Element::Type::V_FACE_CENTER, {"fv"});
  solver.static_fields_.setElementCountFrom(&solver.grid_);

  // boundary

  auto boundary_element_types = {
      core::Element(core::Element::Type::X_FACE_CENTER)
          .setOrientations(core::element_orientation_bits::y),
      core::Element(core::Element::Type::X_FACE_CENTER)
          .setOrientations(core::element_orientation_bits::neg_y),
      core::Element(core::Element::Type::Y_FACE_CENTER)
          .setOrientations(core::element_orientation_bits::x),
      core::Element(core::Element::Type::Y_FACE_CENTER)
          .setOrientations(core::element_orientation_bits::neg_x)};
  for (auto b : boundary_element_types)
    solver.boundary_.setRegion(solver.grid_.boundary(b));

  return Result<SmokeSolver2>(std::move(solver));
}

SmokeSolver2::SmokeSolver2() noexcept {}

SmokeSolver2::~SmokeSolver2() noexcept {}

void SmokeSolver2::step(f32 dt) {
  solveVelocity(dt);

  // current_step_++;
}

NaResult SmokeSolver2::solveVelocity(f32 dt) {
  addForces(dt);

  // advect

  NAIADES_DECLARE_OR_BAD_RESULT(u, current().get<f32>("u"));
  NAIADES_DECLARE_OR_BAD_RESULT(v, current().get<f32>("v"));
  NAIADES_DECLARE_OR_BAD_RESULT(prev_u, previous().get<f32>("u"));
  NAIADES_DECLARE_OR_BAD_RESULT(prev_v, previous().get<f32>("v"));

  auto f = [](const naiades::geo::Grid2 &grid,
              const naiades::core::FieldCRef<f32> &field,
              const hermes::geo::point2 &p) -> f32 {
    auto stencil =
        naiades::sampling::Stencil::bilinear(grid, field.element(), p);
    return stencil.evaluate(field);
  };

  advect<f32>(grid_, prev_u, prev_u, f, dt, prev_u, u);
  advect<f32>(grid_, prev_u, prev_u, f, dt, prev_v, v);

  // solve pressure system
  NAIADES_DECLARE_OR_BAD_RESULT(div, static_fields_.get<f32>("div"));
  core::divergence(grid_, u, v, div);
  NAIADES_DECLARE_OR_BAD_RESULT(p, static_fields_.get<f32>("p"));
  solve(p, div, 1, 4);
  return NaResult::noError();
}

NaResult SmokeSolver2::solve(core::FieldRef<f32> &x,
                             const core::FieldRef<f32> &x0, f32 a, f32 c) {
  const h_size iter = 4;
  const f32 inv_c = 1.f / c;
  const auto x_element = x.element();
  for (h_size k = 0; k < iter; ++k) {
    for (auto ij : hermes::range2(grid_.resolution(x.element()))) {
      const auto fij = INDEX(x, ij);
      x[fij] = (x0[fij] + a * (FIELD(x, ij.left()) + FIELD(x, ij.right()) +
                               FIELD(x, ij.up()) + FIELD(x, ij.down()))) *
               inv_c;
    }
    // TODO set x boundaries
  }
  return NaResult::noError();
}

NaResult SmokeSolver2::addForces(f32 dt) {
  // compute forces at cell centers and transfer them to faces
  NAIADES_DECLARE_OR_BAD_RESULT(den, current().get<f32>("density"));
  NAIADES_DECLARE_OR_BAD_RESULT(fc,
                                static_fields_.get<hermes::geo::vec2>("fc"));
  const auto den_element = den.element();
  for (auto ij : hermes::range2(grid_.resolution(den_element))) {
    // buoyancy
    const auto fij = INDEX(den, ij);
    fc[fij] = {0.f, den[fij] * parameters_.bouyancy_scale};
  }

  // transfer force to faces
  NAIADES_DECLARE_OR_BAD_RESULT(fu, static_fields_.get<f32>("fu"));
  NAIADES_DECLARE_OR_BAD_RESULT(fv, static_fields_.get<f32>("fv"));
  sampling::sample<hermes::geo::vec2>(grid_, fc, 0, fu);
  sampling::sample<hermes::geo::vec2>(grid_, fc, 1, fv);

  NAIADES_DECLARE_OR_BAD_RESULT(u, current().get<f32>("u"));
  NAIADES_DECLARE_OR_BAD_RESULT(v, current().get<f32>("v"));

  core::blas::akb(u, dt, fu);
  core::blas::akb(v, dt, fv);

  return NaResult::noError();
}

NaResult SmokeSolver2::setBoundaries() { return NaResult::noError(); }

const geo::Grid2 &SmokeSolver2::geo() const { return grid_; }

core::FieldRef<f32> SmokeSolver2::density() {
  return *current().get<f32>("density");
}

core::FieldRef<f32> SmokeSolver2::u() { return *current().get<f32>("u"); }

core::FieldRef<f32> SmokeSolver2::v() { return *current().get<f32>("v"); }

core::Boundary &SmokeSolver2::boundary() { return boundary_; }

core::FieldSet &SmokeSolver2::current() {
  return dynamic_fields_[current_step_ % 2];
}

core::FieldSet &SmokeSolver2::previous() {
  return dynamic_fields_[(current_step_ + 1) % 2];
}

} // namespace naiades::solvers
