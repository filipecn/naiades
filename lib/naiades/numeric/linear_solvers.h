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

/// \file   linear_solvers.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-03-24

#pragma once

#include <naiades/numeric/discrete_expression.h>

#include <naiades/core/field.h>

namespace naiades::numeric::solvers {

/// Interface for linear solvers
/// \tparam Derived
template <typename Derived> class LinearSystemSolver {
public:
  Derived &setUnknown(const core::DiscreteSymbol &unknown,
                      core::FieldRef<real_t> *unknown_field);
  Derived &addVariable(const core::FieldRef<real_t> *var);

  void solve(const DiscreteExpression &lhs,
             const core::DiscreteSymbol &rhs) const;

protected:
  virtual void solve_system(const DiscreteExpression &lhs,
                            const core::FieldRef<real_t> *rhs) const = 0;

  core::DiscreteSymbol unknonw_;
  core::FieldRef<real_t> *unknown_field_;
  std::unordered_map<core::Element, const core::FieldRef<real_t> *> variables_;
};

template <typename Derived>
Derived &
LinearSystemSolver<Derived>::setUnknown(const core::DiscreteSymbol &sym,
                                        core::FieldRef<real_t> *unknown_field) {
  unknonw_ = sym;
  unknown_field_ = unknown_field;
  return *reinterpret_cast<Derived *>(this);
}

template <typename Derived>
Derived &
LinearSystemSolver<Derived>::addVariable(const core::FieldRef<real_t> *var) {
  variables_[var->element()] = var;
  return *reinterpret_cast<Derived *>(this);
}

template <typename Derived>
void LinearSystemSolver<Derived>::solve(const DiscreteExpression &lhs,
                                        const core::DiscreteSymbol &rhs) const {
  // resolve lhs
  // resolve rhs
  // solve system
  auto it = variables_.find(rhs.symbol.loc);
  solve_system(lhs, it->second);
}

class CG : public LinearSystemSolver<CG> {
public:
  void solve_system(const DiscreteExpression &lhs,
                    const core::FieldRef<real_t> *rhs) const override;
};

} // namespace naiades::numeric::solvers