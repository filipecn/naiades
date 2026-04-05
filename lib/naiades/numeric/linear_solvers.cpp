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

/// \file   linear_solvers.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-03-24

#include <naiades/numeric/linear_solvers.h>

#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>

namespace naiades::numeric::solvers {

void CG::solve_system(const DiscreteExpression &lhs,
                      const core::FieldRef<real_t> *rhs) const {
  h_size n = rhs->size();

  // Declare a sparse matrix type with double precision
  Eigen::SparseMatrix<double> A(n, n);
  // Declare vectors for the solution (x) and the right-hand side (b)
  Eigen::VectorXd x(n), b(n);

  // Reserve space for 1 non-zero element per row
  A.reserve(Eigen::VectorXi::Constant(n, 1));

  for (auto dop : lhs) {
    for (auto item : dop.nodes())
      A.insert(dop.centerIndex(), item.first) = item.second;
  }
  A.makeCompressed();

  for (h_index i = 0; i < n; ++i) {
    b(i) = (*rhs)[i] - lhs[i].constant();
  }

  // --- Set up and use the Conjugate Gradient solver ---
  // Using Lower|Upper for better performance
  Eigen::ConjugateGradient<Eigen::SparseMatrix<double>,
                           Eigen::Lower | Eigen::Upper>
      cg;

  // 1. Compute the factorization/preconditioner
  cg.compute(A);

  // 2. Solve the linear system
  x = cg.solve(b);

  // --- Output results and convergence information ---
  std::cout << "Solver info:" << std::endl;
  std::cout << "#iterations: " << cg.iterations() << std::endl;
  std::cout << "Estimated error: " << cg.error() << std::endl;
  std::cout << "Solution x (first 5 elements):" << std::endl;
  std::cout << x.head(4) << std::endl;

  // You can also control the max iterations and tolerance if needed
  cg.setMaxIterations(100);
  cg.setTolerance(1e-6);

  //
  for (h_index i = 0; i < n; ++i)
    (*unknown_field_)[i] = x[i];
};

} // namespace naiades::numeric::solvers