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

/// \file   operators.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07

#include <naiades/numeric/discrete_operator.h>

#include <naiades/numeric/boundary.h>

namespace naiades {

HERMES_TO_STRING_METHOD_BEGIN(numeric::DiscreteOperator)
HERMES_TO_STRING_METHOD_MAP_FIELD_BEGIN(nodes_, index, weight)
HERMES_TO_STRING_METHOD_LINE("({}: {})\n", index, weight)
HERMES_TO_STRING_METHOD_MAP_FIELD_END
HERMES_TO_STRING_METHOD_FIELD(constant_);
HERMES_TO_STRING_METHOD_END

} // namespace naiades

namespace naiades::numeric {

DiscreteOperator::DiscreteOperator(h_size center_index)
    : center_index_{center_index} {}

DiscreteOperator::DiscreteOperator(h_size center_index,
                                   const std::vector<h_size> &indices,
                                   const std::vector<real_t> &weights)
    : center_index_{center_index} {
  HERMES_ASSERT(indices.size() == weights.size());
  for (h_size i = 0; i < indices.size(); ++i)
    nodes_[indices[i]] = weights[i];
}

void DiscreteOperator::setCenterIndex(h_size index) { center_index_ = index; }

void DiscreteOperator::add(h_size index, real_t weight) {
  nodes_[index] += weight;
}

void DiscreteOperator::addUnresolved(const core::ElementIndex &element,
                                     real_t weight) {
  boundary_nodes_[*element.index] += weight;
}

NaResult DiscreteOperator::resolve(const Boundary &boundary) {
  for (const auto &item : boundary_nodes_) {
    *this += boundary.stencil(core::Index::global(item.first)) * item.second;
  }
  return NaResult::noError();
}

void DiscreteOperator::setConstant(real_t s) { constant_ = s; }

real_t DiscreteOperator::constant() const { return constant_; }

h_size DiscreteOperator::centerIndex() const { return center_index_; }

real_t DiscreteOperator::operator[](h_size index) const {
  auto it = nodes_.find(index);
  if (it == nodes_.end())
    return 0;
  return it->second;
}

real_t &DiscreteOperator::operator[](h_size index) {
  static real_t s_dummy = 0;
  auto it = nodes_.find(index);
  if (it == nodes_.end())
    return s_dummy;
  return it->second;
}

DiscreteOperator &DiscreteOperator::operator+=(const DiscreteOperator &rhs) {
  for (const auto &node : rhs.nodes_)
    nodes_[node.first] += node.second;
  constant_ += rhs.constant_;
  return *this;
}

DiscreteOperator DiscreteOperator::operator*(real_t s) const {
  DiscreteOperator op;
  op.constant_ = constant_ * s;
  for (auto &node : nodes_)
    op.nodes_[node.first] = node.second * s;
  return op;
}

DiscreteOperator &DiscreteOperator::operator*=(real_t s) {
  constant_ *= s;
  for (auto &node : nodes_)
    node.second *= s;
  return *this;
}

h_size DiscreteOperator::size() const { return nodes_.size(); }

} // namespace naiades::numeric
