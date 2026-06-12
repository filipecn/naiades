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

/// \file   morton_tree.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-06-04

#pragma once

#include <naiades/base/result.h>

#include <hermes/base/index.h>

#include <bitset>
#include <functional>

#define MORTON_TREE_ELEMENT_INDEX_BOUND 1 << 10

namespace naiades::spatial {

class MortonTree2 {
public:
  class iterator {
  public:
    struct Leaf {
      hermes::range2 bounds;
      h_index level;
      h_index z_index;
    };

    Leaf operator*() const;
    iterator &operator++();
    bool operator==(const iterator &rhs) const;

  private:
    friend class MortonTree2;
    iterator(const MortonTree2 &mt, h_index z);

    const MortonTree2 &mt_;
    h_index z_;
  };

  /// \brief
  /// \param max_level
  /// \return
  static Result<MortonTree2> fromMaxLevel(h_size max_level);
  /// \brief
  /// \param resolution
  /// \return
  static Result<MortonTree2> fromResolution(h_size resolution);

  MortonTree2();
  ~MortonTree2() = default;

  iterator begin() const;
  iterator end() const;

  /// /brief
  void reset();

  struct PredicateData {
    hermes::range2 bounds;
    h_index level;
  };

  /// /brief
  /// /param predicate
  /// /return
  NaResult refine(const std::function<bool(const PredicateData &)> &predicate);

private:
  ///
  NaResult split(h_index z_index);
  ///
  NaResult merge(h_index z_index, h_index l);
  /// \return true if the cell indexed by z-index is active.
  bool isActive(h_index z_index) const;
  ///
  NaResult childrenIndices(h_index z_index, h_index l,
                           h_index children_indices[4]) const;
  /// \return the side length of a cell at the given level.
  h_index levelResolution(h_index level) const;
  /// \return the area of a cell at the given level.
  h_index levelArea(h_index level) const;
  /// \return true if the given index can be a cell head.
  bool isCellHead(h_index h_index) const;
  /// The parent index at a given level is the largest z-index value multiple
  /// of the size of the level that is smaller or equal the given index.
  /// \return the index of the parent of the given index at specified level.
  h_index parentIndex(h_index level, h_index z_index) const;
  /// \note this assumes z_index is active.
  /// \return The level of the given node.
  h_index level(h_index active_z_index) const;
  /// \return true if the given node is a leaf node.
  bool isLeaf(h_index z_index) const;
  /// \return The child index [0-4] of the given node in the parent children
  /// list.
  h_index parentChildIndex(h_index z_index) const;
  /// \return The index area covered by the given cell.
  hermes::range2 cellIndexBounds(h_index z_index) const;

  std::bitset<MORTON_TREE_ELEMENT_INDEX_BOUND> active_cells_;
  h_size resolution_{0};
  h_index max_level_{0};

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS
  friend struct hermes::DebugTraits<MortonTree2>;
#endif
};

} // namespace naiades::spatial

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS

namespace hermes {

template <> struct DebugTraits<naiades::spatial::MortonTree2> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::spatial::MortonTree2 &data) {
    auto m = DebugMessage();
    m.addTitle("Morton Tree");
    m.add("resolution", data.resolution_);
    return m;
  }
};

} // namespace hermes

#endif
