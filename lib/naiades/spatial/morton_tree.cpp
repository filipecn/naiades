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

#include <hermes/math/math.h>
#include <hermes/math/space_filling.h>

namespace naiades::spatial {

Result<MortonTree2> MortonTree2::build(h_size resolution) {
  MortonTree2 mt;
  mt.resolution_ = resolution;
  auto max_index = hermes::math::space_filling::mortonEncode(
      hermes::index2(resolution - 1, resolution - 1));
  if (max_index >= MORTON_TREE_ELEMENT_INDEX_BOUND)
    return NaResult::badAllocation();
  mt.max_level_ = hermes::math::log2(resolution);
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

bool MortonTree2::isActive(h_index z_index) const {
  HERMES_ASSERT(z_index < active_cells_.size());
  return active_cells_[z_index];
}

h_index MortonTree2::levelResolution(h_index l) const { return 1 << (2 * l); }

bool MortonTree2::isCellHead(h_index z) const { return z % 4 == 0; }

h_index MortonTree2::parentIndex(h_index l, h_index z) const {
  return z - (z % levelResolution(l));
}

h_index MortonTree2::level(h_index z) const {
  // if this is not a cell head, then it must be a leaf
  if (!isCellHead(z))
    return max_level_;
  // for head indices, we must find out the level by checking the active
  // children
  h_index l = 1;
  while (!isActive(z + levelResolution(l)))
    l++;
  return max_level_ - l;
}

bool MortonTree2::isLeaf(h_index z) const {
  return !isCellHead(z) || !isActive(z + 1);
}

h_index MortonTree2::parentChildIndex(h_index z) const {
  if (isCellHead(z))
    return 0;
  auto l = level(z);
  auto p = parentIndex(l, z);
  return (z - p) / levelResolution(l);
}

hermes::range2 MortonTree2::cellIndexBounds(h_index z) const {
  HERMES_ASSERT(isActive(z));
  auto ij = hermes::math::space_filling::mortonDecode2(z);
  auto l = level(z);
  auto s = levelResolution(l);
  return hermes::range2(ij, ij.plus(s, s));
}

} // namespace naiades::spatial
