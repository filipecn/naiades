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

#include <hermes/math/space_filling.h>

namespace naiades {
HERMES_TO_STRING_DEBUG_METHOD_BEGIN(geo::Grid2)
HERMES_PUSH_DEBUG_TITLE
HERMES_PUSH_DEBUG_HERMES_FIELD(bounds_)
HERMES_PUSH_DEBUG_HERMES_FIELD(resolution_)
HERMES_PUSH_DEBUG_HERMES_FIELD(cell_size_)
HERMES_TO_STRING_DEBUG_METHOD_END
} // namespace naiades

namespace naiades::geo {
Grid2::Config &Grid2::Config::setSize(const hermes::size2 &size) {
  resolution_ = size;
  bounds_.upper.x = resolution_.width * cell_size_.x;
  bounds_.upper.y = resolution_.height * cell_size_.y;
  return *this;
}

Grid2::Config &
Grid2::Config::setDomain(const hermes::geo::bounds::bbox2 &region) {
  bounds_ = region;
  resolution_.width = bounds_.extends().x / cell_size_.x;
  resolution_.height = bounds_.extends().y / cell_size_.y;
  return *this;
}

Grid2::Config &Grid2::Config::setCellSize(float dx) {
  cell_size_ = {dx, dx};
  bounds_.upper.x = resolution_.width * cell_size_.x;
  bounds_.upper.y = resolution_.height * cell_size_.y;
  return *this;
}

Grid2::Config &Grid2::Config::setCellSize(const hermes::geo::vec2 &d) {
  cell_size_ = d;
  return *this;
}

Result<Grid2> Grid2::Config::build() const {
  if (((cell_size_.x == 0 || cell_size_.y == 0) &&
       (bounds_.size(0) == 0 || bounds_.size(1) == 0)) ||
      (resolution_.width == 0 || resolution_.height == 0))
    return NaResult::inputError();

  Grid2 grid;
  if (bounds_.size(0) == 0 || bounds_.size(1) == 0) {
    grid.bounds_.lower.x = 0;
    grid.bounds_.lower.y = 0;
    grid.bounds_.upper.x = resolution_.width * cell_size_.x;
    grid.bounds_.upper.y = resolution_.height * cell_size_.y;
  } else
    grid.bounds_ = bounds_;

  if (resolution_.width == 0 || resolution_.height == 0) {
    grid.resolution_.width = bounds_.size(0) / cell_size_.x;
    grid.resolution_.height = bounds_.size(1) / cell_size_.y;
  } else
    grid.resolution_ = resolution_;

  if (cell_size_.x == 0 || cell_size_.y == 0)
    grid.cell_size_ = {bounds_.size(0) / resolution_.width,
                       bounds_.size(1) / resolution_.height};
  else
    grid.cell_size_ = cell_size_;

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

hermes::geo::vec2 Grid2::cellSize() const { return cell_size_; }

hermes::geo::point2 Grid2::origin(core::Element loc) const {
  auto io = gridOffset(loc);
  return {io.x * cell_size_.x, io.y * cell_size_.y};
}

hermes::geo::vec2 Grid2::gridOffset(core::Element loc) const {
  switch (loc) {
  case core::Element::Type::CELL_CENTER:
    return {0.5f, 0.5f};
  case core::Element::Type::FACE_CENTER:
    return {0.f, 0.f};
  case core::Element::Type::HORIZONTAL_FACE_CENTER:
    return {0.5f, 0.f};
  case core::Element::Type::VERTICAL_FACE_CENTER:
    return {0.f, 0.5f};
  case core::Element::Type::VERTEX_CENTER:
    return {0.f, 0.f};
  default:
    return {0, 0};
  }
}

h_size Grid2::locationCount(core::Element loc) const {
  switch (loc) {
  case core::Element::Type::CELL_CENTER:
    return (resolution_.width + 0) * (resolution_.height + 0);
  case core::Element::Type::FACE_CENTER:
    return (resolution_.width + 1) * (resolution_.height + 1);
  case core::Element::Type::HORIZONTAL_FACE_CENTER:
    return (resolution_.width + 0) * (resolution_.height + 1);
  case core::Element::Type::VERTICAL_FACE_CENTER:
    return (resolution_.width + 1) * (resolution_.height + 0);
  case core::Element::Type::VERTEX_CENTER:
    return (resolution_.width + 1) * (resolution_.height + 1);
  default:
    return 0;
  }
}

hermes::size2 Grid2::resolution(core::Element loc) const {
  switch (loc) {
  case core::Element::Type::CELL_CENTER:
    return resolution_;
  case core::Element::Type::FACE_CENTER:
    HERMES_WARN("Getting face resolution!");
    return resolution_ + hermes::size2(1, 1);
  case core::Element::Type::HORIZONTAL_FACE_CENTER:
    return resolution_ + hermes::size2(0, 1);
  case core::Element::Type::VERTICAL_FACE_CENTER:
    return resolution_ + hermes::size2(1, 0);
  case core::Element::Type::VERTEX_CENTER:
    return resolution_ + hermes::size2(1, 1);
  default:
    return hermes::size2(0, 0);
  }
}

h_size Grid2::flatIndex(core::Element loc, const hermes::index2 &index) const {
  auto res = resolution(loc);
  return flatIndexOffset(loc) + index.j * res.width + index.i;
}

h_size Grid2::flatIndexOffset(core::Element loc) const {
  if (loc == core::Element::Type::Y_FACE_CENTER)
    return resolution(core::Element::Type::X_FACE_CENTER).total();
  return 0;
}

hermes::index2 Grid2::index(core::Element loc, h_size flat_index) const {
  auto res = resolution(loc);
  auto local_flat_index = flat_index - flatIndexOffset(loc);
  return hermes::index2(local_flat_index % res.width,
                        local_flat_index / res.width);
}

hermes::geo::point2 Grid2::position(core::Element loc,
                                    const hermes::index2 &index) const {
  auto io = gridOffset(loc);
  return {(index.i + io.x) * cell_size_.x, (index.j + io.y) * cell_size_.y};
}

hermes::geo::point2
Grid2::position(core::Element loc,
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

hermes::geo::point2 Grid2::position(core::Element loc,
                                    h_size flat_index) const {
  return position(loc, index(loc, flat_index));
}

std::vector<hermes::geo::point2> Grid2::positions(core::Element loc) const {
  std::vector<hermes::geo::point2> ps;
  if (loc == core::Element::FACE_CENTER) {
    const core::Element x_element(core::Element::X_FACE_CENTER);
    const hermes::range2 x_range(resolution(x_element));
    const core::Element y_element(core::Element::Y_FACE_CENTER);
    const hermes::range2 y_range(resolution(y_element));

    ps.resize(x_range.area() + y_range.area());
    h_size flat_ij = 0;
    for (auto ij : x_range)
      ps[flat_ij++] = position(x_element, ij);
    for (auto ij : y_range)
      ps[flat_ij++] = position(y_element, ij);
  } else {
    const hermes::range2 range(resolution(loc));
    ps.resize(range.area());
    for (auto ij : range)
      ps[range.flatIndex(ij)] = position(loc, ij);
  }
  return ps;
}

std::vector<std::vector<h_size>> Grid2::indices(core::Element loc,
                                                core::Element sub_loc) const {
  HERMES_ASSERT(loc == core::Element::Type::CELL_CENTER);
  HERMES_ASSERT(sub_loc == core::Element::Type::VERTEX_CENTER);
  const hermes::range2 range(resolution(loc));
  const hermes::range2 sub_range(resolution(sub_loc));
  std::vector<std::vector<h_size>> is(range.area());
  for (auto ij : range) {
    is[range.flatIndex(ij)].emplace_back(sub_range.flatIndex(ij.plus(0, 0)));
    is[range.flatIndex(ij)].emplace_back(sub_range.flatIndex(ij.plus(1, 0)));
    is[range.flatIndex(ij)].emplace_back(sub_range.flatIndex(ij.plus(1, 1)));
    is[range.flatIndex(ij)].emplace_back(sub_range.flatIndex(ij.plus(0, 1)));
  }
  return is;
}

std::vector<h_size> Grid2::boundary(core::Element loc) const {
  std::vector<h_size> b;
  if (loc.is(core::element_primitive_bits::face)) {

    // x faces

    core::Element h_face_e(core::Element::Type::X_FACE_CENTER);
    auto h_face_res = resolution(h_face_e);
    for (h_size i = 0; i < h_face_res.width; ++i)
      b.emplace_back(flatIndex(h_face_e, hermes::index2(i, 0)));
    for (h_size i = 0; i < h_face_res.width; ++i)
      b.emplace_back(
          flatIndex(h_face_e, hermes::index2(i, h_face_res.height - 1)));

    // y faces

    core::Element v_face_e(core::Element::Type::Y_FACE_CENTER);
    auto v_face_res = resolution(v_face_e);
    for (h_size i = 0; i < v_face_res.height; ++i)
      b.emplace_back(flatIndex(v_face_e, hermes::index2(0, i)));
    for (h_size i = 0; i < v_face_res.height; ++i)
      b.emplace_back(
          flatIndex(v_face_e, hermes::index2(h_face_res.width - 1, i)));
  } else {

    const auto cell_res = resolution(loc);
    const auto cell_range = hermes::range2(cell_res);
    for (auto o : hermes::math::space_filling::OnionRange(cell_res, 1))
      b.emplace_back(cell_range.flatIndex(o.coord2()));
  }
  return b;
}

} // namespace naiades::geo
