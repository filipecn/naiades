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

#include <naiades/core/symbol.h>
#include <naiades/numeric/boundary.h>
#include <naiades/numeric/discrete_expression.h>

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
/// The spatial discretization provides the setup of fields and discrete
/// operators over a numeric geometry/topology.
class SpatialDiscretization {
public:
  using Ptr = hermes::Ref<SpatialDiscretization>;

  // fields

  template <typename T>
  NaResult addFields(const core::Element &loc,
                     const std::vector<std::string> &field_names) {
    for (const auto &name : field_names)
      NAIADES_RETURN_BAD_RESULT(addField<T>({.name = name, .loc = loc}));
    return NaResult::noError();
  }

  template <typename T>
  NaResult addFields(const std::vector<core::Symbol> &symbols) {
    for (const auto &symbol : symbols)
      NAIADES_RETURN_BAD_RESULT(addField<T>(symbol));
    return NaResult::noError();
  }

  template <typename T> NaResult addField(const core::Symbol &symbol) {
    core::FieldGroup field_group;
    field_group.setElement(symbol.loc);
    field_group.setIndexOffset(topology_->elementIndexOffset(symbol.loc));
    field_group.pushField<T>("value");
    NAIADES_HE_RETURN_BAD_RESULT(
        field_group.resize(topology_->elementCount(symbol.loc)));
    if (fields_.count(symbol))
      HERMES_WARN("Overwriting field {} in field set.", symbol.name);
    fields_[symbol] = std::move(field_group);
    return NaResult::noError();
  }

  /// \param name Field group name.
  /// \return Field reference or NOT_FOUND error.
  template <typename T>
  Result<core::FieldRef<T>> getField(const core::Symbol &symbol) {
    auto it = fields_.find(symbol);
    if (it != fields_.end())
      return Result<core::FieldRef<T>>(it->second.get<T>(0));
    return NaResult::notFound();
  }

  /// \param name Field group name.
  /// \return Field const reference or NOT_FOUND error.
  template <typename T>
  Result<core::FieldCRef<T>> getField(const core::Symbol &symbol) const {
    auto it = fields_.find(symbol);
    if (it != fields_.end())
      return Result<core::FieldCRef<T>>(it->second.get<T>(0));
    return NaResult::notFound();
  }

  // boundaries

  /// Compute boundary stencils for all boundaries.
  NaResult resolveBoundaries();
  /// Compute boundary stencils for the boundaries of the given field.
  NaResult resolveBoundary(const core::Symbol &symbol);
  /// Defines a boundary region from the given boundary element index set.
  /// \param symbol
  /// \param indices
  /// \return The index of the newly created region.
  void addBoundary(const core::Symbol &symbol,
                   const std::vector<h_size> &indices,
                   h_size *region_index = nullptr);
  /// Set a boundary condition of a field for a given region index.
  /// \param symbol
  /// \param region_index
  /// \param condition
  void setBoundaryCondition(const core::Symbol &symbol, h_size region_index,
                            bc::BoundaryCondition::Ptr condition);
  /// Set the same boundary condition of a field in all regions.
  /// \param field_name
  /// \param condition
  /// \param interior_field_loc
  void setBoundaryCondition(const core::Symbol &symbol,
                            bc::BoundaryCondition::Ptr condition);
  ///
  const Boundary &boundary(const core::Symbol &symbol) const;
  ///
  Boundary &boundary(const core::Symbol &symbol);
  ///
  const std::unordered_map<core::Symbol, Boundary> &boundaries() const;

  // discrete operators

  ///
  DiscreteExpression dx(const core::DiscreteSymbol &dsym) const;
  DiscreteExpression dy(const core::DiscreteSymbol &dsym) const;
  DiscreteExpression L(const core::DiscreteSymbol &dsym) const;

protected:
  /// Compute the derivative operator centered at the given element.
  /// \param d Derivative direction.
  /// \param index
  /// \param sym
  virtual DiscreteOperator
  derivative(derivative_bits d, h_size index,
             const core::DiscreteSymbol &sym) const = 0;
  /// Compute the discrete Laplacian operator centered at the given element.
  /// \param index
  /// \param sym
  virtual DiscreteOperator laplacian(h_size index,
                                     const core::DiscreteSymbol &sym) const = 0;
  /// Compute the discrete Divergence operator centered at the given element.
  /// \tparam DiscretizationType Discretization type.
  /// \param boundary
  /// \param loc
  /// \param index
  virtual DiscreteOperator divergence(const core::Element &loc, h_size index,
                                      const core::Element &vector_loc,
                                      bool staggered) const = 0;

protected:
  std::unordered_map<core::Symbol, Boundary> boundaries_;
  std::unordered_map<core::Symbol, core::FieldGroup> fields_;
  core::Topology::Ptr topology_;
};

} // namespace naiades::numeric
