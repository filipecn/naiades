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

#include <naiades/numeric/rbf.h>

#include <hermes/math/math.h>

namespace naiades::numeric::rbf {

GaussianKernel::GaussianKernel(real_t h) noexcept : h2_{h * h} {}

real_t GaussianKernel::phi(real_t d) {
  real_t d2 = d * d;
  return std::exp(-d2 * h2_);
}

real_t GaussianKernel::dphi(real_t d) {
  real_t d2 = d * d;
  return (-2 * h2_) * std::exp(-d2 * h2_);
}

real_t GaussianKernel::d2phi(real_t d) {
  real_t d2 = d * d;
  return (2 * h2_) * std::exp(-d2 * h2_) * (2 * d2 * h2_ - 1);
}

GaussianInverseKernel::GaussianInverseKernel(real_t h) noexcept : h2_(h * h) {}

real_t GaussianInverseKernel::phi(real_t d) {
  real_t d2 = d * d;
  return std::exp(-d2 / h2_);
}

real_t GaussianInverseKernel::dphi(real_t d) {
  real_t d2 = d * d;
  return (-2 / h2_) * std::exp(-d2 / h2_);
}

real_t GaussianInverseKernel::d2phi(real_t d) {
  real_t d2 = d * d;
  return (2 / h2_) * std::exp(-d2 / h2_) * (2 * d2 / h2_ - 1);
}

IMQKernel::IMQKernel(real_t h) noexcept : h2_(h * h) {}

real_t IMQKernel::phi(real_t d) {
  real_t d2 = d * d;
  return 1 / std::sqrt(1 + h2_ * d2);
}

real_t IMQKernel::dphi(real_t d) {
  real_t d2 = d * d;
  real_t den = (1 + d2 * h2_);

  return -(h2_) / (den * std::sqrt(den));
}

real_t IMQKernel::d2phi(real_t d) {
  real_t d2 = d * d;
  real_t den = (1 + d2 * h2_);
  return h2_ * (2 * h2_ * d2 - 1) / (den * den * std::sqrt(den));
}

WendlandKernel::WendlandKernel(real_t h) noexcept : h_(h) {}

real_t WendlandKernel::phi(real_t d) {
  auto h_d = h_ * d;
  auto h_d2 = h_d * h_d;
  return std::pow(std::max(0.0, 1.0 - h_d), 8) *
         (32.0 * h_d2 * h_d + 25.0 * h_d2 + 8.0 * h_d + 1.0);
}

real_t WendlandKernel::dphi(real_t d) {
  auto h_d = h_ * d;
  return -22.0 * h_ * h_ * std::pow(std::max(0.0, 1.0 - h_d), 7) +
         (16.0 * h_d * h_d + 7.0 * h_ * d + 1.0);
}

real_t WendlandKernel::d2phi(real_t d) {
  auto h_d = h_ * d;
  return 528.0 * std::pow(h_, 4) * std::pow(std::max(0.0, 1.0 - h_d), 6) +
         (6.0 * h_d + 1.0);
}

real_t Wendland32Kernel::phi(real_t d) {
  real_t d2 = d * d, h2 = h_ * h_;
  return std::pow(1 - d * h_, 6) + (35 * d2 * h2 + 18 * d * h_ + 3);
}

real_t Wendland32Kernel::dphi(real_t d) {
  real_t h2 = h_ * h_;
  return -56 * h2 * (5 * h_ * d + 1) * std::pow(1 - d * h_, 5);
}

real_t Wendland32Kernel::d2phi(real_t d) {
  real_t d2 = d * d, h2 = h_ * h_;
  return 56 * h2 * (35 * d2 * h2 - 4 * h_ * d - 1) * std::pow(1 - d * h_, 4);
}

} // namespace naiades::numeric::rbf

namespace naiades::numeric {

h_size Polynomial2::size(PolynomialType polynomial_type) {
  switch (polynomial_type) {
  case PolynomialType::ZERO:
    return 0;
  case PolynomialType::CONSTANT:
    return 1;
  case PolynomialType::LINEAR:
    return 3;
  case PolynomialType::QUADRATIC:
    return 6;
  case PolynomialType::CUBIC:
    return 10;
  default:
    return 0;
  }
  return 0;
}

std::vector<real_t> Polynomial2::f(PolynomialType polynomial_type,
                                   const hermes::geo::point2 &center) {
  const real_t x = center.x;
  const real_t y = center.y;
  switch (polynomial_type) {
  case PolynomialType::ZERO:
    return {};
  case PolynomialType::CONSTANT:
    return {1.0};
  case PolynomialType::LINEAR:
    return {1.0, x, y};
  case PolynomialType::QUADRATIC:
    return {1.0, x, y, x * y, x * x, y * y};
  case PolynomialType::CUBIC:
    return {1.0,   x,         y,         x * y,     x * x,
            y * y, x * x * y, x * y * y, x * x * x, y * y * y};
  }
  return {};
}

std::vector<real_t> Polynomial2::df(PolynomialType polynomial_type,
                                    derivative_bits d,
                                    const hermes::geo::point2 &center) {
  HERMES_ASSERT(d == derivative_bits::x || d == derivative_bits::y);
  const real_t x = center.x;
  const real_t y = center.y;
  switch (polynomial_type) {
  case PolynomialType::ZERO:
    return {};
  case PolynomialType::CONSTANT:
    return {0.0};
  case PolynomialType::LINEAR:
    if (d == derivative_bits::x)
      return {0.0, 1.0, 0.0};
    return {0.0, 0.0, 1.0};
  case PolynomialType::QUADRATIC:
    if (d == derivative_bits::x)
      return {0.0, 1.0, 0.0, y, 2 * x, 0.0};
    return {0.0, 0.0, 1.0, x, 0.0, 2 * y};
  case PolynomialType::CUBIC:
    if (d == derivative_bits::x)
      return {0.0, 1.0, 0.0, y, 2 * x, 0.0, 2 * x * y, y * y, 3 * x * x, 0.0};
    return {0.0, 0.0, 1.0, x, 0.0, 2 * y, x * x, 2 * x * y, 0.0, 3 * y * y};
  }
  return {};
}

std::vector<real_t> Polynomial2::ddf(PolynomialType polynomial_type,
                                     derivative_bits d,
                                     const hermes::geo::point2 &center) {
  HERMES_ASSERT(d == derivative_bits::x || d == derivative_bits::y);
  const real_t x = center.x;
  const real_t y = center.y;
  switch (polynomial_type) {
  case PolynomialType::ZERO:
    return {};
  case PolynomialType::CONSTANT:
    return {0.0};
  case PolynomialType::LINEAR:
    if (d == derivative_bits::x)
      return {0.0, 0.0, 0.0};
    return {0.0, 0.0, 0.0};
  case PolynomialType::QUADRATIC:
    if (d == derivative_bits::x)
      return {0.0, 0.0, 0.0, 0.0, 2, 0.0};
    return {0.0, 0.0, 0.0, 0.0, 0.0, 2};
  case PolynomialType::CUBIC:
    if (d == derivative_bits::x)
      return {0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 2 * y, 0.0, 6 * x, 0.0};
    return {0.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 2 * x, 0.0, 6 * y};
  }
  return {};
}

} // namespace naiades::numeric
