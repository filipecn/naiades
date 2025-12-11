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

/// \file   math.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Math utils

#pragma once

#include <hermes/geometry/point.h>

namespace naiades::utils {

f32 gaussian(f32 sigma2, f32 mu, f32 x);

f32 gaussian(const hermes::geo::vec2 &sigma2, const hermes::geo::point2 &mu,
             const hermes::geo::point2 &p);

hermes::geo::vec2 enright(const hermes::geo::point2 &p, float t);

/// Constant vorticity velocity field
/// \param p Evaluation point
/// \param center Rotation center
/// \param omega Angular velocity (radians per second)
hermes::geo::vec2 zalesak(const hermes::geo::point2 &p,
                          const hermes::geo::point2 &center, float omega);

} // namespace naiades::utils
