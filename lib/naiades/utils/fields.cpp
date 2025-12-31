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

/// \file   utils.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07

#include <naiades/utils/fields.h>

#include <naiades/utils/math.h>

namespace naiades::utils {

void zalesakVelocityField(const geo::Grid2 &grid,
                          core::FieldRef<hermes::geo::vec2> &field,
                          const hermes::geo::point2 &center, f32 omega) {
  for (auto ij : hermes::range2(grid.resolution(field.element()))) {
    auto flat_ij = grid.safeFlatIndex(field.element(), ij);
    auto wp = grid.position(field.element(), ij);
    field[flat_ij] = zalesak(wp, center, omega);
  }
}

void enrightVelocityField(const geo::Grid2 &grid,
                          core::FieldRef<hermes::geo::vec2> &field, f32 t) {
  for (auto ij : hermes::range2(grid.resolution(field.element()))) {
    auto flat_ij = grid.safeFlatIndex(field.element(), ij);
    auto wp = grid.position(field.element(), ij);
    field[flat_ij] = enright(wp, t);
  }
}

} // namespace naiades::utils
