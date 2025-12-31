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

#include <variant>

namespace naiades::core {

namespace bc {

template <typename T> class Condition {
public:
  using Ptr = hermes::Ref<Condition>;

  T implicitCoeff() const;
  T explicitCoeff() const;
};

/// Dirichlet
template <typename T> class Dirichlet : public Condition<T> {
public:
  using Ptr = hermes::Ref<Dirichlet>;
  Dirichlet() = default;
  Dirichlet(const T &fixed_value) : value(fixed_value) {}
  std::variant<T, const std::function<void()>> value;
};

/// Neumann
template <typename T> class Neumann : public Condition<T> {
public:
};

} // namespace bc

/// Represents a contiguous interval of boundary indices [start,end)
struct IndexInterval {
  h_size start;
  h_size end;
};

using IndexSet = std::variant<std::monostate, std::vector<IndexInterval>,
                              std::vector<h_size>>;
template <class... Ts> struct IndexSetOverloaded : Ts... {
  using Ts::operator()...;
};

template <class... Ts> IndexSetOverloaded(Ts...) -> IndexSetOverloaded<Ts...>;

/// The boundary field stores the implicit and explicit values of the boundary
/// conditions for a particular field.
/// - Implicit Values: Values that build the left hand side of the system,
///                    such as coefficients in the system matrix.
/// - Explicit Values: Known values that appear on the right hand side of the
///                    system.
///
/// The indices of the elements in the field are the indices of these same
/// elements in the spatial discretization structure. Usually, boundary elements
/// are faces.
template <typename T> class BoundaryField {
public:
  ///
  const T &implicitValue(h_size element_index) {}
  ///
  const T &explicitValue(h_size element_index) {}

  bc::Condition<f32>::Ptr condition;
};

/// The boundary object holds the boundary topology and numerical information
/// of a simulation setting.
///  - The boundary can be split into boundary regions, disjoint sub-sets of
///    boundary element indices.
///  - Different boundary conditions can be applied on the boundary regions for
///    each field.
class Boundary {
public:
  class Region {
  public:
    void setCondition(const std::string &field_name,
                      bc::Condition<f32>::Ptr condition);

  private:
    friend class Boundary;

    std::unordered_map<std::string, BoundaryField<f32>> scalar_fields_;
    IndexSet index_set_;

    NAIADES_to_string_FRIEND(Region);
  };

  /// Defines a boundary region from the given boundary element index set.
  /// \param indices
  /// \return The index of the newly created region.
  h_size setRegion(const std::vector<h_size> &indices);

  /// Set a boundary field for a given region index.
  /// \param region_index
  /// \param field_name
  /// \param condition
  void set(h_size region_index, const std::string &field_name,
           bc::Condition<f32>::Ptr condition);
  /// Set a boundary field for all regions.
  /// \param field_name
  /// \param condition
  void set(const std::string &field_name, bc::Condition<f32>::Ptr condition);

private:
  std::vector<Region> regions_;

  NAIADES_to_string_FRIEND(Boundary);
};

} // namespace naiades::core
