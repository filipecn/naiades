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
    : sym_{sym}, default_coefficient_{1.0}, default_constant_{0.0} {}

DiscreteExpression::DiscreteExpression(real_t value) noexcept
    : default_coefficient_{1.0}, default_constant_{value} {}

void DiscreteExpression::addIndexEntry(h_index index, DiscreteOperator &&e) {
  HERMES_ASSERT(sym_.has_value());
  entries_[index] = e;
}

const core::Symbol &DiscreteExpression::symbol() const {
  if (sym_.has_value())
    return (*sym_).symbol;
  HERMES_ERROR("Trying to access symbol from constant discrete expression.");
  static core::Symbol s_sym;
  return s_sym;
}

bool DiscreteExpression::isConstant() const { return !sym_.has_value(); }

const DiscreteOperator &DiscreteExpression::operator[](h_index index) const {
  static DiscreteOperator s_dop;
  if (!sym_.has_value()) {
    s_dop = DiscreteOperator();
    s_dop.setConstant(default_constant_);
    return s_dop;
  }
  auto it = entries_.find(index);
  if (it != entries_.end())
    return it->second;
  // fallback to mono-stencil
  s_dop = DiscreteOperator(index);
  s_dop.add(index, default_coefficient_);
  s_dop.setConstant(default_constant_);
  return s_dop;
}

h_size DiscreteExpression::size() const { return entries_.size(); }

DiscreteExpression::iterator DiscreteExpression::begin() const {
  return DiscreteExpression::iterator(*this, entries_.begin());
}

DiscreteExpression::iterator DiscreteExpression::end() const {
  return DiscreteExpression::iterator(*this, entries_.end());
}

DiscreteExpression DiscreteExpression::operator-() const {
  DiscreteExpression r;
  r.sym_ = sym_;
  r.default_coefficient_ = -default_coefficient_;
  r.default_constant_ = -default_constant_;
  for (const auto &item : entries_)
    r.entries_[item.first] = -item.second;
  return r;
}

DiscreteExpression
DiscreteExpression::operator+(const DiscreteExpression &rhs) const {
  DiscreteExpression r;
  // sum default_constant values
  r.default_constant_ = default_constant_ + rhs.default_constant_;
  r.default_coefficient_ = default_coefficient_ + rhs.default_coefficient_;
  // here we need to handle symbol cases
  if (sym_.has_value() && rhs.sym_.has_value()) {
    HERMES_ASSERT(*sym_ == *rhs.sym_);
    r.sym_ = sym_;

    // for all in lhs add the correspondent in rhs
    // note that unexistent indices in rhs should return the mono-stencil
    for (const auto &item : entries_) {
      r.entries_[item.first] += rhs[item.first];
    }

    // for all in rhs (not caught by the loop above) add to new indices in lhs
    for (const auto &item : rhs.entries_) {
      if (!entries_.count(item.first)) {
        r.entries_[item.first] = (*this)[item.first] + item.second;
      }
    }

  } else if (sym_.has_value() && !rhs.sym_.has_value()) {
    r.sym_ = sym_;
    for (const auto &item : entries_) {
      r.entries_[item.first] += rhs[item.first];
    }
  } else if (!sym_.has_value() && rhs.sym_.has_value()) {
    r.sym_ = rhs.sym_;
    for (const auto &item : rhs.entries_) {
      r.entries_[item.first] = (*this)[item.first] + item.second;
    }
  }

  return r;
}

} // namespace naiades::numeric