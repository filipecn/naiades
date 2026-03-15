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

/// \file   debug.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Auxiliary functions for debugging and error handling.

#pragma once

#include <cassert>
#include <naiades/base/result.h>

#include <hermes/core/debug.h>

#ifndef NAIADES_CHECK_NA_RESULT
#define NAIADES_CHECK_NA_RESULT(A)                                             \
  {                                                                            \
    NaResult _naiades_check_na_error_ = (A);                                   \
    if (!_naiades_check_na_error_) {                                           \
      HERMES_ERROR("Error at: {}", #A);                                        \
      HERMES_ERROR("  w/ err: {}",                                             \
                   hermes::to_string(_naiades_check_na_error_));               \
    }                                                                          \
  }
#endif
#ifndef NAIADES_CHECK_OR_RESULT
#define NAIADES_CHECK_OR_RESULT(A)                                             \
  {                                                                            \
    if (!(A)) {                                                                \
      HERMES_ERROR("Check error: {}", #A);                                     \
      return NaResult::checkError();                                           \
    }                                                                          \
  }
#endif
#ifndef NAIADES_RETURN_BAD_RESULT
#define NAIADES_RETURN_BAD_RESULT(A)                                           \
  {                                                                            \
    NaResult _naiades_return_na_error_ = (A);                                  \
    if (!_naiades_return_na_error_) {                                          \
      HERMES_ERROR("Error at: {}", #A);                                        \
      HERMES_ERROR("  w/ err: {}",                                             \
                   hermes::to_string(_naiades_return_na_error_));              \
      return _naiades_return_na_error_;                                        \
    }                                                                          \
  }
#endif

#ifndef NAIADES_RETURN_ON_BAD_RESULT
#define NAIADES_RETURN_ON_BAD_RESULT(A, R)                                     \
  {                                                                            \
    NaResult _naiades_return_na_error_ = (A);                                  \
    if (!_naiades_return_na_error_) {                                          \
      HERMES_ERROR("Error at: {}", #A);                                        \
      HERMES_ERROR("  w/ err: {}",                                             \
                   hermes::to_string(_naiades_return_na_error_));              \
      return R;                                                                \
    }                                                                          \
  }
#endif
#ifndef NAIADES_HE_RETURN_BAD_RESULT
#define NAIADES_HE_RETURN_BAD_RESULT(A)                                        \
  {                                                                            \
    HeError _naiades_return_na_error_ = (A);                                   \
    if (_naiades_return_na_error_ != HeError::None) {                          \
      HERMES_ERROR("Error at: {}", #A);                                        \
      HERMES_ERROR("  w/ err: {}",                                             \
                   hermes::to_string(_naiades_return_na_error_));              \
      return NaResult::heError(_naiades_return_na_error_);                     \
    }                                                                          \
  }
#endif

#ifndef NAIADES_ASSIGN
#define NAIADES_ASSIGN(R, V)                                                   \
  if (auto _naiades_result_ = V)                                               \
    R = std::move(*_naiades_result_);                                          \
  else {                                                                       \
    HERMES_ERROR("Error at: {} = {}", #R, #V);                                 \
    HERMES_ERROR("  w/ err: {}",                                               \
                 hermes::to_string(_naiades_result_.status()));                \
  }
#endif

#ifndef NAIADES_ASSIGN_OR
#define NAIADES_ASSIGN_OR(R, V, O)                                             \
  if (auto _naiades_result_ = V)                                               \
    R = std::move(*_naiades_result_);                                          \
  else {                                                                       \
    HERMES_ERROR("Error at: {} = {}", #R, #V);                                 \
    HERMES_ERROR("  w/ err: {}",                                               \
                 hermes::to_string(_naiades_result_.status()));                \
    O;                                                                         \
  }
#endif

#ifndef NAIADES_ASSIGN_OR_RETURN_BAD_RESULT
#define NAIADES_ASSIGN_OR_RETURN_BAD_RESULT(R, V)                              \
  if (auto _naiades_result_ = V)                                               \
    R = std::move(*_naiades_result_);                                          \
  else {                                                                       \
    HERMES_ERROR("Error at: {} = {}", #R, #V);                                 \
    HERMES_ERROR("  w/ err: {}",                                               \
                 hermes::to_string(_naiades_result_.status()));                \
    return _naiades_result_.status();                                          \
  }

#endif

#ifndef NAIADES_DECLARE_OR_BAD_RESULT
#define NAIADES_DECLARE_OR_BAD_RESULT(VAR, V)                                  \
  auto _naiades_result_##VAR = V;                                              \
  if (!_naiades_result_##VAR) {                                                \
    HERMES_ERROR("Error at: auto {} = {}", #VAR, #V);                          \
    HERMES_ERROR("  w/ err: {}",                                               \
                 hermes::to_string(_naiades_result_##VAR.status()));           \
    return _naiades_result_##VAR.status();                                     \
  }                                                                            \
  auto VAR = std::move(*_naiades_result_##VAR);

#endif

#ifndef NAIADES_DECLARE_SHARED_PTR_FROM_RESULT_OR_RETURN_BAD_RESULT
#define NAIADES_DECLARE_SHARED_PTR_FROM_RESULT_OR_RETURN_BAD_RESULT(TYPE, PTR, \
                                                                    V)         \
  auto PTR = TYPE::Ptr::shared();                                              \
  if (auto _naiades_result_ = V) {                                             \
    *PTR = std::move(*_naiades_result_);                                       \
  } else {                                                                     \
    HERMES_ERROR("Error at: {}::Ptr {} = {}", #TYPE, #PTR, #V);                \
    HERMES_ERROR("  w/ err: {}",                                               \
                 hermes::to_string(_naiades_result_.status()));                \
    return _naiades_result_.status();                                          \
  }

#endif

#ifndef NAIADES_ASSIGN_RESULT_OR_RETURN
#define NAIADES_ASSIGN_RESULT_OR_RETURN(R, V, B)                               \
  if (auto _naiades_result_ = V)                                               \
    R = std::move(*_naiades_result_);                                          \
  else {                                                                       \
    HERMES_ERROR("Error at: {} = {}", #R, #V);                                 \
    HERMES_ERROR("  w/ err: {}",                                               \
                 hermes::to_string(_naiades_result_.status()));                \
    return B;                                                                  \
  }
#endif

#ifndef NAIADES_ASSIGN_RESULT_OR_RETURN_VOID
#define NAIADES_ASSIGN_RESULT_OR_RETURN_VOID(R, V)                             \
  if (auto _naiades_result_ = V)                                               \
    R = std::move(*_naiades_result_);                                          \
  else {                                                                       \
    HERMES_ERROR("Error at: {} = {}", #R, #V);                                 \
    HERMES_ERROR("  w/ err: {}",                                               \
                 hermes::to_string(_naiades_result_.status()));                \
    return;                                                                    \
  }
#endif
