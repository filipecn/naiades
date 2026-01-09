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

/// \file   operators.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Field differential operators.

#pragma once

#include <naiades/geo/grid.h>

namespace naiades::core {

class Boundary;

/// A discrete operator represents the discretization of a linear operator that
/// can be implicit or explicit. Implicit forms must be solved within linear
/// systems, while explicit forms can be computed from fields.
class DiscreteOperator {
public:
  /// Compute the discrete Laplacian operator centered at the given element.
  /// \param discretization
  /// \param boundary
  /// \param loc
  /// \param index
  static DiscreteOperator laplacian(DiscretizationGeometry2::Ptr discretization,
                                    const Boundary &boundary, Element loc,
                                    h_size index, Element boundary_loc);
  /// Compute the discrete Divergence operator centered at the given element.
  /// \param grid
  /// \param boundary
  /// \param loc
  /// \param index
  static DiscreteOperator divergence(const geo::Grid2 &grid, h_size index);

  DiscreteOperator() = default;
  DiscreteOperator(const std::vector<h_size> &indices,
                   const std::vector<real_t> &weights);
  /// Add element to this operator.
  /// \param index
  /// \param weight
  void add(h_size index, real_t weight);
  void setConstant(real_t s);
  /// Computes this operator for the given field.
  template <typename FieldType>
  real_t operator()(const FieldType &field) const {
    real_t s = constant_;
    for (const auto &node : nodes_)
      s += field[node.first] * node.second;
    return s;
  }
  real_t constant() const;

  real_t operator[](h_size index) const;
  real_t &operator[](h_size index);
  DiscreteOperator &operator+=(const DiscreteOperator &rhs);
  DiscreteOperator operator*(real_t s) const;
  /// \return the diagonal size (element count).
  h_size size() const;

private:
  std::unordered_map<h_size, real_t> nodes_;
  real_t constant_{0};

  NAIADES_to_string_FRIEND(DiscreteOperator);
};

void divergence(const geo::Grid2 &grid, const FieldCRef<f32> &u,
                const FieldCRef<f32> &v, FieldRef<f32> &f);

} // namespace naiades::core
