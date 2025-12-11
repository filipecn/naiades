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

/// \file   stencil.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Interpolation stencil.

#pragma once

#include <naiades/core/field.h>
#include <naiades/geo/grid.h>

namespace naiades::sampling {

class Stencil {
public:
  static Stencil nearest(const geo::RegularGrid2 &grid, core::FieldLocation loc,
                         const hermes::geo::point2 &p);
  static Stencil bilinear(const geo::RegularGrid2 &grid,
                          core::FieldLocation loc,
                          const hermes::geo::point2 &p);

  template <typename T> T evaluate(const core::Field<T> &field) {
    T s = 0.f;
    const h_size n = indices_.size();
    for (h_size i = 0; i < n; ++i) {
      s += field[indices_[i]] * weights_[i];
    }
    return s;
  }

  void add(h_size index, float weight);
  h_size size() const;
  const std::vector<h_size> indices() const;
  const std::vector<f32> weights() const;

private:
  std::vector<h_size> indices_;
  std::vector<float> weights_;

  NAIADES_to_string_FRIEND(Stencil);
};

} // namespace naiades::sampling
