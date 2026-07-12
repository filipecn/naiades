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

/// \file   geometry.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-07-11

#include <naiades/core/geometry.h>

namespace naiades::core {

ElementIndex Geometry2::iterator::ElementInstance::globalIndex() const {
  return ElementIndex::global(element, global_index);
}

ElementIndex Geometry2::iterator::ElementInstance::localIndex() const {
  return ElementIndex::local(element, local_index);
}

Geometry2::iterator::ElementInstance Geometry2::iterator::operator*() const {
  return {.center = geo_->center(iloc_),
          .local_index = *(geo_->localIndex(iloc_).index),
          .global_index = *(geo_->globalIndex(iloc_).index),
          .element = iloc_.element};
}

Geometry2::iterator &Geometry2::iterator::operator++() {
  ++iloc_;
  return *this;
}

bool Geometry2::iterator::operator==(const Geometry2::iterator &rhs) const {
  return iloc_ == rhs.iloc_;
}

Geometry2::iterator::iterator(const Geometry2 *geometry,
                              const ElementIndex &iloc)
    : geo_{geometry}, iloc_{iloc} {}

Geometry2::iterator Geometry2::element_view::begin() const {
  return Geometry2::iterator(geo_, {loc_, Index::local(0)});
}

Geometry2::iterator Geometry2::element_view::end() const {
  return Geometry2::iterator(geo_,
                             {loc_, Index::local(geo_->elementCount(loc_))});
}

Geometry2::element_view::element_view(const Geometry2 *geometry,
                                      const Element &loc)
    : geo_{geometry}, loc_{loc} {}

Geometry2::element_view Geometry2::elements(const Element &loc) const {
  return Geometry2::element_view(this, loc);
}

numeric::Scalar Geometry2::x(const Element &loc) const {
  numeric::Scalar values(elementCount(loc));
  for (auto e : this->elements(loc)) {
    values[e.global_index] = e.center.x;
  }
  return values;
}

numeric::Scalar Geometry2::y(const Element &loc) const {
  numeric::Scalar values(elementCount(loc));
  for (auto e : this->elements(loc)) {
    values[e.global_index] = e.center.y;
  }
  return values;
}

} // namespace naiades::core