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

namespace naiades::numeric::solvers {

void CG::buildSystem() {
  h_size n = implicit_.size();

  // Declare a sparse matrix type with double precision
  A_ = Eigen::SparseMatrix<double>(n, n);

  // Reserve space for 1 non-zero element per row
  A_.reserve(Eigen::VectorXi::Constant(n, 1));

  // Build matrix rows
  for (auto dop : implicit_) {
    for (auto item : dop.nodes())
      A_.insert(dop.centerIndex(), item.first) = item.second;
  }
  A_.makeCompressed();
}

void CG::solveFor(core::FieldRef<real_t> &unknown_field,
                  const Scalar &rhs) const {
  h_size n = implicit_.size();

  // Declare vectors for the solution (x) and the right-hand side (b)
  Eigen::VectorXd x(n), b(n);

  for (h_index i = 0; i < n; ++i) {
    b(i) = rhs[i];
  }

  // --- Set up and use the Conjugate Gradient solver ---
  // Using Lower|Upper for better performance
  Eigen::ConjugateGradient<Eigen::SparseMatrix<double>,
                           Eigen::Lower | Eigen::Upper>
      cg;

  // 1. Compute the factorization/preconditioner
  cg.compute(A_);

  // 2. Solve the linear system
  x = cg.solve(b);

  // --- Output results and convergence information ---
  std::cout << "Solver info:" << std::endl;
  std::cout << "#iterations: " << cg.iterations() << std::endl;
  std::cout << "Estimated error: " << cg.error() << std::endl;

  // You can also control the max iterations and tolerance if needed
  cg.setMaxIterations(100);
  cg.setTolerance(1e-6);

  //
  for (h_index i = 0; i < n; ++i)
    unknown_field[i] = x[i];
}

} // namespace naiades::numeric::solvers