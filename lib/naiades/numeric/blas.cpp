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

/// \file   blas.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-04-04

#include <naiades/numeric/blas.h>

#include <hermes/numeric/numeric.h>

namespace naiades::numeric {

Scalar Scalar::zero(h_size size) { return Scalar::constant(size, 0.0); }

Scalar Scalar::one(h_size size) { return Scalar::constant(size, 1.0); }

Scalar Scalar::constant(h_size size, real_t v) {
  Scalar r;
  r.data_ = std::vector<real_t>(size, v);
  return r;
}

Scalar::Scalar(h_size size) noexcept { data_.resize(size); }

Scalar::Scalar(const Scalar &rhs) noexcept : data_{rhs.data_} {}

Scalar &Scalar::operator=(const Scalar &rhs) {
  data_ = rhs.data_;
  return *this;
}

h_size Scalar::size() const { return data_.size(); }

real_t Scalar::operator[](h_index i) const {
  HERMES_ASSERT(i < data_.size());
  return data_[i];
}

real_t &Scalar::operator[](h_index i) {
  HERMES_ASSERT(i < data_.size());
  return data_[i];
}

hermes::Interval<f32> Scalar::valueRange() const {
  auto n = data_.size();
  hermes::Interval<f32> interval(0, 0);
  if (n > 0) {
    interval.low = (*this)[0];
    interval.high = (*this)[0];
  }
  for (h_index i = 1; i < n; ++i) {
    interval.low = hermes::numbers::cmp::min(interval.low, (*this)[i]);
    interval.high = hermes::numbers::cmp::max(interval.high, (*this)[i]);
  }
  return interval;
}

// arithmetic functions
Scalar &Scalar::operator-=(const Scalar &rhs) {
  for (h_index i = 0; i < data_.size(); ++i)
    data_[i] -= rhs.data_[i];
  return *this;
}
Scalar &Scalar::operator+=(const Scalar &rhs) {
  HERMES_ASSERT(data_.size() == rhs.data_.size());
  for (h_index i = 0; i < data_.size(); ++i)
    data_[i] += rhs.data_[i];
  return *this;
}
Scalar &Scalar::operator*=(const Scalar &rhs) {
  HERMES_ASSERT(data_.size() == rhs.data_.size());
  for (h_index i = 0; i < data_.size(); ++i)
    data_[i] *= rhs.data_[i];
  return *this;
}
Scalar &Scalar::operator/=(const Scalar &rhs) {
  HERMES_ASSERT(data_.size() == rhs.data_.size());
  for (h_index i = 0; i < data_.size(); ++i)
    data_[i] /= rhs.data_[i];
  return *this;
}

Scalar &Scalar::operator-=(real_t s) {
  for (h_index i = 0; i < data_.size(); ++i)
    data_[i] -= s;
  return *this;
}
Scalar &Scalar::operator+=(real_t s) {
  for (h_index i = 0; i < data_.size(); ++i)
    data_[i] += s;
  return *this;
}
Scalar &Scalar::operator*=(real_t s) {
  for (h_index i = 0; i < data_.size(); ++i)
    data_[i] *= s;
  return *this;
}
Scalar &Scalar::operator/=(real_t s) {
  HERMES_ASSERT(!hermes::numbers::cmp::is_zero(s));
  for (h_index i = 0; i < data_.size(); ++i)
    data_[i] /= s;
  return *this;
}

Scalar operator+(const Scalar &lhs, const Scalar &rhs) {
  Scalar r = lhs;
  r += rhs;
  return r;
}
Scalar operator-(const Scalar &lhs, const Scalar &rhs) {
  Scalar r = lhs;
  r -= rhs;
  return r;
}
Scalar operator*(real_t s, const Scalar &field) {
  Scalar r = field;
  r *= s;
  return r;
}
Scalar operator*(const Scalar &field, real_t s) { return s * field; }
Scalar operator*(const Scalar &lhs, const Scalar &rhs) {
  HERMES_ASSERT(lhs.data_.size() == rhs.data_.size());
  Scalar r = lhs;
  r *= rhs;
  return r;
}

// math/trigonometric functions

real_t sum(const Scalar &field) {
  real_t s = 0;
  for (h_index i = 0; i < field.data_.size(); ++i)
    s += field.data_[i];
  return s;
}
Scalar sqr(const Scalar &field) {
  Scalar r = field;
  r *= field;
  return r;
}
Scalar sin(const Scalar &field) {
  Scalar r = field;
  for (h_index i = 0; i < field.data_.size(); ++i)
    r.data_[i] = std::sin(r.data_[i]);
  return r;
}
Scalar sinh(const Scalar &field) {
  Scalar r = field;
  for (h_index i = 0; i < field.data_.size(); ++i)
    r.data_[i] = std::sinh(r.data_[i]);
  return r;
}
Scalar abs(const Scalar &field) {
  Scalar r = field;
  for (h_index i = 0; i < field.data_.size(); ++i)
    r.data_[i] = std::fabs(r.data_[i]);
  return r;
}

} // namespace naiades::numeric