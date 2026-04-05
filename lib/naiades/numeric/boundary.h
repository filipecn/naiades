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

/// \file   boundary.h
/// \author FilipeCN (filipedecn/ggmail.com)
/// \date   2025-06-07
/// \brief  Boundary conditions.

#pragma once

#include <naiades/core/field.h>
#include <naiades/numeric/boundary_conditions.h>
#include <naiades/utils/utils.h>

namespace naiades::numeric {

/// \brief The Boundary object defines the boundary topological and numerical
///        information of a field.
///
/// The boundary can be split into index regions of the boundary element type.
/// A different boundary condition can be defined for each boundary region,
/// where implicit and explicit values are represented.
///
/// - Implicit Values: Values that build the left hand side of the system,
///                    such as coefficients in the system matrix.
/// - Explicit Values: Known values that appear on the right hand side of
///                    the system.
///
/// \note  The boundary class holds information for just one boundary element
///        type.
/// \note  The boundary conditions defined in the regions consider only one
///        interior element.
/// \note  Different boundary conditions can be applied on the boundary regions.
///
/// Example:
///   Consider three boundary regions -- [a, b, c] -- defined at the boundary
///   faces of the simulation domain.
///
///        a a a a a a
///        ____________
///     b |            | c
///     b |   Domain   | c
///     b |            | c
///        ------------
///         b b b b c c
class Boundary {
public:
  class Region {
  public:
    /// /param boundary_element_type
    /// /param indices
    Region(const core::Element &boundary_element_type,
           const std::vector<h_size> &indices);

    /// Set the boundary condition for this field.
    void setCondition(bc::BoundaryCondition::Ptr condition,
                      core::Element interior_field_loc);
    ///
    bool contains(const core::Index &index) const;
    /// Build boundary stencils.
    NaResult resolve(core::Topology::Ptr topology);
    ///
    const DiscreteOperator &stencil(const core::Index &index) const;
    ///
    NaResult compute(core::FieldCRef<f32> interior_field,
                     core::FieldRef<f32> field) const;
    /// Set of boundary element indices of this region.
    const utils::IndexSet &indices() const;

  private:
    friend class Boundary;

    utils::IndexSet index_set_;
    bc::BoundaryCondition::Ptr condition_;
    core::Element boundary_element_type_;
    core::Element interior_element_type_;
    std::vector<DiscreteOperator> stencils_;

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS
    friend struct hermes::DebugTraits<Region>;
#endif
  };

  /// Creates a boundary for face elements with cell interior elements.
  Boundary() = default;
  /// \brief
  /// \param loc Boundary element type.
  /// \param interiror_loc Interior element type.
  Boundary(const core::Element &loc, const core::Element &interior_loc);
  /// \param loc Boundary element type.
  Boundary &setBoundaryElement(const core::Element &loc);
  /// \param loc Interior element type.
  Boundary &setInteriorElement(const core::Element &loc);
  /// Defines a boundary region from the given boundary element index set.
  /// \param indices Boundary element indices.
  /// \param [out] The index of the newly created region.
  Boundary &addRegion(const std::vector<h_size> &indices,
                      h_size *region_index = nullptr);
  /// Set the boundary condition for region.
  /// \param region_index Index returned by addRegion.
  /// \param condition
  Boundary &setCondition(h_size region_index,
                         bc::BoundaryCondition::Ptr condition);
  /// Set the same boundary condition for all regions.
  /// \param condition
  Boundary &setCondition(bc::BoundaryCondition::Ptr condition);
  /// Correct field boundary elements by explicitly updating their values.
  /// \param interior_field
  /// \param boundary_field
  /// \note This only updates values at elements located at the boundary.
  NaResult compute(core::FieldCRef<f32> interior_field,
                   core::FieldRef<f32> boundary_field) const;

  NaResult resolve(core::Topology::Ptr topology);

  const core::Element &boundaryElement() const;
  const core::Element &interiorElement() const;
  const DiscreteOperator &stencil(const core::Index &index) const;
  const std::vector<Region> regions() const;

private:
  core::Element boundary_element_type_{core::Element::Type::FACE};
  core::Element interior_element_type_{core::Element::Type::CELL};
  std::vector<Region> regions_;

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS
  friend struct hermes::DebugTraits<Boundary>;
#endif
};

} // namespace naiades::numeric

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS

namespace hermes {

template <> struct DebugTraits<naiades::numeric::Boundary::Region> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::numeric::Boundary::Region &data) {
    auto m = DebugMessage();
    m.add("index set", hermes::to_string(data.index_set_));
    m.addArray("stencils", data.stencils_);
    return m;
  }
};

template <> struct DebugTraits<naiades::numeric::Boundary> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::numeric::Boundary &data) {
    auto m = DebugMessage();
    m.addTitle("Boundary");
    m.add("boundary element type", data.boundary_element_type_);
    m.add("interior element type", data.interior_element_type_);
    m.addArray("regions", data.regions_);
    return m;
  }
};

} // namespace hermes

#endif
