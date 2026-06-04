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

namespace naiades::core {

/// \brief Interface for discretization topologies.
/// A derived topology holds the topology of a discretization that is commonly
/// required by simulation algorithms. The discretization topology may have the
/// relationship of different element types.
class ElementSet {
public:
  using Ptr = hermes::Ref<ElementSet>;

  ElementSet() noexcept {}
  virtual ~ElementSet() noexcept {}

  /// \param loc
  /// \return Total number of locations of a given element.
  virtual h_size elementCount(Element loc) const = 0;
  /// Some structures may store elements in single sequences so element
  /// indices may have an offset.
  /// @param loc
  virtual h_size elementIndexOffset(Element loc) const = 0;
  /// Get element alignment.
  /// \note This considers the element primitive.
  /// \param iloc Element location index.
  /// \return The element alignment at the given index.
  virtual element_alignments
  elementAlignment(const ElementIndex &loci) const = 0;
  /// Get element orientation.
  /// \note This considers the element primitive.
  /// \param iloc Element location index.
  /// \return The element orientation at the given index.
  virtual element_orientations
  elementOrientation(const ElementIndex &iloc) const = 0;

  /// Compute the local index of the given global element index.
  /// \param iloc Element location index.
  ElementIndex localIndex(const ElementIndex &iloc) const;
  /// Compute the global index of the given local element index.
  /// \param iloc Element location index.
  ElementIndex globalIndex(const ElementIndex &iloc) const;
};

} // namespace naiades::core
