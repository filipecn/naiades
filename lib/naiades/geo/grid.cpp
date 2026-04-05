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

/// \file   grid.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07

#include <naiades/geo/grid.h>

#include <naiades/numeric/boundary.h>

#include <hermes/math/space_filling.h>

namespace naiades::geo {

Result<Grid2> Grid2::Config::build() const {
  Grid2 grid;
  grid.setSize(resolution_);
  grid.setCellSize(cell_size_);
  return Result<Grid2>(std::move(grid));
}

void Grid2::setSize(const hermes::size2 &size) {
  resolution_ = size;
  bounds_.upper = bounds_.lower + hermes::geo::vec2(size.width * cell_size_.x,
                                                    size.height * cell_size_.y);
}

void Grid2::setCellSize(f32 dx) {
  cell_size_ = {dx, dx};
  bounds_.upper =
      bounds_.lower + hermes::geo::vec2(resolution_.width * cell_size_.x,
                                        resolution_.height * cell_size_.y);
}

void Grid2::setCellSize(const hermes::geo::vec2 &cell_size) {
  cell_size_ = cell_size;
  bounds_.upper =
      bounds_.lower + hermes::geo::vec2(resolution_.width * cell_size_.x,
                                        resolution_.height * cell_size_.y);
}

hermes::geo::vec2 Grid2::cellSize() const { return cell_size_; }

hermes::geo::point2 Grid2::origin(core::Element loc) const {
  auto io = gridOffset(loc);
  return {io.x * cell_size_.x, io.y * cell_size_.y};
}

hermes::geo::vec2 Grid2::gridOffset(core::Element loc) const {
  switch (loc) {
  case core::Element::Type::CELL:
    return {0.5f, 0.5f};
  case core::Element::Type::FACE:
    return {0.f, 0.f};
  case core::Element::Type::HORIZONTAL_FACE:
    return {0.5f, 0.f};
  case core::Element::Type::VERTICAL_FACE:
    return {0.f, 0.5f};
  case core::Element::Type::VERTEX:
    return {0.f, 0.f};
  default:
    return {0, 0};
  }
}

hermes::geo::bounds::bbox2 Grid2::bbounds() const {
  return hermes::geo::bounds::bbox2(
      center(core::Element::Type::VERTEX, 0),
      center(core::Element::Type::VERTEX,
             flatIndex(core::Element::Type::VERTEX,
                       hermes::index2(resolution_.width, resolution_.height))));
}

hermes::geo::normal2 Grid2::normal(core::Element loc,
                                   h_index flat_index) const {
  if (loc.is(core::element_primitive_bits::face)) {
    // by design the vertices of a face are sorted in bottom->up, left->right
    // order:
    //            ^
    //            |            - for the left boundary, normal is orth. left
    //         >  >  >
    //       ^         ^       - for the right boundary, normal is orth. right
    //  <--  ^         ^ -->
    //       ^         ^       - for the bottom boundary, normal is orth. right
    //         >  >  >
    //            |            - for the top boundary, normal is orth. left
    //            v
    // and for all the edges in the iterior of the grid, we follow the original
    // order.
    auto vertex_indices = indices(loc, flat_index, core::Element::vertex());
    HERMES_ASSERT(vertex_indices.size() == 2);
    auto edge = center(core::Element::vertex(), vertex_indices[1]) -
                center(core::Element::vertex(), vertex_indices[0]);
    // we choose left or right based on the lower boundaries (left|bottom)
    if (loc == core::Element::Type::HORIZONTAL_FACE) {
      return {index(loc, flat_index).j == 0 ? edge.right() : edge.left()};
    } else {
      return {index(loc, flat_index).i == 0 ? edge.left() : edge.right()};
    }
  }
  // other type of elements have no normal
  return {};
}

h_size Grid2::elementCount(core::Element loc) const {
  switch (loc) {
  case core::Element::Type::CELL:
    return (resolution_.width + 0) * (resolution_.height + 0);
  case core::Element::Type::FACE:
    return (resolution_.width + 1) * (resolution_.height + 0) +
           (resolution_.width + 0) * (resolution_.height + 1);
  case core::Element::Type::HORIZONTAL_FACE:
    return (resolution_.width + 0) * (resolution_.height + 1);
  case core::Element::Type::VERTICAL_FACE:
    return (resolution_.width + 1) * (resolution_.height + 0);
  case core::Element::Type::VERTEX:
    return (resolution_.width + 1) * (resolution_.height + 1);
  default:
    return 0;
  }
}

h_size Grid2::elementIndexOffset(core::Element loc) const {
  if (loc == core::Element::Type::Y_FACE)
    return resolution(core::Element::Type::X_FACE).total();
  return 0;
}

hermes::size2 Grid2::resolution(core::Element loc) const {
  switch (loc) {
  case core::Element::Type::CELL:
    return resolution_;
  case core::Element::Type::FACE:
    HERMES_WARN("Getting face resolution!");
    return resolution_ + hermes::size2(1, 1);
  case core::Element::Type::HORIZONTAL_FACE:
    return resolution_ + hermes::size2(0, 1);
  case core::Element::Type::VERTICAL_FACE:
    return resolution_ + hermes::size2(1, 0);
  case core::Element::Type::VERTEX:
    return resolution_ + hermes::size2(1, 1);
  default:
    return hermes::size2(0, 0);
  }
}

h_size Grid2::flatIndex(core::Element loc, const hermes::index2 &index) const {
  auto res = resolution(loc);
  return elementIndexOffset(loc) + index.j * res.width + index.i;
}

hermes::index2 Grid2::index(core::Element loc, h_size flat_index) const {
  // force face alignment based on index
  if (loc.is(core::element_primitive_bits::face))
    loc = faceType(flat_index);
  auto res = resolution(loc);
  auto local_flat_index = flat_index - elementIndexOffset(loc);
  return hermes::index2(local_flat_index % res.width,
                        local_flat_index / res.width);
}

hermes::geo::point2 Grid2::center(core::Element loc,
                                  const hermes::index2 &index) const {
  auto io = gridOffset(loc);
  return {(index.i + io.x) * cell_size_.x, (index.j + io.y) * cell_size_.y};
}

hermes::geo::point2
Grid2::center(core::Element loc,
              const hermes::geo::point2 &grid_position) const {
  auto io = gridOffset(loc);
  return {(grid_position.x + io.x) * cell_size_.x,
          (grid_position.y + io.y) * cell_size_.y};
}

hermes::index2 Grid2::safeIndex(core::Element loc,
                                const hermes::index2 &index) const {
  auto res = resolution(loc);
  return {hermes::numbers::clamp(index.i, 0, static_cast<int>(res.width - 1)),
          hermes::numbers::clamp(index.j, 0, static_cast<int>(res.height - 1))};
}

h_size Grid2::safeFlatIndex(core::Element loc,
                            const hermes::index2 &index) const {
  auto res = resolution(loc);
  return flatIndex(
      loc,
      {hermes::numbers::clamp(index.i, 0, static_cast<int>(res.width - 1)),
       hermes::numbers::clamp(index.j, 0, static_cast<int>(res.height - 1))});
}

hermes::geo::point2
Grid2::gridPosition(core::Element loc,
                    const hermes::geo::point2 &world_position) const {
  auto o = origin(loc);
  return {(world_position.x - o.x) / cell_size_.x,
          (world_position.y - o.y) / cell_size_.y};
}

hermes::geo::point2 Grid2::center(core::Element loc, h_size flat_index) const {
  // force face alignment based on index
  if (loc.is(core::element_primitive_bits::face))
    loc = faceType(flat_index);
  return center(loc, index(loc, flat_index));
}

std::vector<hermes::geo::point2> Grid2::centers(core::Element loc) const {
  std::vector<hermes::geo::point2> ps;
  if (loc == core::Element::FACE) {
    const core::Element x_element(core::Element::X_FACE);
    const hermes::range2 x_range(resolution(x_element));
    const core::Element y_element(core::Element::Y_FACE);
    const hermes::range2 y_range(resolution(y_element));

    ps.resize(x_range.area() + y_range.area());
    h_size flat_ij = 0;
    for (auto ij : x_range)
      ps[flat_ij++] = center(x_element, ij);
    for (auto ij : y_range)
      ps[flat_ij++] = center(y_element, ij);
  } else {
    const hermes::range2 range(resolution(loc));
    ps.resize(range.area());
    for (auto ij : range)
      ps[range.flatIndex(ij)] = center(loc, ij);
  }
  return ps;
}

std::vector<h_size> Grid2::indices(core::Element element, h_index flat_index,
                                   core::Element sub_element) const {
  // vertices have no sub elements
  if (element == core::Element::VERTEX)
    return {};
  // faces have only vertices as sub-elements
  if (element.is(core::element_primitive_bits::face) &&
      !sub_element.is(core::element_primitive_bits::vertex))
    return {};

  std::vector<h_size> is;
  auto ij = index(element, flat_index);
  const hermes::range2 sub_range(resolution(sub_element));

  if (element == core::Element::CELL) {
    if (sub_element == core::Element::Type::VERTEX) {
      is.emplace_back(safeFlatIndex(sub_element, ij.plus(0, 0)));
      is.emplace_back(safeFlatIndex(sub_element, ij.plus(1, 0)));
      is.emplace_back(safeFlatIndex(sub_element, ij.plus(1, 1)));
      is.emplace_back(safeFlatIndex(sub_element, ij.plus(0, 1)));
    } else if (sub_element == core::Element::Type::FACE) {
      is.emplace_back(
          safeFlatIndex(core::Element::Type::HORIZONTAL_FACE, ij.plus(0, 0)));
      is.emplace_back(
          safeFlatIndex(core::Element::Type::VERTICAL_FACE, ij.plus(0, 0)));
      is.emplace_back(
          safeFlatIndex(core::Element::Type::HORIZONTAL_FACE, ij.plus(1, 0)));
      is.emplace_back(
          safeFlatIndex(core::Element::Type::VERTICAL_FACE, ij.plus(0, 1)));
    } else if (sub_element == core::Element::Type::HORIZONTAL_FACE) {
      is.emplace_back(
          safeFlatIndex(core::Element::Type::HORIZONTAL_FACE, ij.plus(0, 0)));
      is.emplace_back(
          safeFlatIndex(core::Element::Type::HORIZONTAL_FACE, ij.plus(1, 0)));
    } else if (sub_element == core::Element::Type::VERTICAL_FACE) {
      is.emplace_back(
          safeFlatIndex(core::Element::Type::VERTICAL_FACE, ij.plus(0, 0)));
      is.emplace_back(
          safeFlatIndex(core::Element::Type::VERTICAL_FACE, ij.plus(0, 1)));
    } else {
      HERMES_NOT_IMPLEMENTED;
    }
  } else if (element == core::Element::Type::HORIZONTAL_FACE) {
    is.emplace_back(safeFlatIndex(sub_element, ij.plus(0, 0)));
    is.emplace_back(safeFlatIndex(sub_element, ij.plus(1, 0)));
  } else if (element == core::Element::Type::VERTICAL_FACE) {
    is.emplace_back(safeFlatIndex(sub_element, ij.plus(0, 0)));
    is.emplace_back(safeFlatIndex(sub_element, ij.plus(0, 1)));
  } else {
    HERMES_NOT_IMPLEMENTED;
  }
  return is;
}

std::vector<h_size> Grid2::boundaryIndices(core::Element loc) const {
  std::vector<h_size> b;
  if (loc.is(core::element_primitive_bits::face)) {

    // x faces

    core::Element h_face_e(core::Element::Type::X_FACE);
    auto h_face_res = resolution(h_face_e);
    if (loc.has(core::element_orientation_bits::neg_y))
      for (h_size i = 0; i < h_face_res.width; ++i)
        b.emplace_back(flatIndex(h_face_e, hermes::index2(i, 0)));
    if (loc.has(core::element_orientation_bits::y))
      for (h_size i = 0; i < h_face_res.width; ++i)
        b.emplace_back(
            flatIndex(h_face_e, hermes::index2(i, h_face_res.height - 1)));

    // y faces

    core::Element v_face_e(core::Element::Type::Y_FACE);
    auto v_face_res = resolution(v_face_e);
    if (loc.has(core::element_orientation_bits::neg_x))
      for (h_size i = 0; i < v_face_res.height; ++i)
        b.emplace_back(flatIndex(v_face_e, hermes::index2(0, i)));
    if (loc.has(core::element_orientation_bits::x))
      for (h_size i = 0; i < v_face_res.height; ++i)
        b.emplace_back(
            flatIndex(v_face_e, hermes::index2(h_face_res.width, i)));
  } else {
    const auto res = resolution(loc);
    const auto range = hermes::range2(res);
    for (auto o : hermes::math::space_filling::OnionRange(res, 1))
      b.emplace_back(range.flatIndex(o.coord2()));
  }
  return b;
}

core::element_alignments Grid2::elementAlignment(core ::Element loc,
                                                 h_size index) const {
  if (loc.is(core::element_primitive_bits::face)) {
    return index < elementCount(core::Element::Type::X_FACE)
               ? core::element_alignment_bits::x
               : core::element_alignment_bits::y;
  }
  return core::element_alignment_bits::none;
}

core::element_orientations Grid2::elementOrientation(core ::Element loc,
                                                     h_size index) const {
  if (loc.is(core::element_primitive_bits::face)) {
    if (index < elementCount(core::Element::Type::X_FACE)) {
      auto res = resolution(core::Element::Type::X_FACE);
      // x alignment
      if (index < res.width)
        return core::element_orientation_bits::neg_y;
      if (index > res.total() - res.width)
        return core::element_orientation_bits::y;
      return core::element_orientation_bits::any_y;
    } else {
      auto res = resolution(core::Element::Type::Y_FACE);
      auto i = index - elementIndexOffset(core::Element::Y_FACE);
      // y alignment
      if (i % res.width == 0)
        return core::element_orientation_bits::neg_x;
      if ((i + 1) % res.width == 0)
        return core::element_orientation_bits::x;
      return core::element_orientation_bits::any_x;
    }
  }
  return core::element_orientation_bits::none;
}

bool Grid2::isBoundary(core::Element loc, h_size i) const {
  auto res = resolution(loc);
  auto ij = index(loc, i);
  return ij.i <= 0 || ij.i >= static_cast<i32>(res.width) - 1 || ij.j <= 0 ||
         ij.j >= static_cast<i32>(res.height) - 1;
}

h_size Grid2::interiorNeighbour(const core::ElementIndex &boundary_element,
                                const core::Element &interior_loc) const {
  HERMES_ASSERT(
      boundary_element.element.is(core::element_primitive_bits::face));
  HERMES_ASSERT(interior_loc.is(core::element_primitive_bits::cell));
  auto loc = faceType(*boundary_element.index);
  auto flat_index =
      *boundary_element.index +
      (boundary_element.index.isLocal() ? elementIndexOffset(loc) : 0);
  auto bij = index(loc, flat_index);
  auto res = resolution(loc);
  if (loc == core::Element::Type::HORIZONTAL_FACE) {
    if (bij.j == 0)
      return safeFlatIndex(interior_loc, bij);
    if (bij.j == static_cast<i32>(res.height) - 1)
      return safeFlatIndex(interior_loc, bij.down());
  } else if (loc == core::Element::Type::VERTICAL_FACE) {
    if (bij.i == 0)
      return safeFlatIndex(interior_loc, bij);
    if (bij.i == static_cast<i32>(res.width) - 1)
      return safeFlatIndex(interior_loc, bij.left());
  }
  HERMES_ASSERT(false);
  return 0;
}

core::Neighbour Grid2::neighbour(core::Element loc, const hermes::index2 &index,
                                 core::element_orientation_bits orientation,
                                 core::Element boundary_loc) const {
  auto buildNeighbour = [&](core::Element element, const hermes::index2 &ij,
                            real_t distance) -> core::Neighbour {
    auto flat_index = safeFlatIndex(element, ij);
    return {.element_index = core::ElementIndex::global(element, flat_index),
            .distance = distance};
  };
  auto half_dx = cell_size_.x * 0.5f;
  auto half_dy = cell_size_.y * 0.5f;
  //
  //            (i,j+1)
  //           -------
  //         |         |
  //   (i,j) |  [ij]   | (i+1,j)
  //         |         |
  //           -------
  //            (i,j)
  //
  auto res = resolution(loc);
  auto flat_index = safeFlatIndex(loc, index);
  if (loc.is(core::element_primitive_bits::cell) &&
      boundary_loc.is(core::element_primitive_bits::face)) {
    if (orientation == core::element_orientation_bits::down) {
      if (index.j > 0)
        return buildNeighbour(loc, index.down(), cell_size_.y);
      else
        return buildNeighbour(core::Element::Type::X_FACE, index, half_dy);
    } else if (orientation == core::element_orientation_bits::up) {
      if (index.j < static_cast<i32>(res.height) - 1)
        return buildNeighbour(loc, index.up(), cell_size_.y);
      else
        return buildNeighbour(core::Element::Type::X_FACE, index.up(), half_dy);
    } else if (orientation == core::element_orientation_bits::left) {
      if (index.i > 0)
        return buildNeighbour(loc, index.left(), cell_size_.x);
      else
        return buildNeighbour(core::Element::Type::Y_FACE, index, half_dy);
    } else if (orientation == core::element_orientation_bits::right) {
      if (index.i < static_cast<i32>(res.width) - 1)
        return buildNeighbour(loc, index.right(), cell_size_.x);
      else
        return buildNeighbour(core::Element::Type::Y_FACE, index.right(),
                              half_dy);
    } else
      HERMES_ERROR("Invalid neighbour direction {}.",
                   hermes::to_string(orientation));
  } else
    HERMES_ERROR("Invalid neighbour direction pair {} {}.",
                 hermes::to_string(loc), hermes::to_string(boundary_loc));
  return {};
}

core::Element Grid2::faceType(h_size flat_index) const {
  if (flat_index >= elementIndexOffset(core::Element::Y_FACE))
    return core::Element::Y_FACE;
  return core::Element::X_FACE;
}

} // namespace naiades::geo

