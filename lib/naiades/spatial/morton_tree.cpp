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
      .bounds = mt_.cellBounds(z_), .level = mt_.cellLevel(z_), .z_index = z_};
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
      if (predicate(p_data)) {
        split(node, l);
      } else
        return;
    }
    for (h_index i = 0; i < 4; ++i)
      f(children[i], l + 1);
  };
  f(0, 0);
  return NaResult::noError();
}

hermes::geo::bounds::bbox2 MortonTree2::bounds() const {
  return hermes::geo::bounds::bbox2(
      hermes::geo::point2(),
      g2w_(hermes::geo::point2(resolution_, resolution_)));
}

hermes::range2 MortonTree2::indexBounds() const {
  return hermes::range2(
      {0, 0}, {static_cast<i32>(resolution_), static_cast<i32>(resolution_)});
}

NaResult MortonTree2::split(h_index z, h_index level) {
  HERMES_ASSERT(isActive(z));
  HERMES_ASSERT(isCellHead(z));
  HERMES_ASSERT(level < max_level_);
  h_index children[4];
  NAIADES_RETURN_BAD_RESULT(childrenIndices(z, level, children));
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

h_size MortonTree2::maxResolution() const { return resolution_; }

hermes::geo::point2 MortonTree2::indexPosition(h_index z_index) const {
  auto ij = hermes::math::space_filling::mortonDecode2(z_index);
  return g2w_(hermes::geo::point2(ij));
}

hermes::geo::point2 MortonTree2::position(const hermes::geo::point2 &gp) const {
  return g2w_(gp);
}

bool MortonTree2::isActive(h_index z_index) const {
  HERMES_ASSERT(z_index < active_cells_.size());
  return active_cells_[z_index];
}

NaResult MortonTree2::childrenIndices(h_index z, h_index l,
                                      h_index children_indices[4]) const {
  HERMES_ASSERT(l < max_level_);
  HERMES_ASSERT(isCellHead(z));
  auto s = levelCellArea(l + 1);
  for (h_index i = 0; i < 4; ++i)
    children_indices[i] = z + i * s;
  return NaResult::noError();
}

h_index MortonTree2::levelCellSize(h_index l) const {
  HERMES_ASSERT(l <= max_level_);
  return 1 << (max_level_ - l);
}

h_index MortonTree2::levelCellArea(h_index l) const {
  HERMES_ASSERT(l <= max_level_);
  return 1 << (2 * (max_level_ - l));
}

bool MortonTree2::isCellHead(h_index z) const { return z % 4 == 0; }

h_index MortonTree2::parentIndex(h_index child_level, h_index z) const {
  HERMES_ASSERT(child_level > 0);
  return z - (z % levelCellArea(child_level - 1));
}

h_index MortonTree2::cellLevel(h_index z) const {
  // if this is not a cell head, then it must be a leaf
  if (!isCellHead(z))
    return max_level_;
  // for head indices, we descent the levels until the node is not the head
  // of that level or any of its siblings is active
  h_index l = max_level_;
  while (l > 0 && parentChildIndex(z, l) == 0 &&
         !isActive(z + levelCellArea(l)))
    l--;
  return l;
}

bool MortonTree2::isLeaf(h_index z) const {
  HERMES_ASSERT(isActive(z));
  auto level = cellLevel(z);
  if (level == max_level_)
    return true;
  auto s = levelCellArea(level + 1);
  // check second child
  return !isActive(z + s);
}

h_index MortonTree2::parentChildIndex(h_index z, h_index child_level) const {
  // if this is not a cell head, it must be in max_level
  if (!isCellHead(z)) {
    HERMES_ASSERT(child_level == max_level_);
    return z % 4;
  }
  HERMES_ASSERT(child_level > 0);
  auto p = parentIndex(child_level, z);

  return (z - p) / levelCellArea(child_level);
}

hermes::range2 MortonTree2::cellIndexBounds(h_index z) const {
  if (!isActive(z)) {
    HERMES_ERROR("{}", z);
  }
  HERMES_ASSERT(isActive(z));
  auto ij = hermes::math::space_filling::mortonDecode2(z);
  auto l = cellLevel(z);
  auto s = levelCellSize(l);
  return hermes::range2(ij, ij.plus(s, s));
}

hermes::geo::bounds::bbox2 MortonTree2::cellBounds(h_index z) const {
  auto range = cellIndexBounds(z);
  return hermes::geo::bounds::bbox2(g2w_(hermes::geo::point2(range.lower())),
                                    g2w_(hermes::geo::point2(range.upper())));
}

} // namespace naiades::spatial
