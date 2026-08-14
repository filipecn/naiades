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

#include <naiades/core/geometry.h>
#include <naiades/core/symbol.h>
#include <naiades/core/topology.h>
#include <naiades/numeric/blas.h>
#include <naiades/numeric/boundary.h>
#include <naiades/numeric/discrete_expression.h>

#include <hermes/core/ref.h>
#include <hermes/geometry/point.h>

namespace naiades::numeric {

template <typename T>
concept CanDifferentiate =
    requires(T object, const core::ElementIndex &iloc) { object.center(iloc); };

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
template <typename DiscretizationStructureType,
          typename DifferentiationMethodType>
  requires HasPtr<DiscretizationStructureType> &&
           core::HasGeometry<DiscretizationStructureType> &&
           core::HasElementSet<DiscretizationStructureType> &&
           core::HasTopology<DiscretizationStructureType> &&
           HasPtr<DifferentiationMethodType>
class SpatialDiscretization {
public:
  using Ptr = hermes::Ref<SpatialDiscretization<DiscretizationStructureType,
                                                DifferentiationMethodType>>;

  SpatialDiscretization(typename DiscretizationStructureType::Ptr structure)
      : structure_{structure} {
    diff_ = DifferentiationMethodType::Ptr::shared();
  }

  DifferentiationMethodType &differentiationMethod() { return *diff_; }

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
    field_group.setIndexOffset(structure_->elementIndexOffset(symbol.loc));
    field_group.pushField<T>("value");
    NAIADES_HE_RETURN_BAD_RESULT(
        field_group.resize(structure_->elementCount(symbol.loc)));
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
  NaResult resolveBoundaries() {
    for (auto &item : boundaries_)
      NAIADES_RETURN_BAD_RESULT(resolveBoundary(item.first));
    return NaResult::noError();
  }
  /// Compute boundary stencils for the boundaries of the given field.
  NaResult resolveBoundary(const core::Symbol &symbol) {
    auto it = boundaries_.find(symbol);
    if (it == boundaries_.end())
      return NaResult::notFound();
    NAIADES_RETURN_BAD_RESULT(it->second.resolve(structure_));
    return NaResult::noError();
  }
  /// Defines a boundary region from the given boundary element index set.
  /// \param symbol
  /// \param indices
  /// \return The index of the newly created region.
  void addBoundary(const core::Symbol &symbol, const IndexSet &indices,
                   h_size *region_index = nullptr) {
    boundaries_[symbol].addRegion(indices, region_index);
  }
  /// Set a boundary condition of a field for a given region index.
  /// \param symbol
  /// \param region_index
  /// \param condition
  void setBoundaryCondition(const core::Symbol &symbol, h_size region_index,
                            bc::BoundaryCondition::Ptr condition) {
    boundaries_[symbol].setCondition(region_index, condition);
  }
  /// Set the same boundary condition of a field in all regions.
  /// \param field_name
  /// \param condition
  /// \param interior_field_loc
  void setBoundaryCondition(const core::Symbol &symbol,
                            bc::BoundaryCondition::Ptr condition) {
    boundaries_[symbol].setCondition(condition);
  }
  ///
  const Boundary &boundary(const core::Symbol &symbol) const {
    static Boundary s_dummy({});
    auto it = boundaries_.find(symbol);
    if (it == boundaries_.end())
      return s_dummy;
    return it->second;
  }
  ///
  Boundary &boundary(const core::Symbol &symbol) {
    static Boundary s_dummy({});
    auto it = boundaries_.find(symbol);
    if (it == boundaries_.end())
      return s_dummy;
    return it->second;
  }
  ///
  const std::unordered_map<core::Symbol, Boundary> &boundaries() const {
    return boundaries_;
  }

  // geometry

  Scalar x(const core::Element &loc) const {
    auto indices = structure_->indices(loc);
    HERMES_LOG_VARIABLE(indices);
    Scalar values(indices.size());
    for (auto index : indices) {
      values[index.local_set_index] = structure_->center(
          core::ElementIndex::global(loc, index.global_index))[0];
    }
    return values;
  }

  // discrete operators

  ///
  DiscreteExpression dx(const core::DiscreteSymbol &dsym) const {
    return derivative(derivative_bits::x, dsym);
  }
  DiscreteExpression dy(const core::DiscreteSymbol &dsym) const {
    return derivative(derivative_bits::y, dsym);
  }
  DiscreteExpression derivative(derivative_bits d,
                                const core::DiscreteSymbol &dsym) const {
    // sanity error checks
    auto it = boundaries_.find(dsym.boundary_symbol);
    HERMES_ASSERT(it != boundaries_.end() && structure_);
    // get boundary
    auto &boundary = it->second;

    DiscreteExpression de(dsym);
    auto n = structure_->elementCount(dsym.symbol.loc);
    for (h_index i = 0; i < n; ++i) {
      de.addIndexEntry(i, diff_->derivative(structure_, boundary, d, i, dsym));
    }
    return de;
  }
  DiscreteExpression L(const core::DiscreteSymbol &dsym) const {
    // sanity error checks
    auto it = boundaries_.find(dsym.boundary_symbol);
    HERMES_ASSERT((it != boundaries_.end() && structure_));
    // get boundary
    auto &boundary = it->second;

    DiscreteExpression de(dsym);
    auto n = structure_->elementCount(dsym.symbol.loc);
    for (h_index i = 0; i < n; ++i) {
      de.addIndexEntry(i, diff_->laplacian(structure_, boundary, i, dsym));
    }
    return de;
  }

protected:
  std::unordered_map<core::Symbol, Boundary> boundaries_;
  std::unordered_map<core::Symbol, core::FieldGroup> fields_;
  DiscretizationStructureType::Ptr structure_;
  DifferentiationMethodType::Ptr diff_;

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS
  friend struct hermes::DebugTraits<SpatialDiscretization<
      DiscretizationStructureType, DifferentiationMethodType>>;
#endif
};

} // namespace naiades::numeric

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS
namespace hermes {

template <typename DiscretizationStructureType,
          typename DifferentiationMethodType>
struct DebugTraits<naiades::numeric::SpatialDiscretization<
    DiscretizationStructureType, DifferentiationMethodType>> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage
  message(const naiades::numeric::SpatialDiscretization<
          DiscretizationStructureType, DifferentiationMethodType> &data) {
    auto m = DebugMessage();
    m.addTitle("Grid2 - FD");
    if (data.structure_)
      m.add("mesh", *data.structure_);
    m.addMap("fields", data.fields_);
    m.addMap("boundaries", data.boundaries_);
    return m;
  }
};

} // namespace hermes

#endif
