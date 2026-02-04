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
#include <naiades/core/geometry.h>
#include <naiades/core/neighbourhood.h>
#include <naiades/core/topology.h>
#include <naiades/numeric/spatial_discretization.h>

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
class Grid2 : public core::Topology, public core::Geometry2 {
public:
  using Ptr = hermes::Ref<Grid2>;

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
  hermes::geo::point2 center(core::Element loc,
                             const hermes::index2 &index) const;
  /// World position from grid position (index space).
  hermes::geo::point2 center(core::Element loc,
                             const hermes::geo::point2 &grid_position) const;
  /// The directional neighbour of an element.
  core::Neighbour neighbour(core::Element loc, const hermes::index2 &index,
                            core::element_orientation_bits orientation,
                            core::Element boundary_loc) const;

  // interface

  /// Grid location counts
  h_size elementCount(core::Element loc) const override;
  hermes::geo::point2 center(core::Element loc,
                             h_size flat_index) const override;
  std::vector<hermes::geo::point2> centers(core::Element loc) const override;
  std::vector<std::vector<h_size>>
  indices(core::Element loc, core::Element sub_loc) const override;
  std::vector<h_size> boundary(core::Element loc) const override;
  core::element_alignments elementAlignment(core::Element loc,
                                            h_size index) const override;
  core::element_orientations elementOrientation(core::Element loc,
                                                h_size index) const override;
  bool isBoundary(core::Element loc, h_size index) const override;
  h_size interiorNeighbour(const core::ElementIndex &boundary_element,
                           const core::Element &interior_loc) const override;

private:
  core::Element faceType(h_size flat_index) const;

  hermes::geo::bounds::bbox2 bounds_{{0.f, 0.f}, {1.f, 1.f}};
  hermes::size2 resolution_{100, 100};
  hermes::geo::vec2 cell_size_{0.01};

  NAIADES_to_string_FRIEND(Grid2);
};

} // namespace naiades::geo

namespace naiades::numeric {

class Grid2FD : public SpatialDiscretization {
public:
  Grid2FD(geo::Grid2::Ptr mesh);
  NaResult resolveBoundary(const std::string &field_name) override;
  /// Compute the derivative operator centered at the given element.
  /// \param d Derivative direction.
  /// \param loc
  /// \param index
  /// \param boundary_loc
  /// \param boundary
  virtual DiscreteOperator derivative(derivative_bits d,
                                      const core::Element &loc, h_size index,
                                      core::Element boundary_loc,
                                      const Boundary &boundary) const override;
  /// Compute the discrete Laplacian operator centered at the given element.
  /// \param loc
  /// \param index
  /// \param boundary_loc
  /// \param boundary
  virtual DiscreteOperator laplacian(const core::Element &loc, h_size index,
                                     core::Element boundary_loc,
                                     const Boundary &boundary) const override;
  /// Compute the discrete Divergence operator centered at the given element.
  /// \tparam DiscretizationType Discretization type.
  /// \param boundary
  /// \param loc
  /// \param index
  /// \param staggered
  virtual DiscreteOperator divergence(const core::Element &loc, h_size index,
                                      const core::Element &vector_loc,
                                      bool staggered) const override;

private:
  geo::Grid2::Ptr mesh_;
};

} // namespace naiades::numeric

namespace naiades {

#ifdef NAIADES_INCLUDE_TO_STRING
template <typename T>
std::string spatialFieldString(const geo::Grid2 &grid,
                               const core::FieldCRef<T> &field) {
  auto res = grid.resolution(field.element());
  hermes::cstr s;
  s.appendLine(naiades::to_string(field.element()));
  for (i32 y = res.height - 1; y >= 0; --y) {
    for (i32 x = 0; x < static_cast<i32>(res.width); ++x) {
      s.append(field.at(core::Index::global(
                   grid.safeFlatIndex(field.element(), {x, y}))),
               " ");
    }
    s.append("\n");
  }
  return s.str();
}
#endif

} // namespace naiades