namespace naiades::numeric {

const geo::Grid2 &Grid2FD::mesh() const {
  return *reinterpret_cast<const geo::Grid2 *>(topology_.get());
}

Result<Grid2FD> Grid2FD::Config::build() const {

  auto grid = geo::Grid2::Ptr::shared();
  grid->setSize(resolution_);
  grid->setCellSize(cell_size_);
  Grid2FD grid_fd;
  grid_fd.topology_ = grid;

  return Result<Grid2FD>(std::move(grid_fd));
}

DiscreteOperator Grid2FD::derivative(derivative_bits d, h_size index,
                                     const core::DiscreteSymbol &sym) const {
  DiscreteOperator op(index);

  // sanity error checks
  auto it = boundaries_.find(sym.boundary_symbol);
  HERMES_ASSERT(it != boundaries_.end() && topology_);

  // get mesh
  const geo::Grid2 *mesh =
      reinterpret_cast<const geo::Grid2 *>(topology_.get());
  // get boundary
  auto &boundary = it->second;

  auto addNeighbour = [&](const core::Neighbour &n, real_t k) {
    if (n.element_index.element != sym.symbol.loc)
      op += boundary.stencil(n.element_index.index) * k;
    else
      op.add(*n.element_index.index, k);
  };
  auto ij = mesh->index(sym.symbol.loc, index);
  if (d == derivative_bits::x) {
    auto left = mesh->neighbour(sym.symbol.loc, ij,
                                core::element_orientation_bits::left,
                                sym.boundary_symbol.loc);
    auto right = mesh->neighbour(sym.symbol.loc, ij,
                                 core::element_orientation_bits::right,
                                 sym.boundary_symbol.loc);
    // TODO assuming ghost point, so boundary distance is hx
    // TODO if we want to allow non-uniform distances we need to consider hx/2
    auto hx = mesh->cellSize().x;
    auto k = 1 / (hx * hx);
    addNeighbour(left, k);
    addNeighbour(right, k);
    op.add(index, -2 * k);
  } else if (d == derivative_bits::y) {
    auto down = mesh->neighbour(sym.symbol.loc, ij,
                                core::element_orientation_bits::down,
                                sym.boundary_symbol.loc);
    auto up =
        mesh->neighbour(sym.symbol.loc, ij, core::element_orientation_bits::up,
                        sym.boundary_symbol.loc);
    // TODO assuming ghost point, so boundary distance is hy
    // TODO if we want to allow non-uniform distances we need to consider hy/2
    auto hy = mesh->cellSize().y;
    auto k = 1 / (hy * hy);
    addNeighbour(down, k);
    addNeighbour(up, k);
    op.add(index, -2 * k);
  } else {
    // err
  }
  return op;
}

DiscreteOperator Grid2FD::laplacian(h_size index,
                                    const core::DiscreteSymbol &sym) const {
  DiscreteOperator op(index);
  op += derivative(derivative_bits::x, index, sym);
  op += derivative(derivative_bits::y, index, sym);
  return op;
}

DiscreteOperator Grid2FD::divergence(const core::Element &loc, h_size index,
                                     const core::Element &vector_loc,
                                     bool staggered) const {
  DiscreteOperator op;
  // if (staggered && loc.is(core::element_primitive_bits::cell) &&
  //     vector_loc.is(core::element_primitive_bits::face)) {
  //   const auto d = mesh_->cellSize();
  //   const auto ij = mesh_->index(core::Element::Type::CELL, index);
  //   op.add(mesh_->flatIndex(core::Element::Type::X_FACE, ij.up()), -d.y);
  //   op.add(mesh_->flatIndex(core::Element::Type::X_FACE, ij), d.y);
  //   op.add(mesh_->flatIndex(core::Element::Type::Y_FACE, ij.right()), -d.x);
  //   op.add(mesh_->flatIndex(core::Element::Type::Y_FACE, ij), d.x);
  //   op *= 0.5;
  // } else {
  //   HERMES_ERROR("divergence for {} and {} not supported!",
  //                hermes::to_string(loc), hermes::to_string(vector_loc));
  // }
  return op;
}

/// Compute divergence field.
// void divergence(const geo::Grid2 &grid, const FieldCRef<f32> &u,
//                 const FieldCRef<f32> &v, FieldRef<f32> &f) {
//   HERMES_ASSERT(u.element() == Element::Type::Y_FACE);
//   HERMES_ASSERT(v.element() == Element::Type::X_FACE);
//   HERMES_ASSERT(f.element() == Element::Type::CELL);
//
// #d efine AT(F, IJ) \
//  F[grid.flatIndex(F.element(), IJ) - grid.elementIndexOffset(F.element())]
//
//   const auto d = grid.cellSize();
//   for (auto ij : hermes::range2(grid.resolution(f.element()))) {
//     AT(f, ij) = -0.5 * (d.y * (AT(v, ij.up()) - AT(v, ij)) +
//                         d.x * (AT(u, ij.right()) - AT(u, ij)));
//   }
// #undef AT
// }

} // namespace naiades::numeric
