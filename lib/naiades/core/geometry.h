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

/// \file   geometry.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Geometry interface.

#pragma once

#include <naiades/base/result.h>
#include <naiades/core/element.h>
#include <naiades/core/element_set.h>
#include <naiades/numeric/blas.h>

#include <hermes/geometry/bounds.h>
#include <hermes/geometry/normal.h>
#include <hermes/geometry/point.h>

#include <vector>

namespace naiades::core {

template <typename T>
concept HasGeometry =
    requires(T object, const ElementIndex &iloc) { object.center(iloc); };

/// \brief Interface for discretization 2-dimensional geometries.
/// A derived discretization geometry holds the geometry of a discretization
/// that is commonly required by simulation algorithms. The discretization
/// geometry may have the positions of different discretization elements, such
/// as centers of cells and faces.
class Geometry2 : virtual public ElementSet {
public:
  using Ptr = hermes::Ref<Geometry2>;

  class element_view;
  ///
  class iterator {
  public:
    struct ElementInstance {
      hermes::geo::point2 center;
      h_index local_index;
      h_index global_index;
      Element element;
      ElementIndex globalIndex() const;
      ElementIndex localIndex() const;
    };

    ElementInstance operator*() const;

    iterator &operator++();
    bool operator==(const iterator &rhs) const;

  private:
    friend class element_view;
    iterator(const Geometry2 *geometry, const ElementIndex &iloc);

    const Geometry2 *geo_;
    ElementIndex iloc_;
  };

  class element_view {
  public:
    iterator begin() const;
    iterator end() const;

  private:
    friend class Geometry2;
    element_view(const Geometry2 *geometry, const Element &loc);

    const Geometry2 *geo_;
    Element loc_;
  };

  /// \param loc Element filter.
  /// \return View for iterating over instances of the given element.
  element_view elements(const Element &loc) const;

  /// \param loc Element type.
  /// \param space Element space.
  /// \note The element space defines the what elements are returned.
  /// \note - If ElementSpace::INTERIOR is given, only interior elements are
  /// returned.
  /// \note - If ElementSpace::BOUNDARY is given, only boundary elements are
  /// returned.
  /// \note - If ElementSpace::GLOBAL is given, all elements are returned.
  /// \return center's x coordinate.
  numeric::Scalar x(const Element &loc) const;
  /// \param loc Element type.
  /// \param space Element space.
  /// \note The element space defines the what elements are returned.
  /// \note - If ElementSpace::INTERIOR is given, only interior elements are
  /// returned.
  /// \note - If ElementSpace::BOUNDARY is given, only boundary elements are
  /// returned.
  /// \note - If ElementSpace::GLOBAL is given, all elements are returned.
  /// \return center's y coordinate.
  numeric::Scalar y(const Element &loc) const;

  /// \return The bounding box containing the whole geometry.
  virtual hermes::geo::bounds::bbox2 bbounds() const = 0;
  /// Get the position of an element center.
  /// \note This returns a copy of the position.
  /// \param iloc Element index.
  /// \param index Element index.
  /// \return The element center position in world coordinates.
  virtual hermes::geo::point2 center(const ElementIndex &iloc) const = 0;
  /// Get the flat list of center positions of an element type.
  /// \note The indices of the list match the element index.
  /// \note This returns a copy of the list.
  /// \param loc Element.
  virtual std::vector<hermes::geo::point2> centers(Element loc) const = 0;
  /// Get the normal defined at the given element instance.
  /// \param iloc Element index.
  /// \return The normal defined at (loc, index) or a null-vector if not found.
  virtual hermes::geo::normal2 normal(const ElementIndex &iloc) const = 0;
};

} // namespace naiades::core
