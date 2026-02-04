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

#include <hermes/core/ref.h>
#include <hermes/geometry/point.h>

#include <vector>

namespace naiades::core {

/// \brief Discretization neighbour.
struct Neighbour {
  ElementIndex element_index;
  real_t distance;

  NAIADES_to_string_FRIEND(Neighbour);
};

/// \brief Interface for discretization neighbourhoods.
/// A stencil represents a set of discretization elements grouped by some
/// criteria, such as neighbourhood.
class NeighbourhoodSet {
public:
  /// The star neighbourhood of a given element.
  /// \param loc Element type.
  /// \param index Center index.
  /// \param boundary_loc Boundary elements included in the star.
  /// \return List of neighbours of the given element.
  virtual std::vector<Neighbour> star(Element loc, h_size index,
                                      Element boundary_loc) const = 0;
  /// The direct neighbourhood of elements for a given element.
  /// \param loc Element type.
  /// \param index Center index.
  /// \param neighbour_loc neighbour element type.
  /// \return List of pairs neighbour <index, distance> of the given element.
  virtual std::vector<std::pair<h_size, real_t>>
  neighbours(Element loc, h_size index, Element neighbour_loc) const = 0;
};

} // namespace naiades::core
