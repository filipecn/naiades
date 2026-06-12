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

#include <naiades/base/debug.h>
#include <naiades/spatial/morton_tree.h>

#include <hermes/math/math.h>
#include <hermes/math/space_filling.h>

namespace naiades::spatial {

MortonTree2::iterator::iterator(const MortonTree2 &mt, h_index z)
    : mt_{mt}, z_{z} {}

MortonTree2::iterator::Leaf MortonTree2::iterator::operator*() const {
  return {
      .bounds = mt_.cellIndexBounds(z_), .level = mt_.level(z_), .z_index = z_};
}

MortonTree2::iterator &MortonTree2::iterator::operator++() {
  z_ = mt_.active_cells_._Find_next(z_);
  return *this;
}

bool MortonTree2::iterator::operator==(const iterator &rhs) const {
  return z_ == rhs.z_;
}

Result<MortonTree2> MortonTree2::fromMaxLevel(h_size max_level) {
  MortonTree2 mt;
  if ((1 << (2 * max_level)) >= MORTON_TREE_ELEMENT_INDEX_BOUND)
    return NaResult::badAllocation();
  mt.resolution_ = 1 << max_level;
  mt.max_level_ = max_level;
  mt.reset();
  return Result<MortonTree2>(std::move(mt));
}

Result<MortonTree2> MortonTree2::fromResolution(h_size resolution) {
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

MortonTree2::iterator MortonTree2::begin() const { return {*this, 0}; }

MortonTree2::iterator MortonTree2::end() const {
  return {*this, active_cells_.size()};
}

void MortonTree2::reset() {
  active_cells_.reset();
  active_cells_.set(0);
}

NaResult MortonTree2::refine(
    const std::function<bool(const MortonTree2::PredicateData &)> &predicate) {
  if (!predicate)
    return NaResult::inputError();
  std::function<void(h_index, h_index)> f;
  f = [&](h_index node, h_index l) {
    if (l == max_level_)
      return;
    h_index children[4];
    if (!childrenIndices(node, l, children))
      return;
    // descent into the tree if this is not a leaf node
    if (isLeaf(node)) {
      MortonTree2::PredicateData p_data;
      p_data.bounds = cellIndexBounds(node);
      p_data.level = l;
      if (predicate(p_data))
        split(node);
    }
    for (h_index i = 0; i < 4; ++i)
      f(children[i], l + 1);
  };
  f(0, 0);
  return NaResult::noError();
}

NaResult MortonTree2::split(h_index z) {
  HERMES_ASSERT(isActive(z));
  HERMES_ASSERT(isCellHead(z));
  auto l = level(z);
  h_index children[4];
  NAIADES_RETURN_BAD_RESULT(childrenIndices(z, l, children));
  for (h_index i = 1; i < 4; ++i) {
    HERMES_ASSERT(!isActive(children[i]));
    active_cells_.set(children[i]);
  }
  return NaResult::noError();
}

NaResult MortonTree2::merge(h_index z, h_index l) {
  HERMES_ASSERT(isActive(z));
  HERMES_ASSERT(isCellHead(z));
  HERMES_ASSERT(l > 0);
  h_index children[4];
  NAIADES_RETURN_BAD_RESULT(childrenIndices(z, l - 1, children));
  for (h_index i = 1; i < 4; ++i) {
    HERMES_ASSERT(isActive(children[i]));
    active_cells_.reset(children[i]);
  }
  return NaResult::noError();
}

bool MortonTree2::isActive(h_index z_index) const {
  HERMES_ASSERT(z_index < active_cells_.size());
  return active_cells_[z_index];
}

NaResult MortonTree2::childrenIndices(h_index z, h_index l,
                                      h_index children_indices[4]) const {
  HERMES_ASSERT(l < max_level_);
  HERMES_ASSERT(isCellHead(z));
  auto s = levelArea(l + 1);
  for (h_index i = 0; i < 4; ++i)
    children_indices[i] = z + i * s;
  return NaResult::noError();
}

h_index MortonTree2::levelResolution(h_index l) const {
  HERMES_ASSERT(l <= max_level_);
  return 1 << (max_level_ - l);
}

h_index MortonTree2::levelArea(h_index l) const {
  HERMES_ASSERT(l <= max_level_);
  return 1 << (2 * (max_level_ - l));
}

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
  h_index l = max_level_;
  while (l > 0 && !isActive(z + levelArea(l))) {
    HERMES_ERROR("{}", z, l, levelArea(l), isActive(z + levelArea(l)));
    l--;
  }
  HERMES_ERROR("level of {}: l {}", z, l);
  return l;
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
  HERMES_WARN("bounds for {}: ij {} l {} s {}", z, hermes::to_string(ij), l, s);
  return hermes::range2(ij, ij.plus(s, s));
}

} // namespace naiades::spatial
