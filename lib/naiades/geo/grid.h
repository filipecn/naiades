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

/// \file   grid.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Grid geometry.

#pragma once

#include <naiades/core/field.h>

#include <hermes/base/size.h>
#include <hermes/geometry/bounds.h>

namespace naiades::geo {

/// \class RegularGrid2
/// \brief Regular grid geometry in 2D
class RegularGrid2 {
public:
  struct Config {
    Config &setSize(const hermes::size2 &size);
    Config &setDomain(const hermes::geo::bounds::bbox2 &region);
    Config &setCellSize(float dx);
    Config &setCellSize(const hermes::geo::vec2 &d);
    Result<RegularGrid2> build() const;

  private:
    hermes::geo::bounds::bbox2 bounds_;
    hermes::size2 resolution_;
    hermes::geo::vec2 dx_;
  };

  /// Grid cell size
  hermes::geo::vec2 cellSize() const;
  /// Grid origin in world space.
  hermes::geo::point2 origin(core::FieldLocation loc) const;
  /// Grid offset in index space.
  hermes::geo::vec2 indexOffset(core::FieldLocation loc) const;
  /// Grid resolution
  hermes::size2 resolution(core::FieldLocation loc) const;
  /// Grid location counts
  h_size locationCount(core::FieldLocation loc) const;
  /// Grid flat index from index
  h_size flatIndex(core::FieldLocation loc, const hermes::index2 &index) const;
  /// Grid index from flat index
  hermes::index2 index(core::FieldLocation loc, h_size flat_index) const;
  /// Grid safe index (clamped)
  hermes::index2 safeIndex(core::FieldLocation loc,
                           const hermes::index2 &index) const;
  /// Grid safe index (clamped)
  h_size safeFlatIndex(core::FieldLocation loc,
                       const hermes::index2 &index) const;
  /// Grid position (index space) from world position
  hermes::geo::point2
  gridPosition(core::FieldLocation loc,
               const hermes::geo::point2 &world_position) const;
  /// Grid location position from flat index
  hermes::geo::point2 position(core::FieldLocation loc,
                               h_size flat_index) const;
  /// Grid location position from index
  hermes::geo::point2 position(core::FieldLocation loc,
                               const hermes::index2 &index) const;
  /// Grid position in world space from grid position (index space).
  hermes::geo::point2 position(core::FieldLocation loc,
                               const hermes::geo::point2 &grid_position) const;

private:
  hermes::geo::bounds::bbox2 bounds_;
  hermes::size2 resolution_;
  hermes::geo::vec2 cell_size_{};
};

} // namespace naiades::geo
