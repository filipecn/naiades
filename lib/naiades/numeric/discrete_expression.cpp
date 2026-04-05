/* Copyright (c) 2026, FilipeCN.
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

/// \file   discrete_expression.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-03-20

#include <naiades/numeric/discrete_expression.h>

namespace naiades::numeric {

DiscreteExpression::iterator::iterator(
    const DiscreteExpression &de,
    std::unordered_map<h_index, DiscreteOperator>::const_iterator it)
    : de_{de}, it_{it} {}

const DiscreteOperator &DiscreteExpression::iterator::operator*() const {
  return it_->second;
}

DiscreteExpression::iterator &DiscreteExpression::iterator::operator++() {
  if (it_ != de_.entries_.end())
    it_++;
  return *this;
}

bool DiscreteExpression::iterator::operator==(
    const DiscreteExpression::iterator &rhs) const {
  return it_ == rhs.it_;
}

DiscreteExpression::DiscreteExpression(const core::DiscreteSymbol &sym) noexcept
    : sym_{sym} {}

void DiscreteExpression::addIndexEntry(h_index index, DiscreteOperator &&e) {
  entries_[index] = e;
}

const core::Symbol &DiscreteExpression::symbol() const { return sym_.symbol; }

const DiscreteOperator &DiscreteExpression::operator[](h_index index) const {
  auto it = entries_.find(index);
  if (it != entries_.end())
    return it->second;
  HERMES_ERROR("Entry with index {} not found!", index);
  static DiscreteOperator s_dop;
  return s_dop;
}

DiscreteExpression::iterator DiscreteExpression::begin() const {
  return DiscreteExpression::iterator(*this, entries_.begin());
}

DiscreteExpression::iterator DiscreteExpression::end() const {
  return DiscreteExpression::iterator(*this, entries_.end());
}

DiscreteExpression DiscreteExpression::operator-() const {
  DiscreteExpression r;
  for (const auto &item : entries_)
    r.entries_[item.first] = -item.second;
  return r;
}

DiscreteExpression
DiscreteExpression::operator+(const DiscreteExpression &rhs) const {
  DiscreteExpression r;
  for (const auto &item : entries_) {
    auto it = rhs.entries_.find(item.first);
    if (it != rhs.entries_.end())
      r.entries_[item.first] = item.second + it->second;
    else {
      // err
    }
  }
  return r;
}

} // namespace naiades::numeric