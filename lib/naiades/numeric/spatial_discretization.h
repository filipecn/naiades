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
/// \date   2026-17-01
/// \brief  Spatial Discretization interface.

#pragma once

#include <naiades/core/element.h>
#include <naiades/numeric/boundary.h>
#include <naiades/numeric/discrete_operator.h>

#include <hermes/core/ref.h>
#include <hermes/geometry/point.h>

namespace naiades::numeric {

enum class derivative_bits : u32 {
  none = 0,
  x = 1 << 0,
  y = 1 << 1,
  z = 1 << 2,
  xy = 1 << 3,
  yx = 1 << 4,
  xz = 1 << 5,
  zx = 1 << 6,
  yz = 1 << 7,
  zy = 1 << 8,
  xx = 1 << 9,
  yy = 1 << 10,
  zz = 1 << 11,
  n = 1 << 12,
  custom = 1 << 13
};

using derivatives = hermes::Flags<numeric::derivative_bits>;

} // namespace naiades::numeric

namespace hermes {

template <> struct FlagTraits<naiades::numeric::derivative_bits> {
  static HERMES_CONST_OR_CONSTEXPR bool is_bitmask = true;
  static HERMES_CONST_OR_CONSTEXPR naiades::numeric::derivatives all_flags =
      naiades::numeric::derivative_bits::x |
      naiades::numeric::derivative_bits::y |
      naiades::numeric::derivative_bits::z |
      naiades::numeric::derivative_bits::xy |
      naiades::numeric::derivative_bits::yx |
      naiades::numeric::derivative_bits::xz |
      naiades::numeric::derivative_bits::zx |
      naiades::numeric::derivative_bits::yz |
      naiades::numeric::derivative_bits::zy |
      naiades::numeric::derivative_bits::xx |
      naiades::numeric::derivative_bits::yy |
      naiades::numeric::derivative_bits::zz |
      naiades::numeric::derivative_bits::n |
      naiades::numeric::derivative_bits::custom;
};

} // namespace hermes

namespace naiades::numeric {

class Boundary;

/// \brief Spatial discretization.
class SpatialDiscretization {
public:
  using Ptr = hermes::Ref<SpatialDiscretization>;
  /// Compute the derivative operator centered at the given element.
  /// \param d Derivative direction.
  /// \param loc
  /// \param index
  /// \param boundary_loc
  /// \param boundary
  virtual DiscreteOperator derivative(derivative_bits d,
                                      const core::Element &loc, h_size index,
                                      core::Element boundary_loc,
                                      const Boundary &boundary) const = 0;
  /// Compute the discrete Laplacian operator centered at the given element.
  /// \param loc
  /// \param index
  /// \param boundary_loc
  /// \param boundary
  virtual DiscreteOperator laplacian(const core::Element &loc, h_size index,
                                     core::Element boundary_loc,
                                     const Boundary &boundary) const = 0;
  /// Compute the discrete Divergence operator centered at the given element.
  /// \tparam DiscretizationType Discretization type.
  /// \param boundary
  /// \param loc
  /// \param index
  virtual DiscreteOperator divergence(const core::Element &loc, h_size index,
                                      const core::Element &vector_loc,
                                      bool staggered) const = 0;
  /// Compute boundary stencils for all boundaries.
  NaResult resolveBoundaries();
  /// Compute boundary stencils for the boundaries of the given field.
  virtual NaResult resolveBoundary(const std::string &field_name) = 0;
  /// Defines a boundary region from the given boundary element index set.
  /// \param indices
  /// \return The index of the newly created region.
  void addBoundary(const std::string &field_name, core::Element loc,
                   const std::vector<h_size> &indices,
                   h_size *region_index = nullptr);
  /// Set a boundary condition of a field for a given region index.
  /// \param field_name
  /// \param region_index
  /// \param condition
  /// \param interior_field_loc
  void setBoundaryCondition(const std::string &field_name, h_size region_index,
                            bc::BoundaryCondition::Ptr condition,
                            core::Element interior_field_loc);
  /// Set the same boundary condition of a field in all regions.
  /// \param field_name
  /// \param condition
  /// \param interior_field_loc
  void setBoundaryCondition(const std::string &field_name,
                            bc::BoundaryCondition::Ptr condition,
                            core::Element interior_field_loc);
  ///
  const Boundary &boundary(const std::string &field_name) const;
  ///
  Boundary &boundary(const std::string &field_name);

protected:
  std::unordered_map<std::string, Boundary> boundaries_;
};

} // namespace naiades::numeric
