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

/// \file   point_set.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-07-06

#include <naiades/geo/point_set.h>

namespace naiades::geo {

h_size PointSet2::size() const { return points_.size(); }

void PointSet2::applyTransform(const hermes::geo::Transform2 &t) {
  bounds_ = hermes::geo::bounds::bbox2();
  for (auto &point : points_) {
    point = t(point);
    bounds_ += point;
  }
}

h_size PointSet2::elementCount(core::Element loc) const {
  if (loc.is(core::element_primitive_bits::point))
    return points_.size();
  return 0;
}

h_size PointSet2::elementIndexOffset(core::Element loc) const {
  HERMES_UNUSED_VARIABLE(loc);
  return 0;
}

core::element_alignments
PointSet2::elementAlignment(const core::ElementIndex &loci) const {
  HERMES_UNUSED_VARIABLE(loci);
  return core::element_alignment_bits::any;
}

core::element_orientations
PointSet2::elementOrientation(const core::ElementIndex &iloc) const {
  HERMES_UNUSED_VARIABLE(iloc);
  return core::element_orientation_bits::any;
}

hermes::geo::bounds::bbox2 PointSet2::bbounds() const { return bounds_; }

hermes::geo::point2 PointSet2::center(const core::ElementIndex &iloc) const {
  return points_[*iloc.index];
}
std::vector<hermes::geo::point2> PointSet2::centers(core::Element loc) const {
  if (loc.is(core::element_primitive_bits::point))
    return points_;
  return {};
}

hermes::geo::normal2 PointSet2::normal(const core::ElementIndex &iloc) const {
  HERMES_UNUSED_VARIABLE(iloc);
  return hermes::geo::normal2(0, 1);
}

} // namespace naiades::geo
