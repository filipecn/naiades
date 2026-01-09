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

/// \file   smoke_solver.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Smoke solvers.

#pragma once

#include <naiades/core/boundary.h>
#include <naiades/core/field.h>
#include <naiades/geo/grid.h>

namespace naiades::solvers {

class Solver {
public:
  using Ptr = hermes::Ref<Solver>;

  virtual void step(f32 dt) = 0;
};

class SmokeSolver2 : public Solver {
public:
  using Ptr = hermes::Ref<SmokeSolver2>;

  struct Parameters {
    f32 visc{};
    f32 diff{};
    f32 bouyancy_scale{0.5f};
  };
  struct Config {
    Config &setGrid(const geo::Grid2 &grid);
    Result<SmokeSolver2> build() const;

  private:
    geo::Grid2 grid_;
  };

  SmokeSolver2() noexcept;
  virtual ~SmokeSolver2() noexcept;

  void step(f32 dt) override;

  const geo::Grid2 &geo() const;
  core::FieldRef<f32> density();
  core::FieldRef<f32> u();
  core::FieldRef<f32> v();
  core::BoundarySet &boundary();

private:
  core::FieldSet &current();
  core::FieldSet &previous();

  // Solve Ax = b by jacobi Gauss-Siedel method
  NaResult solve(core::FieldRef<f32> &x, const core::FieldRef<f32> &b, f32 a,
                 f32 c);
  NaResult addForces(f32 dt);
  NaResult solveVelocity(f32 dt);
  NaResult solveDensity(f32 dt);
  NaResult setBoundaries();

  Parameters parameters_;
  h_size current_step_{0};
  // advected fields
  core::FieldSet dynamic_fields_[2];
  // p, div, etc
  core::FieldSet static_fields_;
  geo::Grid2 grid_;
  // Bounday conditions
  core::BoundarySet boundary_;
};

} // namespace naiades::solvers
