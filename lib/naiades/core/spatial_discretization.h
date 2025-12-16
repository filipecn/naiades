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

/// \file   spatial_discretization.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Spatial Discretization interface.

#pragma once

#include <naiades/base/result.h>
#include <naiades/core/element.h>

#include <hermes/core/ref.h>
#include <hermes/geometry/point.h>

#include <unordered_map>
#include <vector>

namespace naiades::core {

struct Boundary {
  /// element indices
  std::unordered_map<element_alignments, std::vector<h_size>> indices;
  /// section name -> (start, count)
  std::unordered_map<std::string, std::pair<h_size, h_size>> sections;
};

/// A simulation mesh holds the geometry and topology of the discretization and
/// other information required by simulation algorithms.
/// \note Mesh can only be setup by friend GeometryType classes.
class SpatialDiscretization2 {
public:
  SpatialDiscretization2() noexcept {}
  virtual ~SpatialDiscretization2() noexcept {}

  /// \param loc
  /// \return Total number of locations of a given element.
  virtual h_size elementCount(Element loc) const = 0;
  /// Get the flat list of positions of an element type.
  /// \note The indices of the list match the element index.
  /// \note This returns a copy of the list.
  /// \param loc Element.
  virtual std::vector<hermes::geo::point2> positions(Element loc) const = 0;
  /// \brief Get the lists of indices of an element type.
  /// Elements may consist of a set of different sub-elements.
  /// Ex: A polygonal cell contains a set of vertices and a set of faces.
  /// \param element
  /// \param sub_element
  /// \return The lists of sub-elements of all elements.
  virtual std::vector<std::vector<h_size>>
  indices(Element element, Element sub_element) const = 0;
  /// Get the position of an element.
  /// \note This returns a copy of the position.
  /// \param loc Element.
  /// \param index Element index.
  /// \return The element position in world coordinates.
  virtual hermes::geo::point2 position(Element loc, h_size index) const = 0;
  /// Get the indices of an element at the boundary.
  /// \note This returns a copy of the indices.
  /// \param loc Element.
  /// \return A Boundary object with the indices and groups of the boundary
  ///         elements.
  virtual std::vector<h_size> boundary(Element loc) const = 0;
  /// Get element alignment.
  /// \note This considers the element primitive.
  /// \param loc Element.
  /// \param index Element index.
  /// \return The element alignment at the given index.
  virtual element_alignments elementAlignment(Element loc,
                                              h_size index) const = 0;
};

} // namespace naiades::core
