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

/// \file   point_set.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-07-06

#include <naiades/core/geometry.h>

#include <hermes/geometry/transform.h>

#pragma once

namespace naiades::geo {

class PointSet2 : public naiades::core::Geometry2 {
public:
  using Ptr = hermes::Ref<PointSet2>;

  template <class... Args> void emplace_back(Args &&...args) {
    points_.emplace_back(std::forward<Args>(args)...);
    if (points_.size() == 1) {
      bounds_ = hermes::geo::bounds::bbox2(points_.back(), points_.back());
    } else {
      bounds_ += points_.back();
    }
  }

  h_size size() const;
  void applyTransform(const hermes::geo::Transform2 &t);

  // element set interface

  /// \param loc
  /// \return Total number of locations of a given element.
  h_size elementCount(core::Element loc) const override;
  /// Some structures may store elements in single sequences so element
  /// indices may have an offset.
  /// @param loc
  h_size elementIndexOffset(core::Element loc) const override;
  /// Get element alignment.
  /// \note This considers the element primitive.
  /// \param iloc Element location index.
  /// \return The element alignment at the given index.
  core::element_alignments
  elementAlignment(const core::ElementIndex &loci) const override;
  /// Get element orientation.
  /// \note This considers the element primitive.
  /// \param iloc Element location index.
  /// \return The element orientation at the given index.
  core::element_orientations
  elementOrientation(const core::ElementIndex &iloc) const override;

  // geometry interface

  hermes::geo::bounds::bbox2 bbounds() const override;
  hermes::geo::point2 center(const core::ElementIndex &iloc) const override;
  std::vector<hermes::geo::point2> centers(core::Element loc) const override;
  hermes::geo::normal2 normal(const core::ElementIndex &iloc) const override;

private:
  hermes::geo::bounds::bbox2 bounds_;
  std::vector<hermes::geo::point2> points_;

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS
  friend struct hermes::DebugTraits<PointSet2>;
#endif
};
} // namespace naiades::geo

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS

namespace hermes {

template <> struct DebugTraits<naiades::geo::PointSet2> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::geo::PointSet2 &data) {
    auto m = DebugMessage();
    m.add("bounds", data.bounds_);
    m.addArray("points", data.points_);
    return m;
  }
};

} // namespace hermes

#endif