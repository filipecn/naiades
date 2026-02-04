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

/// \file   boundary_conditions.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Boundary conditions.

#pragma once

#include <naiades/core/field.h>
#include <naiades/numeric/discrete_operator.h>

#include <variant>

namespace naiades::numeric::bc {

class BoundaryCondition {
public:
  using Ptr = hermes::Ref<BoundaryCondition>;

  virtual DiscreteOperator
  resolve(const core::ElementIndex &boundary_element,
          const core::ElementIndex &interior_element) const = 0;
};

/// Dirichlet
class Dirichlet : public BoundaryCondition {
public:
  using Ptr = hermes::Ref<Dirichlet>;

  Dirichlet(const real_t &fixed_value) : value_(fixed_value) {}

  DiscreteOperator
  resolve(const core::ElementIndex &boundary_element,
          const core::ElementIndex &interior_element) const override {
    HERMES_UNUSED_VARIABLE(boundary_element);
    HERMES_UNUSED_VARIABLE(interior_element);
    DiscreteOperator op;
    std::visit(
        [&](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, real_t>)
            op.setConstant(arg);
          else if constexpr (std::is_same_v<T, std::function<void()>>)
            HERMES_NOT_IMPLEMENTED;
        },
        value_);
    return op;
  }

private:
  std::variant<real_t, std::function<void()>> value_;
};

/// Neumann
class Neumann : public BoundaryCondition {
public:
  using Ptr = hermes::Ref<Neumann>;

  DiscreteOperator
  resolve(const core::ElementIndex &boundary_element,
          const core::ElementIndex &interior_element) const override {
    HERMES_UNUSED_VARIABLE(boundary_element);
    DiscreteOperator op;
    op.add(*interior_element.index, 1.0);
    return op;
  }
};

} // namespace naiades::numeric::bc
