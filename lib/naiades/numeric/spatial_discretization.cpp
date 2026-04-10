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

/// \file   spatial_discretization.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-17-01

#include <naiades/numeric/spatial_discretization.h>

namespace naiades::numeric {

NaResult SpatialDiscretization::resolveBoundaries() {
  for (auto &item : boundaries_)
    NAIADES_RETURN_BAD_RESULT(resolveBoundary(item.first));
  return NaResult::noError();
}

NaResult SpatialDiscretization::resolveBoundary(const core::Symbol &symbol) {
  auto it = boundaries_.find(symbol);
  if (it == boundaries_.end())
    return NaResult::notFound();
  NAIADES_RETURN_BAD_RESULT(it->second.resolve(topology_));
  return NaResult::noError();
}

void SpatialDiscretization::addBoundary(const core::Symbol &symbol,
                                        const std::vector<h_size> &indices,
                                        h_size *region_index) {
  boundaries_[symbol].addRegion(indices, region_index);
}

void SpatialDiscretization::setBoundaryCondition(
    const core::Symbol &symbol, h_size region_index,
    bc::BoundaryCondition::Ptr condition) {
  boundaries_[symbol].setCondition(region_index, condition);
}

void SpatialDiscretization::setBoundaryCondition(
    const core::Symbol &symbol, bc::BoundaryCondition::Ptr condition) {
  boundaries_[symbol].setCondition(condition);
}

const Boundary &
SpatialDiscretization::boundary(const core::Symbol &symbol) const {
  static Boundary s_dummy({});
  auto it = boundaries_.find(symbol);
  if (it == boundaries_.end())
    return s_dummy;
  return it->second;
}

Boundary &SpatialDiscretization::boundary(const core::Symbol &symbol) {
  static Boundary s_dummy({});
  auto it = boundaries_.find(symbol);
  if (it == boundaries_.end())
    return s_dummy;
  return it->second;
}

const std::unordered_map<core::Symbol, Boundary> &
SpatialDiscretization::boundaries() const {
  return boundaries_;
}

DiscreteExpression
SpatialDiscretization::dx(const core::DiscreteSymbol &ds) const {
  DiscreteExpression de(ds);
  auto n = topology_->elementCount(ds.symbol.loc);
  for (h_index i = 0; i < n; ++i) {
    de.addIndexEntry(i, derivative(derivative_bits::x, i, ds));
  }
  return de;
}

DiscreteExpression
SpatialDiscretization::dy(const core::DiscreteSymbol &ds) const {
  DiscreteExpression de(ds);
  auto n = topology_->elementCount(ds.symbol.loc);
  for (h_index i = 0; i < n; ++i) {
    de.addIndexEntry(i, derivative(derivative_bits::y, i, ds));
  }
  return de;
}

DiscreteExpression
SpatialDiscretization::L(const core::DiscreteSymbol &ds) const {
  DiscreteExpression de(ds);
  auto n = topology_->elementCount(ds.symbol.loc);
  for (h_index i = 0; i < n; ++i) {
    de.addIndexEntry(i, laplacian(i, ds));
  }
  return de;
}

} // namespace naiades::numeric
