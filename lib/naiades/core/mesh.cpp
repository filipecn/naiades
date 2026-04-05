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

/// \file   mesh.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-03-28

#include <naiades/core/mesh.h>

namespace naiades::core {

Mesh2::iterator::ElementInstance Mesh2::iterator::operator*() const {
  return {.center = mesh_->center(loc_, local_index_ +
                                            mesh_->elementIndexOffset(loc_)),
          .local_index = local_index_,
          .global_index = local_index_ + mesh_->elementIndexOffset(loc_)};
}

Mesh2::iterator &Mesh2::iterator::operator++() {
  local_index_++;
  return *this;
}

bool Mesh2::iterator::operator==(const Mesh2::iterator &rhs) const {
  return local_index_ == rhs.local_index_ && loc_ == rhs.loc_;
}

Mesh2::iterator::iterator(const Mesh2 *mesh, const Element &loc, h_index index)
    : mesh_{mesh}, loc_{loc}, local_index_{index} {}

Mesh2::iterator Mesh2::element_view::begin() const {
  return Mesh2::iterator(mesh_, loc_, 0);
}

Mesh2::iterator Mesh2::element_view::end() const {
  return Mesh2::iterator(mesh_, loc_, mesh_->elementCount(loc_));
}

Mesh2::element_view::element_view(const Mesh2 *mesh, const Element &loc)
    : mesh_{mesh}, loc_{loc} {}

Mesh2::element_view Mesh2::elements(const Element &loc) const {
  return Mesh2::element_view(this, loc);
}

numeric::Scalar Mesh2::x(const Element &loc) const {
  numeric::Scalar values(elementCount(loc));
  for (auto e : this->elements(loc)) {
    values[e.global_index] = e.center.x;
  }
  return values;
}

numeric::Scalar Mesh2::y(const Element &loc) const {
  numeric::Scalar values(elementCount(loc));
  for (auto e : this->elements(loc)) {
    values[e.global_index] = e.center.y;
  }
  return values;
}

} // namespace naiades::core