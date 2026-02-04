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
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Boundary conditions.

#pragma once

#include <naiades/core/field.h>
#include <naiades/numeric/boundary_conditions.h>
#include <naiades/utils/utils.h>

namespace naiades::numeric {

/// The BoundaryField object holds the boundary topology and numerical
/// information of a simulation setting for a specific field.
///  - The boundary can be split into boundary regions, disjoint sub-sets of
///    boundary element indices.
///  - Different boundary conditions can be applied on the boundary regions.
/// The boundary field stores the implicit and explicit values of the boundary
/// conditions for a particular field.
/// - Implicit Values: Values that build the left hand side of the system, such
///                    as coefficients in the system matrix.
/// - Explicit Values: Known values that appear on the right hand side of the
///                    system.
class Boundary {
public:
  class Region {
  public:
    Region(core::Element element_type, const std::vector<h_size> &indices);

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

    NAIADES_to_string_FRIEND(Region);
  };

  /// Defines a boundary region from the given boundary element index set.
  /// \param loc Boundary element type
  /// \param indices Boundary element indices.
  /// \param [out] The index of the newly created region.
  Boundary &addRegion(core::Element loc, const std::vector<h_size> &indices,
                      h_size *region_index = nullptr);
  /// Set the boundary condition for this field.
  Boundary &setCondition(h_size region_index,
                         bc::BoundaryCondition::Ptr condition,
                         core::Element interior_field_loc);
  /// Set the same boundary condition in all regions.
  Boundary &setCondition(bc::BoundaryCondition::Ptr condition,
                         core::Element interior_field_loc);

  /// Correct fields boundary elements by explicitly updating their values.
  /// \note This only updates values at elements located at the boundary, such
  ///       as faces.
  NaResult compute(core::FieldCRef<f32> interior_field,
                   core::FieldRef<f32> boundary_field) const;

  NaResult resolve(core::Topology::Ptr topology);

  const DiscreteOperator &stencil(const core::Index &index) const;

private:
  std::vector<Region> regions_;

  NAIADES_to_string_FRIEND(Boundary);
};

} // namespace naiades::numeric
