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
#include <naiades/utils/utils.h>

#include <hermes/core/ref.h>
#include <hermes/geometry/point.h>

#include <concepts>
#include <optional>
#include <vector>

namespace naiades::core {

template <typename T>
concept HasTopology = requires(T t, Element element, const ElementIndex &iloc,
                               std::optional<Element> b_loc) {
  { t.neighbours(iloc, b_loc) } -> std::same_as<std::vector<ElementIndex>>;
  { t.indices(iloc.element) } -> std::same_as<IndexSet>;
};

/// \brief Discretization neighbour.
struct Neighbour {
  ElementIndex element_index;
  real_t distance;
};

struct NeighbourhoodCriteria {
  static NeighbourhoodCriteria knn(h_index n);
  static NeighbourhoodCriteria k_ring(h_index k);

  NeighbourhoodCriteria &withMaxCount(h_index max_count);
  NeighbourhoodCriteria &withMaxDistance(f32 max_distance);
  NeighbourhoodCriteria &withSortByDistance();

  h_index max_topological_distance{1};
  h_index max_count{0};
  real_t max_distance{-1.f};
  bool sort_by_distance{false};
};

/// \brief Interface for discretization neighbourhoods.
/// A stencil represents a set of discretization elements grouped by some
/// criteria, such as neighbourhood.
class Topology : virtual public ElementSet {
public:
  using Ptr = hermes::Ref<Topology>;

  // elements

  /// \brief Get the lists of indices of an element type.
  /// Elements may consist of a set of different sub-elements.
  /// Ex: A polygonal cell contains a set of vertices and a set of faces.
  /// \param element
  /// \param sub_element
  /// \return The lists of sub-elements of all elements.
  virtual std::vector<std::vector<h_size>>
  subElements(Element element, Element sub_element) const;
  /// \brief Get the list of indices of a given element instance.
  /// \param element
  /// \param index
  /// \param sub_element
  /// \return The lists of sub-elements of the given element instance.
  virtual std::vector<h_size> elementIndices(const ElementIndex &iloc,
                                             Element sub_element) const = 0;

  // boundary

  /// \param iloc Element location index.
  /// \return True if this is the index of a boundary element.
  virtual bool isBoundary(const ElementIndex &iloc) const = 0;

  // neighbourhood

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
