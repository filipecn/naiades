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

/// \file   rbf.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-06-10

#pragma once

#include <naiades/base/result.h>
#include <naiades/core/topology.h>
#include <naiades/numeric/spatial_discretization.h>

#include <Eigen/Dense>

namespace naiades::numeric::rbf {

class GaussianKernel {
public:
  GaussianKernel(real_t h = 1.) noexcept;
  real_t phi(real_t r);
  real_t dphi(real_t r);
  real_t d2phi(real_t r);

private:
  real_t h2_{1.f};
};

class GaussianInverseKernel {
public:
  GaussianInverseKernel(real_t h = 1.) noexcept;
  real_t phi(real_t r);
  real_t dphi(real_t r);
  real_t d2phi(real_t r);

private:
  real_t h2_{1.f};
};

class IMQKernel {
public:
  IMQKernel(real_t h = 1.) noexcept;

  real_t phi(real_t r);
  real_t dphi(real_t r);
  real_t d2phi(real_t r);

private:
  real_t h2_{1.f};
};

class CubicKernel {
public:
  static inline real_t phi(real_t r) { return r * r * r; }
  static inline real_t dphi(real_t r) { return 3 * r; }
  static inline real_t d2phi(real_t r) { return 6 * r; }
};

class QuinticKernel {
public:
  static inline real_t phi(real_t r) {
    real_t r2 = r * r;
    return r2 * r2 * r;
  }
  static inline real_t dphi(real_t r) {
    real_t r2 = r * r;
    return 5 * r2 * r;
  }
  static inline real_t d2phi(real_t r) {
    real_t r2 = r * r;
    return 20 * r2 * r;
  }
};

class HepticKernel {
public:
  static inline real_t phi(real_t r) {
    real_t r2 = r * r;
    return r2 * r2 * r2 * r;
  }
  static inline real_t dphi(real_t r) {
    real_t r2 = r * r;
    return 7 * r2 * r2 * r;
  }
  static inline real_t d2phi(real_t r) {
    real_t r2 = r * r;
    return 42 * r2 * r2 * r;
  }
};

class WendlandKernel {
public:
  WendlandKernel(real_t h = 1.) noexcept;
  real_t phi(real_t);
  real_t dphi(real_t);
  real_t d2phi(real_t);

private:
  real_t h_{1.f};
};

class Wendland32Kernel {
public:
  Wendland32Kernel(real_t h = 1.) noexcept;
  real_t phi(real_t);
  real_t dphi(real_t);
  real_t d2phi(real_t);

private:
  real_t h_{1.f};
};

} // namespace naiades::numeric::rbf

namespace naiades::numeric {

enum class PolynomialType { ZERO, CONSTANT, LINEAR, QUADRATIC, CUBIC };

/// Auxiliary functions for evaluating polynomials.
struct Polynomial2 {
  ///
  static h_size size(PolynomialType polynomial_type);
  /// \note ZERO:      {}
  /// \note CONSTANT:  1
  /// \note LINEAR:    1 x y
  /// \note QUADRATIC: 1 x y xy x^2 y^2
  /// \note CUBIC:     1 x y xy x^2 y^2 x^2y xy^2 x^3 y^3
  static std::vector<real_t> f(PolynomialType polynomial_type,
                               const hermes::geo::point2 &center);
  /// \note ZERO:             {}
  /// \note CONSTANT:         0
  /// \note LINEAR for dx:    0 1 0
  /// \note LINEAR for dy:    0 0 1
  /// \note QUADRATIC for dx: 0 1 0 y 2x 0
  /// \note QUADRATIC for dy: 0 0 1 x 0 2y
  /// \note CUBIC for dx:     0 1 0 y 2x 0 2xy y^2 3x^2 0
  /// \note CUBIC for dy:     0 0 1 x 0 2y x^2 x2y 0 3y^2
  static std::vector<real_t> df(PolynomialType polynomial_type,
                                derivative_bits d,
                                const hermes::geo::point2 &center);
  /// \note ZERO:              {}
  /// \note CONSTANT:          0
  /// \note LINEAR:            0 0 0
  /// \note QUADRATIC for ddx: 0 0 0 0 2 0
  /// \note QUADRATIC for ddy: 0 0 0 0 0 2
  /// \note CUBIC for ddx:     0 0 0 0 2 0 2y 0 6x 0
  /// \note CUBIC for ddy:     0 0 0 0 0 2 0 2x 0 6y
  static std::vector<real_t> ddf(PolynomialType polynomial_type,
                                 derivative_bits d,
                                 const hermes::geo::point2 &center);
};

struct DifferentialRBF2 {
  using MatrixType = Eigen::MatrixX<real_t>;
  using VectorType = Eigen::VectorX<real_t>;
  /// \brief Builds the RBF System matrix augmented by a polynomial.
  ///                      | PHI  P |
  ///                  A = |        |
  ///                      | P^T  0 |
  /// \param centers asd
  /// \note The center of the stencil is assumed to be the first index.
  template <typename KernelFunctionPtr>
  static Result<MatrixType>
  computeA(const std::vector<hermes::geo::point2> &centers,
           const std::vector<h_index> &indices, KernelFunctionPtr rbf,
           PolynomialType polynomial_type = PolynomialType::ZERO) {
    const h_size poly_terms = Polynomial2::size(polynomial_type);
    const h_size stencil_size = indices.size();
    const h_size system_size = stencil_size + poly_terms;

    MatrixType A = MatrixType::Zero(system_size, system_size);

    // PHI
    for (h_index i = 0; i < stencil_size; ++i) {
      for (h_index j = i; j < stencil_size; ++j) {
        if (i == j)
          A(i, i) = rbf.phi(0.0);
        else
          A(i, j) = A(j, i) = rbf.phi(
              hermes::geo::distance(centers[indices[i]], centers[indices[j]]));
      }
    }

    // P
    if (poly_terms > 0) {
      for (h_index i = 0; i < stencil_size; ++i) {
        auto polynomial = Polynomial2::f(polynomial_type, centers[indices[i]]);
        for (h_index j = 0; j < polynomial.size(); ++j)
          A(i, stencil_size + j) = polynomial[j];
      }

      A.bottomLeftCorner(poly_terms, stencil_size) =
          A.topRightCorner(stencil_size, poly_terms).transpose();
    }

    return Result<MatrixType>(std::move(A));

    Eigen::FullPivLU<Eigen::MatrixXd> solver;
    solver.compute(A);
  }

  /// Computes the weights for the gradient operator.
  /// \note The given matrix must match the given parameters:
  ///       - same kernel function
  ///       - same polynomial type
  ///       - same indices and centers
  template <typename KernelFunctionPtr>
  static NaResult
  gradient(DiscreteOperator &dop, const MatrixType &A,
           const std::vector<hermes::geo::point2> &centers,
           const std::vector<h_index> &indices, KernelFunctionPtr rbf,
           PolynomialType polynomial_type = PolynomialType::ZERO) {
    const h_size poly_terms = Polynomial2::size(polynomial_type);
    const h_size stencil_size = indices.size();
    const h_size system_size = stencil_size + poly_terms;

    HERMES_ASSERT(A.rows() == system_size);
    HERMES_ASSERT(A.cols() == system_size);

    VectorType v = VectorType::Zero(system_size);

    // L(phi(x0))
  }
};

} // namespace naiades::numeric
