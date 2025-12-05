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

/// \file   stencil.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07

#include <hermes/numeric/numeric.h>
#include <naiades/sampling/stencil.h>

namespace naiades::sampling {

Stencil Stencil::bilinear(const geo::RegularGrid2 &grid,
                          core::FieldLocation loc,
                          const hermes::geo::point2 &wp) {
  // transform wp into index space
  auto gp = grid.gridPosition(loc, wp);
  // consider the unit square
  auto x = hermes::numbers::fract(gp.x);
  auto y = hermes::numbers::fract(gp.y);

  // bottom left coordinates give the grid index
  auto cell_index = hermes::index2(gp.x, gp.y);

  ///   v12        x   v22
  ///              |
  ///              |
  ///    x ------- p -- x
  ///              |
  ///   v11        x   v21

  auto v11 = grid.safeFlatIndex(loc, cell_index.plus(0, 0));
  auto v21 = grid.safeFlatIndex(loc, cell_index.plus(1, 0));
  auto v12 = grid.safeFlatIndex(loc, cell_index.plus(0, 1));
  auto v22 = grid.safeFlatIndex(loc, cell_index.plus(1, 1));

  // since wp may fall off the grid or on top of its edges/vertices
  // we may have repeated indices

  Stencil stencil;

  if (v11 == v12 || v21 == v22) {
    // we are above/bellow the grid
    //  v12 ___________ v22
    //  v11             v21
    stencil.add(v11, x - 0.f);
    stencil.add(v21, 1.f - x);
  } else if (v12 == v22 || v11 == v21) {
    // we are on the left/right side of the grid
    // v12 v22
    //    |
    // v11 v21
    stencil.add(v12, 1.f - y);
    stencil.add(v11, y - 0.f);
  } else if (v11 == v12 && v11 == v21 && v11 == v22) {
    // we are on the corner
    // v12 v22
    // v11 v21
    stencil.add(v11, 1.f);
  } else {
    // all must be different
    HERMES_ASSERT(v11 != v12 && v11 != v21 && v11 != v22);

    // w11 = (x2 - x)(y2 - y) / (x2 - x1)(y2 - y1)
    // w12 = (x2 - x)(y - y1) / (x2 - x1)(y2 - y1)
    // w21 = (x - x1)(y2 - y) / (x2 - x1)(y2 - y1)
    // w22 = (x - x1)(y - y1) / (x2 - x1)(y2 - y1)

    // since (x,y) are the fractional of gp:
    // (x1, y1) = (0,0)
    // (x2, y2) = (1,1)
    // so the denominators  (x2 - x1)(y2 - y1) = 1
    //
    // w11 = (x2 - x)(y2 - y)
    stencil.add(v11, (1.f - x) * (1.f - y));
    // w12 = (x2 - x)(y - y1)
    stencil.add(v12, (1.f - x) * (y - 0.f));
    // w21 = (x - x1)(y2 - y)
    stencil.add(v21, (x - 0.f) * (1.f - y));
    // w22 = (x - x1)(y - y1)
    stencil.add(v22, (x - 0.f) * (y - 0.f));
  }

  return stencil;
}

void Stencil::add(h_size index, float weight) {
  indices_.emplace_back(index);
  weights_.emplace_back(weight);
}

h_size Stencil::size() const { return indices_.size(); }

} // namespace naiades::sampling
