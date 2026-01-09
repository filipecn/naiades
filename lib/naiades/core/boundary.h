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

#include <naiades/core/boundary_conditions.h>
#include <naiades/core/discretization.h>
#include <naiades/core/field.h>
#include <naiades/core/operators.h>
#include <naiades/utils/utils.h>

namespace naiades::core {

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
    Region() = default;
    Region(const std::vector<h_size> &indices);

    void setIndices(const std::vector<h_size> &indices);

    /// Set the boundary condition for this field.
    void setCondition(bc::BoundaryCondition::Ptr condition);
    ///
    bool contains(const Index &index) const;
    /// Resolves the boundary condition in the element.
    DiscreteOperator resolve(h_size boundary_index,
                             h_size interior_index) const;
    ///
    void resolve(core::DiscretizationTopology::Ptr discretization);
    ///
    void compute(FieldCRef<f32> interior_field, FieldRef<f32> field);

  private:
    friend class Boundary;

    utils::IndexSet index_set_;
    bc::BoundaryCondition::Ptr condition_;
    // TODO make it variant:
    // std::variant<DiscreteOperator, std::vector<DiscreteOperator>> stencils_;
    std::vector<DiscreteOperator> stencils_;

    NAIADES_to_string_FRIEND(Region);
  };

  /// Defines a boundary region from the given boundary element index set.
  /// \param indices
  /// \return The index of the newly created region.
  h_size addRegion(const std::vector<h_size> &indices);
  /// Set the boundary condition for this field.
  void setCondition(h_size region_index, bc::BoundaryCondition::Ptr condition);
  /// Set the same boundary condition in all regions.
  void setCondition(bc::BoundaryCondition::Ptr condition);

  /// Correct fields boundary elements by explicitly updating their values.
  /// \note This only updates values at elements located at the boundary, such
  ///       as faces.
  f32 compute(h_size boundary_index, h_size interior_index,
              FieldRef<f32> field) const;

  /// Resolves the boundary condition in the element.
  DiscreteOperator resolve(h_size boundary_index, h_size interior_index) const;

private:
  std::vector<Region> regions_;

  NAIADES_to_string_FRIEND(Boundary);
};

class BoundarySet {
public:
  /// Defines a boundary region from the given boundary element index set.
  /// \param indices
  /// \return The index of the newly created region.
  h_size addRegion(const std::string &field_name,
                   const std::vector<h_size> &indices);
  /// Set a boundary condition of a field for a given region index.
  /// \param field_name
  /// \param region_index
  /// \param condition
  void set(const std::string &field_name, h_size region_index,
           bc::BoundaryCondition::Ptr condition);
  /// Set the same boundary condition of a field in all regions.
  /// \param field_name
  /// \param condition
  void set(const std::string &field_name, bc::BoundaryCondition::Ptr condition);
  ///
  const Boundary &operator[](const std::string &field_name) const;

private:
  std::unordered_map<std::string, Boundary> boundaries_;

  NAIADES_to_string_FRIEND(BoundarySet);
};

} // namespace naiades::core
