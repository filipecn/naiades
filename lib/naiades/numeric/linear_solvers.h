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

#include <naiades/core/field.h>
#include <naiades/numeric/discrete_expression.h>

#include <Eigen/Sparse>

namespace naiades::numeric::solvers {

/// Interface for linear solvers
/// \tparam Derived
template <typename Derived> class LinearSystemSolver {
public:
  Derived &setUnknown(const core::DiscreteSymbol &unknown);

  Derived &build(const DiscreteExpression &lhs, const DiscreteExpression &rhs);

  void
  solve(core::FieldRef<real_t> &unknown_field,
        const std::vector<core::FieldCRef<real_t>> &explicit_fields = {}) const;

protected:
  virtual void buildSystem() = 0;
  virtual void solveFor(core::FieldRef<real_t> &unknown_field,
                        const Scalar &rhs) const = 0;

  core::DiscreteSymbol unknown_;
  DiscreteExpression implicit_;
  DiscreteExpression explicit_;
};

template <typename Derived>
Derived &
LinearSystemSolver<Derived>::setUnknown(const core::DiscreteSymbol &sym) {
  unknown_ = sym;
  return *reinterpret_cast<Derived *>(this);
}

template <typename Derived>
Derived &LinearSystemSolver<Derived>::build(const DiscreteExpression &lhs,
                                            const DiscreteExpression &rhs) {
  // separate implicit and explicit parts
  implicit_ = lhs;
  explicit_ = rhs;
  buildSystem();
  return *reinterpret_cast<Derived *>(this);
}

template <typename Derived>
void LinearSystemSolver<Derived>::solve(
    core::FieldRef<real_t> &unknown_field,
    const std::vector<core::FieldCRef<real_t>> &explicit_fields) const {
  // compute explicit side
  Scalar rhs(unknown_field.size());
  if (!explicit_fields.empty()) {
    HERMES_ASSERT(unknown_field.size() == explicit_fields[0].size());
    for (h_index i = 0; i < rhs.size(); ++i) {
      rhs[i] = explicit_fields[0][i] - implicit_[i].constant();
    }
  } else {
    HERMES_ASSERT(explicit_.isConstant());
    for (h_index i = 0; i < rhs.size(); ++i) {
      rhs[i] = -implicit_[i].constant();
    }
  }
  solveFor(unknown_field, rhs);
}

class CG : public LinearSystemSolver<CG> {
public:
private:
  void buildSystem() override;
  void solveFor(core::FieldRef<real_t> &unknown_field,
                const Scalar &rhs) const override;
  Eigen::SparseMatrix<double> A_;
};

} // namespace naiades::numeric::solvers