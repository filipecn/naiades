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

/// \file   morton_tree.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-06-04

#include <naiades/spatial/morton_tree.h>

#include <hermes/math/space_filling.h>

namespace naiades::spatial {

Result<MortonTree2> MortonTree2::build(h_size resolution) {
  MortonTree2 mt;
  mt.resolution_ = resolution;
  auto max_index = hermes::math::space_filling::mortonEncode(
      hermes::index2(resolution - 1, resolution - 1));
  if (max_index >= MORTON_TREE_ELEMENT_INDEX_BOUND)
    return NaResult::badAllocation();
  mt.reset();
  return Result<MortonTree2>(std::move(mt));
}

MortonTree2::MortonTree2() : resolution_{0} { reset(); }

void MortonTree2::reset() {
  active_cells_.reset();
  active_cells_.set(0);
}

NaResult MortonTree2::refine(
    const std::function<bool(const MortonTree2::PredicateData &)> &predicate) {
  if (!predicate)
    return NaResult::inputError();
  std::function<void(h_index)> f;
  f = [&](h_index node) {

  };
  f(0);
  return NaResult::noError();
}

h_index MortonTree2::levelResolution(h_index level) const {
  return 1 << (2 * level);
}

bool MortonTree2::isCellHead(h_index h_index) const { return h_index % 4 == 0; }

h_index MortonTree2::parentIndex(h_index level, h_index z_index) const {}

h_index MortonTree2::level(h_index z_index) const {
  HERMES_ASSERT(z_index < active_cells_.size());
  HERMES_ASSERT(active_cells_.test(z_index));
  // if this is not
  if (!isCellHead(z_index)) {
  }
}

bool MortonTree2::isLeaf(h_index z_index) const {}

h_index MortonTree2::childIndex(h_index z_index) const {}

hermes::range2 MortonTree2::cellIndexBounds(h_index z_index) const {}

} // namespace naiades::spatial
