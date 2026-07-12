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

/// \file   stencil.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-03-28

#include <naiades/numeric/stencil.h>

namespace naiades::numeric {

Stencil2 Stencil2::build(core::Geometry2::Ptr geo,
                         const std::vector<core::Neighbour> &centers) {
  Stencil2 stencil;
  stencil.geo_ = geo;
  stencil.elements_ = centers;
  return stencil;
}

core::ElementIndex Stencil2::index(h_index i) const {
  HERMES_ASSERT(i < elements_.size());
  return elements_[i].element_index;
}

hermes::geo::point2 Stencil2::center() const {
  HERMES_ASSERT(!elements_.empty());
  HERMES_ASSERT(geo_);
  return geo_->center(elements_[0].element_index);
}

hermes::geo::point2 Stencil2::operator[](h_index i) const {
  HERMES_ASSERT(i < elements_.size());
  HERMES_ASSERT(geo_);
  return geo_->center(elements_[i].element_index);
}

h_size Stencil2::size() const { return elements_.size(); }

real_t Stencil2::distance(h_index i, h_index j) const {
  HERMES_ASSERT(std::max(i, j) < elements_.size());
  HERMES_ASSERT(geo_);
  return hermes::geo::distance(geo_->center(elements_[i].element_index),
                               geo_->center(elements_[j].element_index));
}

real_t Stencil2::delta(derivative_bits d, h_index i) const {
  if (i == 0)
    return 0.0;
  HERMES_ASSERT(i < elements_.size());
  HERMES_ASSERT(geo_);
  h_index dimension = 0;
  if (d == derivative_bits::y)
    dimension = 1;
  else if (d == derivative_bits::z)
    dimension = 2;
  else if (d != derivative_bits::x) {
    HERMES_ERROR("invalid derivative bit value");
  }
  return geo_->center(elements_[0].element_index)[dimension] -
         geo_->center(elements_[i].element_index)[dimension];
}

} // namespace naiades::numeric
