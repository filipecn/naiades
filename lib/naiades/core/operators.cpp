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

/// \file   operators.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07

#include <naiades/core/operators.h>

namespace naiades::core {

/// Compute divergence field.
void divergence(const geo::Grid2 &grid, const Field_RO<f32> &u,
                const Field_RO<f32> &v, Field<f32> &f) {
  HERMES_ASSERT(u.element() == Element::Type::Y_FACE_CENTER);
  HERMES_ASSERT(v.element() == Element::Type::X_FACE_CENTER);
  HERMES_ASSERT(f.element() == Element::Type::CELL_CENTER);

#define AT(F, IJ)                                                              \
  F[grid.flatIndex(F.element(), IJ) - grid.flatIndexOffset(F.element())]

  const auto d = grid.cellSize();
  for (auto ij : hermes::range2(grid.resolution(f.element()))) {
    AT(f, ij) = -0.5 * (d.y * (AT(v, ij.up()) - AT(v, ij)) +
                        d.x * (AT(u, ij.right()) - AT(u, ij)));
  }
#undef AT
}

} // namespace naiades::core
