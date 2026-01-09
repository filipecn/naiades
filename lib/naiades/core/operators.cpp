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

#include "hermes/core/debug.h"
#include <naiades/core/operators.h>

#include <naiades/core/boundary.h>

namespace naiades {

HERMES_TO_STRING_METHOD_BEGIN(core::DiscreteOperator)
HERMES_TO_STRING_METHOD_MAP_FIELD_BEGIN(nodes_, index, weight)
HERMES_TO_STRING_METHOD_LINE("({}: {})\n", index, weight)
HERMES_TO_STRING_METHOD_MAP_FIELD_END
HERMES_TO_STRING_METHOD_FIELD(constant_);
HERMES_TO_STRING_METHOD_END

} // namespace naiades

namespace naiades::core {

DiscreteOperator
DiscreteOperator::laplacian(DiscretizationGeometry2::Ptr discretization,
                            const Boundary &boundary, Element loc, h_size index,
                            Element boundary_loc) {
  DiscreteOperator op;
  // build stencil
  auto neighbours = discretization->star(loc, index, boundary_loc);
  op.add(index, -(int)neighbours.size());
  // resolve stencil
  for (const auto &n : neighbours)
    if (n.is_boundary)
      op += boundary.resolve(n.index, index) * 1.0;
    else
      op.add(n.index, 1.0);
  return op;
}

DiscreteOperator DiscreteOperator::divergence(const geo::Grid2 &grid,
                                              h_size index) {
  DiscreteOperator op;
  const auto d = grid.cellSize();
  const auto ij = grid.index(Element::Type::CELL_CENTER, index);
  op.add(grid.flatIndex(Element::Type::X_FACE_CENTER, ij.up()), -0.5 * d.y);
  op.add(grid.flatIndex(Element::Type::X_FACE_CENTER, ij), 0.5 * d.y);
  op.add(grid.flatIndex(Element::Type::Y_FACE_CENTER, ij.right()), -0.5 * d.x);
  op.add(grid.flatIndex(Element::Type::Y_FACE_CENTER, ij), 0.5 * d.x);
  return op;
}

/// Compute divergence field.
void divergence(const geo::Grid2 &grid, const FieldCRef<f32> &u,
                const FieldCRef<f32> &v, FieldRef<f32> &f) {
  HERMES_ASSERT(u.element() == Element::Type::Y_FACE_CENTER);
  HERMES_ASSERT(v.element() == Element::Type::X_FACE_CENTER);
  HERMES_ASSERT(f.element() == Element::Type::CELL_CENTER);

#define AT(F, IJ)                                                              \
  F[grid.flatIndex(F.element(), IJ) - grid.flatIndexOffset(F.element())]

  const auto d = grid.cellSize();
  for (auto ij : hermes::range2(grid.resolution(f.element()))) {
    AT(f, ij) = -0.5 * (d.y * (AT(v, ij.up()) - AT(v, ij)) +
                        d.x * (AT(u, ij.right()) - AT(u, ij)));
  }
#undef AT
}

DiscreteOperator::DiscreteOperator(const std::vector<h_size> &indices,
                                   const std::vector<real_t> &weights) {
  HERMES_ASSERT(indices.size() == weights.size());
  for (h_size i = 0; i < indices.size(); ++i)
    nodes_[indices[i]] = weights[i];
}

void DiscreteOperator::add(h_size index, real_t weight) {
  nodes_[index] += weight;
}

void DiscreteOperator::setConstant(real_t s) { constant_ = s; }

real_t DiscreteOperator::constant() const { return constant_; }

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

h_size DiscreteOperator::size() const { return nodes_.size(); }

} // namespace naiades::core
