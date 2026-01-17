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

#include <hermes/math/space_filling.h>

namespace naiades::sampling {

template <typename T>
void sample(const geo::Grid2 &grid, const core::FieldCRef<T> &field,
            h_size component, core::FieldRef<f32> &sample_field) {
  auto field_element = field.element();
  auto sample_element = sample_field.element();
  auto sample_resolution = grid.resolution(sample_element);

#define SRC(IJ)                                                                \
  field[grid.safeFlatIndex(field_element, IJ) -                                \
        grid.flatIndexOffset(field_element)][component]

#define DST(IJ)                                                                \
  sample_field[grid.flatIndex(sample_element, IJ) -                            \
               grid.flatIndexOffset(sample_element)]

  if (field.element() == sample_element) {
    // copy
    for (h_size i = 0; i < field.size(); ++i)
      sample_field[i] = field[i][component];
  } else if (field.element().is(core::element_primitive_bits::face)) {
    if (field.element().alignments().contain(core::element_alignment_bits::x)) {
      // x face is source
      if (sample_element.is(core::element_primitive_bits::cell)) {
        // cell is destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(0, 0)) + SRC(ij.plus(0, 1))) * 0.5;
      } else if (sample_element.is(core::element_primitive_bits::vertex)) {
        // vertex is destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(-1, 0)) + SRC(ij.plus(0, 0))) * 0.5;
      } else {
        // y face is destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(-1, 0)) + SRC(ij.plus(0, 0)) +
                     SRC(ij.plus(0, 1)) + SRC(ij.plus(-1, 1))) *
                    0.25;
      }
    } else if (field.element().alignments().contain(
                   core::element_alignment_bits::y)) {
      // y face is source
      if (sample_element.is(core::element_primitive_bits::cell)) {
        // cell is destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(0, 0)) + SRC(ij.plus(1, 0))) * 0.5;
      } else if (sample_element.is(core::element_primitive_bits::vertex)) {
        // vertex is destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(0, -1)) + SRC(ij.plus(0, 0))) * 0.5;
      } else {
        // x face is destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(0, -1)) + SRC(ij.plus(1, -1)) +
                     SRC(ij.plus(1, 0)) + SRC(ij.plus(0, 0))) *
                    0.25;
      }
    }
  } else if (field.element().is(core::element_primitive_bits::cell)) {
    // cell is source
    if (sample_element.is(core::element_primitive_bits::vertex)) {
      // vertex is destination
      for (auto ij : hermes::range2(sample_resolution))
        DST(ij) = (SRC(ij.plus(-1, -1)) + SRC(ij.plus(-1, 0)) +
                   SRC(ij.plus(0, -1)) + SRC(ij.plus(0, 0))) *
                  0.25;
    } else if (sample_element.is(core::element_primitive_bits::face)) {
      if (sample_element.alignments().contain(
              core::element_alignment_bits::x)) {
        // x faces are the destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(0, -1)) + SRC(ij.plus(0, 0))) * 0.5;
      } else if (sample_element.alignments().contain(
                     core::element_alignment_bits::y)) {
        // y faces are the destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(-1, 0)) + SRC(ij.plus(0, 0))) * 0.5;
      }
    }
  } else if (field.element().is(core::element_primitive_bits::vertex)) {
    // vertex is source
    if (sample_element.is(core::element_primitive_bits::cell)) {
      // cell is destination
      for (auto ij : hermes::range2(sample_resolution))
        DST(ij) = (SRC(ij.plus(0, 0)) + SRC(ij.plus(0, 1)) +
                   SRC(ij.plus(1, 0)) + SRC(ij.plus(1, 1))) *
                  0.25;
    } else if (sample_element.is(core::element_primitive_bits::face)) {
      if (sample_element.alignments().contain(
              core::element_alignment_bits::x)) {
        // x faces are the destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(0, 0)) + SRC(ij.plus(1, 0))) * 0.5;
      } else if (sample_element.alignments().contain(
                     core::element_alignment_bits::y)) {
        // y faces are the destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(0, 0)) + SRC(ij.plus(0, 1))) * 0.5;
      }
    }
  }
#undef SRC
#undef DST
}

