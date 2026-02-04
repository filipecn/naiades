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

/// \file   topology.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Topology interface.

#pragma once

#include <naiades/base/result.h>
#include <naiades/core/element.h>

#include <hermes/core/ref.h>
#include <hermes/geometry/point.h>

#include <vector>

namespace naiades::core {

/// \brief Interface for discretization topologies.
/// A derived topology holds the topology of a discretization that is commonly
/// required by simulation algorithms. The discretization topology may have the
/// relationship of different element types.
class Topology {
public:
  using Ptr = hermes::Ref<Topology>;

  Topology() noexcept {}
  virtual ~Topology() noexcept {}

  /// \param loc
  /// \return Total number of locations of a given element.
  virtual h_size elementCount(Element loc) const = 0;
  /// \brief Get the lists of indices of an element type.
  /// Elements may consist of a set of different sub-elements.
  /// Ex: A polygonal cell contains a set of vertices and a set of faces.
  /// \param element
  /// \param sub_element
  /// \return The lists of sub-elements of all elements.
  virtual std::vector<std::vector<h_size>>
  indices(Element element, Element sub_element) const = 0;
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
  /// Get element orientation.
  /// \note This considers the element primitive.
  /// \param loc Element.
  /// \param index Element index.
  /// \return The element orientation at the given index.
  virtual element_orientations elementOrientation(Element loc,
                                                  h_size index) const = 0;
  /// \param loc Element.
  /// \param index Element index.
  /// \return True if this is the index of a boundary element.
  virtual bool isBoundary(Element loc, h_size index) const = 0;
  /// \param boundary_element
  /// \param interior_loc
  virtual h_size interiorNeighbour(const ElementIndex &boundary_element,
                                   const Element &interior_loc) const = 0;
};

} // namespace naiades::core
