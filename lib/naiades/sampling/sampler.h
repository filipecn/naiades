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

/// \file   stencil.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Sampler.

#pragma once

#include <naiades/sampling/stencil.h>

namespace naiades::sampling {

template <typename T>
core::Field<T> sample(const geo::Grid2 &grid, const core::Field<T> &field,
                      core::Element sample_element) {
  core::Field<T> samples(grid.resolution(sample_element).total());
  if (field.element() == sample_element) {
    // same element, copy field
    samples = field;
  } else if (field.element() == core::Element::HORIZONTAL_FACE_CENTER &&
             sample_element == core::Element::CELL_CENTER) {
    for (auto ij : hermes::range2(grid.resolution(sample_element)))
      // -x-(i,j+1)
      //  s (i,j)
      // -x-(i,j)
      samples[grid.flatIndex(sample_element, ij)] =
          (field[grid.safeFlatIndex(field.element(), ij)] +
           field[grid.safeFlatIndex(field.element(), ij.plus(0, 1))]) *
          0.5;
  } else if (field.element() == core::Element::VERTICAL_FACE_CENTER &&
             sample_element == core::Element::CELL_CENTER) {
    samples.resize(grid.resolution(sample_element).total());
    for (auto ij : hermes::range2(grid.resolution(sample_element)))
      //   x    s     x
      // (i,j) (i,j) (i+1,j)
      samples[grid.flatIndex(sample_element, ij)] =
          (field[grid.safeFlatIndex(field.element(), ij)] +
           field[grid.safeFlatIndex(field.element(), ij.plus(1, 0))]) *
          0.5;
  } else {
    HERMES_NOT_IMPLEMENTED;
  }
  return samples;
}

template <typename T>
core::Field<T> sample(const geo::Grid2 &grid, const core::Field<T> &field,
                      const std::vector<hermes::geo::point2> &positions) {
  core::Field<T> samples;
  samples.setElement(field.element());
  samples.resize(positions.size());

  for (h_size i = 0; i < positions.size(); ++i) {
    samples[i] =
        Stencil::bilinear(grid, field.element(), positions[i]).evaluate(field);
  }

  return samples;
}

}; // namespace naiades::sampling
