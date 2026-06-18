/* Copyright (c) 2026, FilipeCN.
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
/// \date   2026-03-28

#include <naiades/core/geometry.h>
#include <naiades/core/topology.h>
#include <naiades/numeric/blas.h>

#include <hermes/geometry/point.h>

#pragma once

namespace naiades::numeric {

/// A numerical stencil consists of a center element and set of neighbour
/// elements. The center is always stored in index 0.
class Stencil2 {
public:
  /// \note the center of the stencil must be in centers[0].
  static Stencil2 build(core::Geometry2::Ptr geo,
                        const std::vector<core::Neighbour> &centers);

  Stencil2() = default;
  virtual ~Stencil2() = default;

  core::ElementIndex index(h_index i) const;
  hermes::geo::point2 center() const;
  hermes::geo::point2 operator[](h_index i) const;
  h_size size() const;
  real_t distance(h_index i, h_index j) const;

private:
  core::Geometry2::Ptr geo_;
  std::vector<core::Neighbour> elements_;
};

} // namespace naiades::numeric
