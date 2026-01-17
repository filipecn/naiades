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

/// \file   blas.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  BLAS

#pragma once

#include <naiades/core/field.h>
#include <naiades/core/operators.h>

namespace naiades::core::blas {

/// a += k * b
template <typename T> NaResult akb(FieldRef<T> &a, T k, const FieldRef<T> &b) {
  HERMES_ASSERT(a.size() == b.size());
  for (h_size i = 0; i < a.size(); ++i)
    a[i] += k * b[i];
  return NaResult::noError();
}

inline NaResult solve(const std::vector<DiscreteOperator> &stencils,
                      const Boundary &boundary, FieldRef<f32> &x,
                      const FieldRef<f32> &x0, f32 a, f32 c) {
  HERMES_UNUSED_VARIABLE(a);
  HERMES_UNUSED_VARIABLE(boundary);
  const h_size iter = 4;
  const f32 inv_c = 1.f / c;
  const auto x_element = x.element();
  HERMES_UNUSED_VARIABLE(x_element);
  for (h_size k = 0; k < iter; ++k) {
    for (h_size i = 0; i < stencils.size(); ++i) {
      x[i] = (x0[i] + stencils[i](x)) * inv_c;
    }
  }
  return NaResult::noError();
}

} // namespace naiades::core::blas
