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

/// \file   convection.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Convection methods.

#pragma once

#include <naiades/core/field.h>
#include <naiades/geo/grid.h>
#include <naiades/sampling/sampler.h>

#include <hermes/math/space_filling.h>

namespace naiades::solvers {

template <typename QuantityType>
void advect(
    const geo::Grid2 &grid, const core::FieldCRef<hermes::geo::vec2> &velocity,
    const std::function<QuantityType(const geo::Grid2 &, core::Element,
                                     const hermes::geo::point2 &)> &sample_func,
    float dt, const core::FieldCRef<QuantityType> &in_field,
    core::FieldRef<QuantityType> &out_field) {
  auto field_res = grid.resolution(in_field.location());
  for (auto z :
       hermes::math::space_filling::MortonRange(0, field_res.total())) {
    auto wp = grid.center(in_field.location(), z.coord2());
    auto flat_index = grid.safeFlatIndex(in_field.location(), z.coord2());
    // sample velocity components
    auto v = velocity[flat_index];
    // compute source position
    auto sp = wp - v * dt;
    // sample field at source
    out_field[flat_index] = sample_func(grid, in_field.location(), sp);
  }
}

template <typename QuantityType>
NaResult advect(const geo::Grid2 &grid, const core::FieldCRef<f32> &u,
                const core::FieldCRef<f32> &v,
                const std::function<QuantityType(
                    const geo::Grid2 &, const core::FieldCRef<QuantityType> &,
                    const hermes::geo::point2 &)> &sample_func,
                float dt, const core::FieldCRef<QuantityType> &in_field,
                core::FieldRef<QuantityType> &out_field) {
  const auto in_field_element = in_field.element();
  auto field_res = grid.resolution(in_field_element);
  // sample velocities at cell centers
  NAIADES_DECLARE_OR_BAD_RESULT(v_v_field,
                                sampling::sample(grid, v, in_field_element));
  NAIADES_DECLARE_OR_BAD_RESULT(v_u_field,
                                sampling::sample(grid, u, in_field_element));
  auto v_v = v_v_field.template get<f32>(0);
  auto v_u = v_u_field.template get<f32>(0);
  for (auto ij : hermes::range2(field_res)) {
    auto wp = grid.center(in_field_element, ij);
    auto flat_index = grid.safeFlatIndex(in_field_element, ij);
    // sample velocity components
    hermes::geo::vec2 velocity(v_u[flat_index], v_v[flat_index]);
    // compute source position
    auto sp = wp - velocity * dt;
    // sample field at source
    out_field[flat_index] = sample_func(grid, in_field, sp);
  }
  return NaResult::noError();
}

} // namespace naiades::solvers
