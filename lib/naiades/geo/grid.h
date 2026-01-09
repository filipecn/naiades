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

#include <naiades/core/discretization.h>
#include <naiades/core/field.h>

#include <hermes/base/size.h>
#include <hermes/geometry/bounds.h>

namespace naiades::geo {

/// \class Grid2
/// \brief  grid geometry in 2D
///
///
/// Given a NxM grid of size (M, N), where N divides the y-axis and M divides
/// the x-axis, indices are laid out by rows (x-aligned). Therefore an element
/// index coordinate (i, j) corresponds its pair the (x, y) position. The
/// flat index of element (i, j) is computed as j * M + i:
///           _
///          |   (N-1)*M ------> (M*N)-1
///        N |        ...
///  y       |   M ------------> (2*M)-1
///  |       |_  0 ------------> M-1
///  ---x       |------------|
///                   M
///
/// Faces are divided into grids based on their alignment:
///  - x-faces (--) consisting of x-aligned faces;
///  - y-faces (|) consisting of y-aligned faces.
///
///           -- -- -- -- --
///          |  |  |  |  |  |     x-faces grid size: (M, N+1)
///           -- -- -- -- --
///          |  |  |  |  |  |     y-faces grid size: (M+1, N)
///           -- -- -- -- --
///   y      |  |  |  |  |  |
///   |       -- -- -- -- --
///   ---x
///
/// Each group of faces (x-faces and y-faces) are indexed the same way as other
/// element grids as described earlier:
///
///         x-faces local indices           y-faces local indices
///
///          N*M           N*(N+1)-1      M*(N-1)            (M+1)*N-1
///           -- -- -- -- --                  |  |  |  |  |  |
///                ...
///   y       -- -- -- -- --                  |  |  |  |  |  |
///   |       0           M-1                 0              M
///   ---x
///
/// However the general index of a face is calculated from the concatenation
/// the two grids above in order [x-faces, y-faces]. Thus flat indices of
/// y-faces come after x-faces:
///
///   - flat x-face index (i, j): j * M + i
///   - flat y-face index (i, j): M * (N + 1) + j * (M + 1) + i
///
class Grid2 : public core::DiscretizationGeometry2 {
public:
  struct Config {
    Config &setSize(const hermes::size2 &size);
    Config &setDomain(const hermes::geo::bounds::bbox2 &region);
    Config &setCellSize(float dx);
    Config &setCellSize(const hermes::geo::vec2 &d);
    Result<Grid2> build() const;

  private:
    hermes::geo::bounds::bbox2 bounds_{{0.f, 0.f}, {1.f, 1.f}};
    hermes::size2 resolution_{100, 100};
    hermes::geo::vec2 cell_size_{0.01, 0.01};
  };

  ///
  void setSize(const hermes::size2 &size);
  ///
  void setCellSize(f32 dx);

  /// Grid cell size
  hermes::geo::vec2 cellSize() const;
  /// Grid origin in world space.
  hermes::geo::point2 origin(core::Element loc) const;
  /// Grid offset in index space.
  hermes::geo::vec2 gridOffset(core::Element loc) const;
  /// Grid resolution
  hermes::size2 resolution(core::Element loc) const;
  /// Grid flat index offset.
  /// \note The flat index offset is zero for all elements, except for faces.
  h_size flatIndexOffset(core::Element loc) const;
  /// Grid flat index from index
  h_size flatIndex(core::Element loc, const hermes::index2 &index) const;
  /// Grid index from flat index
  hermes::index2 index(core::Element loc, h_size flat_index) const;
  /// Grid safe index (clamped)
  hermes::index2 safeIndex(core::Element loc,
                           const hermes::index2 &index) const;
  /// Grid safe index (clamped)
  h_size safeFlatIndex(core::Element loc, const hermes::index2 &index) const;
  /// Grid position (index space) from world position
  hermes::geo::point2
  gridPosition(core::Element loc,
               const hermes::geo::point2 &world_position) const;
  /// World position from index
  hermes::geo::point2 position(core::Element loc,
                               const hermes::index2 &index) const;
  /// World position from grid position (index space).
  hermes::geo::point2 position(core::Element loc,
                               const hermes::geo::point2 &grid_position) const;

  // interface

  /// Grid location counts
  h_size elementCount(core::Element loc) const override;
  hermes::geo::point2 position(core::Element loc,
                               h_size flat_index) const override;
  std::vector<hermes::geo::point2> positions(core::Element loc) const override;
  std::vector<std::vector<h_size>>
  indices(core::Element loc, core::Element sub_loc) const override;
  std::vector<h_size> boundary(core::Element loc) const override;
  core::element_alignments elementAlignment(core::Element loc,
                                            h_size index) const override;
  core::element_orientations elementOrientation(core::Element loc,
                                                h_size index) const override;
  bool isBoundary(core::Element loc, h_size index) const override;
  std::vector<core::Neighbour> star(core::Element loc, h_size index,
                                    core::Element boundary_loc) const override;

private:
  hermes::geo::bounds::bbox2 bounds_{{0.f, 0.f}, {1.f, 1.f}};
  hermes::size2 resolution_{100, 100};
  hermes::geo::vec2 cell_size_{0.01};

  NAIADES_to_string_FRIEND(Grid2);
};

} // namespace naiades::geo
