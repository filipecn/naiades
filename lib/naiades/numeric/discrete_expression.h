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

/// \file   operators.h
/// \author FilipeCN (filipedecn\gmail.com)
/// \date   2026-06-07
/// \brief  Discrete differencial equation term.

#pragma once

#include <naiades/core/symbol.h>
#include <naiades/numeric/discrete_operator.h>

namespace naiades::numeric {

/// \brief  Represents a discrete operator for a element type in a
///         discretization.
///
/// The discrete equation term holds a single discrete operator for each
/// element instance in a discretization. Therefore, the discrete equation
/// term is defined for a given element type.
/// \note Multiple discrete equation terms compose a discrete equation.
class DiscreteExpression {
public:
  ///
  class iterator {
  public:
    const DiscreteOperator &operator*() const;
    iterator &operator++();

    bool operator==(const iterator &rhs) const;

  private:
    friend class DiscreteExpression;

    iterator(const DiscreteExpression &de_,
             std::unordered_map<h_index, DiscreteOperator>::const_iterator it);

    const DiscreteExpression &de_;
    std::unordered_map<h_index, DiscreteOperator>::const_iterator it_;
  };

  DiscreteExpression() = default;
  DiscreteExpression(const core::DiscreteSymbol &sym) noexcept;
  virtual ~DiscreteExpression() = default;

  /// Register an indexed entry to this expression.
  /// \param index
  /// \param e
  void addIndexEntry(h_index index, DiscreteOperator &&e);

  /// \brief
  /// \return
  const core::Symbol &symbol() const;

  ///
  const DiscreteOperator &operator[](h_index index) const;

  iterator begin() const;
  iterator end() const;

  // arithmetic operators

  /// \brief
  /// \return
  DiscreteExpression operator-() const;
  ///
  /// \param rhs
  DiscreteExpression operator+(const DiscreteExpression &rhs) const;

private:
  core::DiscreteSymbol sym_;
  std::unordered_map<h_index, DiscreteOperator> entries_;

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS
  friend struct hermes::DebugTraits<DiscreteExpression>;
#endif
};

} // namespace naiades::numeric

namespace hermes {

template <> struct DebugTraits<naiades::numeric::DiscreteExpression> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage
  message(const naiades::numeric::DiscreteExpression &data) {
    auto m = DebugMessage();
    m.addTitle("Discrete Expression");
    m.add("symbol", data.sym_);
    m.addMap("entries", data.entries_);
    return m;
  }
};

} // namespace hermes