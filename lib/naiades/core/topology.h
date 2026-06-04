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

/// \file   neighbourhood.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-17-01
/// \brief  Stencil interface.

#pragma once

#include <naiades/base/result.h>
#include <naiades/core/element.h>
#include <naiades/core/element_set.h>

#include <hermes/core/ref.h>
#include <hermes/geometry/point.h>

#include <optional>
#include <vector>

namespace naiades::core {

/// \brief Discretization neighbour.
struct Neighbour {
  ElementIndex element_index;
  real_t distance;
};

/// \brief Interface for discretization neighbourhoods.
/// A stencil represents a set of discretization elements grouped by some
/// criteria, such as neighbourhood.
class Topology : public ElementSet {
public:
  using Ptr = hermes::Ref<Topology>;

  // elements

  /// \brief Get the lists of indices of an element type.
  /// Elements may consist of a set of different sub-elements.
  /// Ex: A polygonal cell contains a set of vertices and a set of faces.
  /// \param element
  /// \param sub_element
  /// \return The lists of sub-elements of all elements.
  virtual std::vector<std::vector<h_size>> indices(Element element,
                                                   Element sub_element) const;
  /// \brief Get the list of indices of a given element instance.
  /// \param element
  /// \param index
  /// \param sub_element
  /// \return The lists of sub-elements of the given element instance.
  virtual std::vector<h_size> indices(const ElementIndex &iloc,
                                      Element sub_element) const = 0;

  // boundary

  /// Get the indices of an element type  at the boundary.
  /// \note This returns a copy of the indices.
  /// \param loc Element.
  /// \return A Boundary object with the indices and groups of the boundary
  ///         elements.
  virtual std::vector<h_size> boundaryIndices(Element loc) const = 0;
  /// \param iloc Element location index.
  /// \return True if this is the index of a boundary element.
  virtual bool isBoundary(const ElementIndex &iloc) const = 0;

  // neighbourhood

  /// \brief The star neighbourhood of a given element.
  /// The star consists of elements directly connected to a central element.
  /// \param iloc Center element index.
  /// \param star_loc Star elements location.
  /// \param boundary_loc Boundary elements included in the star.
  /// \return List of neighbours of the given element.
  virtual std::vector<Neighbour>
  star(const ElementIndex &iloc, Element star_loc,
       std::optional<Element> boundary_loc) const = 0;
  std::vector<Neighbour> star(const ElementIndex &iloc,
                              Element boundary_loc) const;
  /// The k-ring consists of the set of concentric elements
  /// surrounding the central element.
  /// \param iloc Center element index.
  /// \param k ring topological radius.
  /// \param boundary_loc Boundary elements included in the ring.
  /// \return List of neighbours of the given element.
  virtual std::vector<Neighbour>
  k_ring(const ElementIndex &iloc, h_size k, Element ring_loc,
         std::optional<Element> boundary_loc) const = 0;
  /// \brief The direct neighbourhood of elements for a given element.
  /// The neighborhood (here, the 1-neighbour) consists of the set of elements
  /// of topological distance of 1.
  /// \param iloc Center element index.
  /// \param radius Topological distance.
  /// \param neighbour_loc neighbour element type.
  /// \return List of pairs neighbour <index, distance> of the given element.
  virtual std::vector<std::pair<h_size, real_t>>
  neighbours(const ElementIndex &iloc, h_size radius, Element neighbour_loc,
             std::optional<core::Element> boundary_loc) const = 0;
  /// \param boundary_element
  /// \param interior_loc
  virtual h_size interiorNeighbour(const ElementIndex &boundary_element,
                                   const Element &interior_loc) const = 0;
};

} // namespace naiades::core

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS

namespace hermes {

template <> struct DebugTraits<naiades::core::Neighbour> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::core::Neighbour &data) {
    return DebugMessage().addFmt(
        "[{} dist: {}]", hermes::to_string(data.element_index), data.distance);
  }
};

} // namespace hermes

#endif