template <typename T>
void sample(const geo::Grid2 &grid, const core::FieldCRef<T> &field,
            core::FieldRef<T> &sample_field) {
  auto field_element = field.element();
  auto sample_element = sample_field.element();
  auto sample_resolution = grid.resolution(sample_element);

#define SRC(IJ)                                                                \
  field[grid.safeFlatIndex(field_element, IJ) -                                \
        grid.flatIndexOffset(field_element)]

#define DST(IJ)                                                                \
  sample_field[grid.flatIndex(sample_element, IJ) -                            \
               grid.flatIndexOffset(sample_element)]

  if (field.element() == sample_element) {
    // copy
    for (h_size i = 0; i < field.size(); ++i)
      sample_field[i] = field[i];
  } else if (field.element().is(core::element_primitive_bits::face)) {
    if (field.element().alignments().contain(core::element_alignment_bits::x)) {
      // x face is source
      if (sample_element.is(core::element_primitive_bits::cell)) {
        // cell is destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(0, 0)) + SRC(ij.plus(0, 1))) * 0.5;
      } else if (sample_element.is(core::element_primitive_bits::vertex)) {
        // vertex is destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(-1, 0)) + SRC(ij.plus(0, 0))) * 0.5;
      } else {
        // y face is destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(-1, 0)) + SRC(ij.plus(0, 0)) +
                     SRC(ij.plus(0, 1)) + SRC(ij.plus(-1, 1))) *
                    0.25;
      }
    } else if (field.element().alignments().contain(
                   core::element_alignment_bits::y)) {
      // y face is source
      if (sample_element.is(core::element_primitive_bits::cell)) {
        // cell is destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(0, 0)) + SRC(ij.plus(1, 0))) * 0.5;
      } else if (sample_element.is(core::element_primitive_bits::vertex)) {
        // vertex is destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(0, -1)) + SRC(ij.plus(0, 0))) * 0.5;
      } else {
        // x face is destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(0, -1)) + SRC(ij.plus(1, -1)) +
                     SRC(ij.plus(1, 0)) + SRC(ij.plus(0, 0))) *
                    0.25;
      }
    }
  } else if (field.element().is(core::element_primitive_bits::cell)) {
    // cell is source
    if (sample_element.is(core::element_primitive_bits::vertex)) {
      // vertex is destination
      for (auto ij : hermes::range2(sample_resolution))
        DST(ij) = (SRC(ij.plus(-1, -1)) + SRC(ij.plus(-1, 0)) +
                   SRC(ij.plus(0, -1)) + SRC(ij.plus(0, 0))) *
                  0.25;
    } else if (sample_element.is(core::element_primitive_bits::face)) {
      if (sample_element.alignments().contain(
              core::element_alignment_bits::x)) {
        // x faces are the destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(0, -1)) + SRC(ij.plus(0, 0))) * 0.5;
      } else if (sample_element.alignments().contain(
                     core::element_alignment_bits::y)) {
        // y faces are the destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(-1, 0)) + SRC(ij.plus(0, 0))) * 0.5;
      }
    }
  } else if (field.element().is(core::element_primitive_bits::vertex)) {
    // vertex is source
    if (sample_element.is(core::element_primitive_bits::cell)) {
      // cell is destination
      for (auto ij : hermes::range2(sample_resolution))
        DST(ij) = (SRC(ij.plus(0, 0)) + SRC(ij.plus(0, 1)) +
                   SRC(ij.plus(1, 0)) + SRC(ij.plus(1, 1))) *
                  0.25;
    } else if (sample_element.is(core::element_primitive_bits::face)) {
      if (sample_element.alignments().contain(
              core::element_alignment_bits::x)) {
        // x faces are the destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(0, 0)) + SRC(ij.plus(1, 0))) * 0.5;
      } else if (sample_element.alignments().contain(
                     core::element_alignment_bits::y)) {
        // y faces are the destination
        for (auto ij : hermes::range2(sample_resolution))
          DST(ij) = (SRC(ij.plus(0, 0)) + SRC(ij.plus(0, 1))) * 0.5;
      }
    }
  }
#undef SRC
#undef DST
}

template <typename T>
Result<core::FieldGroup> sample(const geo::Grid2 &grid,
                                const core::FieldCRef<T> &field,
                                core::Element sample_element) {
  core::FieldGroup samples;
  samples.pushField<T>();
  samples.setElement(sample_element);
  NAIADES_HE_RETURN_BAD_RESULT(
      samples.resize(grid.resolution(sample_element).total()));
  if (field.size() != samples.size()) {
    HERMES_ERROR("Sampling field size mismatch element count {} != {}",
                 field.size(), samples.size());
    return NaResult::checkError();
  }
  auto acc = samples.get<T>(0);
  if (field.element() == sample_element)
    for (h_size i = 0; i < samples.size(); ++i)
      acc[i] = field[i];
  else
    sample(grid, field, acc);
  return Result<core::FieldGroup>(std::move(samples));
}

template <typename T>
Result<core::FieldGroup>
sample(const geo::Grid2 &grid, const core::FieldCRef<T> &field,
       const std::vector<hermes::geo::point2> &positions) {
  core::FieldGroup samples;
  samples.pushField<T>();
  samples.setElement(core::Element::Type::VERTEX);
  NAIADES_HE_RETURN_BAD_RESULT(samples.resize(positions.size()));

  auto acc = samples.get<T>(0);
  for (h_size i = 0; i < positions.size(); ++i) {
    acc[i] =
        Stencil::bilinear(grid, field.element(), positions[i]).evaluate(field);
  }

  return Result<core::FieldGroup>(std::move(samples));
}
}; // namespace naiades::sampling
