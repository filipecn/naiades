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

/// \file   smoke_solver.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07

#include <naiades/solvers/smoke_solver.h>

namespace naiades::solvers {

Result<SmokeSolver2> SmokeSolver2::Config::build() const {
  SmokeSolver2 solver;
  for (int i = 0; i < 2; ++i) {
    solver.fields_[i].addScalarField("u", core::Element::Type::X_FACE_CENTER);
    solver.fields_[i].addScalarField("y", core::Element::Type::X_FACE_CENTER);
    solver.fields_[i].addScalarField("density",
                                     core::Element::Type::CELL_CENTER);
  }
  return Result<SmokeSolver2>(std::move(solver));
}

SmokeSolver2::SmokeSolver2() noexcept {}

} // namespace naiades::solvers
