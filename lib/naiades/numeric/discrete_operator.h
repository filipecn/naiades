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

#include <naiades/core/element.h>

namespace naiades::numeric {

class Boundary;

/// A discrete operator represents the discretization of a linear operator that
/// can be implicit or explicit. Implicit forms must be solved within linear
/// systems, while explicit forms can be computed from fields.
class DiscreteOperator {
public:
  DiscreteOperator() = default;
  DiscreteOperator(h_size center_index);
  DiscreteOperator(h_size center_index, const std::vector<h_size> &indices,
                   const std::vector<real_t> &weights);
  /// Set central element index
  void setCenterIndex(h_size index);
  /// Add element to this operator.
  /// \param index
  /// \param weight
  void add(h_size index, real_t weight);
  /// Add unresolved element to this operator.
  void addUnresolved(const core::ElementIndex &element, real_t weight);
  /// Resolve boundary elements by expanding their terms in the operator.
  /// \note Once resolved, the original unresolved terms are lost.
  NaResult resolve(const Boundary &boundary);
  /// Checks if there are any unresolved terms in this operator.
  bool isUnresolved() const;
  /// Set constant term.
  void setConstant(real_t s);
  /// Computes this operator for the given field.
  template <typename FieldType>
  real_t operator()(const FieldType &field) const {
    real_t s = constant_;
    for (const auto &node : nodes_)
      s += field[core::Index::global(node.first)] * node.second;
    return s;
  }
  real_t constant() const;
  h_size centerIndex() const;

  real_t operator[](h_size index) const;
  real_t &operator[](h_size index);
  DiscreteOperator &operator+=(const DiscreteOperator &rhs);
  DiscreteOperator operator*(real_t s) const;
  DiscreteOperator &operator*=(real_t s);
  /// \return the diagonal size (element count).
  h_size size() const;

private:
  std::unordered_map<h_size, real_t> nodes_;
  std::unordered_map<h_size, real_t> boundary_nodes_;
  real_t constant_{0};
  h_size center_index_{0};

  NAIADES_to_string_FRIEND(DiscreteOperator);
};

} // namespace naiades::numeric
