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

namespace naiades {
HERMES_TO_STRING_DEBUG_METHOD_BEGIN(geo::RegularGrid2)
HERMES_PUSH_DEBUG_TITLE
HERMES_PUSH_DEBUG_HERMES_FIELD(bounds_)
HERMES_PUSH_DEBUG_HERMES_FIELD(resolution_)
HERMES_PUSH_DEBUG_HERMES_FIELD(cell_size_)
HERMES_TO_STRING_DEBUG_METHOD_END
} // namespace naiades

namespace naiades::geo {
RegularGrid2::Config &RegularGrid2::Config::setSize(const hermes::size2 &size) {
  resolution_ = size;
  bounds_.upper.x = resolution_.width * cell_size_.x;
  bounds_.upper.y = resolution_.height * cell_size_.y;
  return *this;
}

RegularGrid2::Config &
RegularGrid2::Config::setDomain(const hermes::geo::bounds::bbox2 &region) {
  bounds_ = region;
  resolution_.width = bounds_.extends().x / cell_size_.x;
  resolution_.height = bounds_.extends().y / cell_size_.y;
  return *this;
}

RegularGrid2::Config &RegularGrid2::Config::setCellSize(float dx) {
  cell_size_ = {dx, dx};
  bounds_.upper.x = resolution_.width * cell_size_.x;
  bounds_.upper.y = resolution_.height * cell_size_.y;
  return *this;
}

RegularGrid2::Config &
RegularGrid2::Config::setCellSize(const hermes::geo::vec2 &d) {
  cell_size_ = d;
  return *this;
}

Result<RegularGrid2> RegularGrid2::Config::build() const {
  if (((cell_size_.x == 0 || cell_size_.y == 0) &&
       (bounds_.size(0) == 0 || bounds_.size(1) == 0)) ||
      (resolution_.width == 0 || resolution_.height == 0))
    return NaResult::inputError();

  RegularGrid2 grid;
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

  return Result<RegularGrid2>(std::move(grid));
}

void RegularGrid2::setSize(const hermes::size2 &size) {
  resolution_ = size;
  bounds_.upper = bounds_.lower + hermes::geo::vec2(size.width * cell_size_.x,
                                                    size.height * cell_size_.y);
}

void RegularGrid2::setCellSize(f32 dx) {
  cell_size_ = {dx, dx};
  bounds_.upper =
      bounds_.lower + hermes::geo::vec2(resolution_.width * cell_size_.x,
                                        resolution_.height * cell_size_.y);
}

hermes::geo::vec2 RegularGrid2::cellSize() const { return cell_size_; }

hermes::geo::point2 RegularGrid2::origin(core::FieldLocation loc) const {
  auto io = gridOffset(loc);
  return {io.x * cell_size_.x, io.y * cell_size_.y};
}

hermes::geo::vec2 RegularGrid2::gridOffset(core::FieldLocation loc) const {
  switch (loc) {
  case core::FieldLocation::CELL_CENTER:
    return {0.5f, 0.5f};
  case core::FieldLocation::FACE_CENTER:
    return {0.f, 0.f};
  case core::FieldLocation::HORIZONTAL_FACE_CENTER:
  case core::FieldLocation::V_FACE_CENTER:
  case core::FieldLocation::X_FACE_CENTER:
    return {0.5f, 0.f};
  case core::FieldLocation::VERTICAL_FACE_CENTER:
  case core::FieldLocation::U_FACE_CENTER:
  case core::FieldLocation::Y_FACE_CENTER:
    return {0.f, 0.5f};
  case core::FieldLocation::VERTEX_CENTER:
    return {0.f, 0.f};
  default:
    return {0, 0};
  }
}

h_size RegularGrid2::locationCount(core::FieldLocation loc) const {
  switch (loc) {
  case core::FieldLocation::CELL_CENTER:
    return (resolution_.width + 0) * (resolution_.height + 0);
  case core::FieldLocation::FACE_CENTER:
    return (resolution_.width + 1) * (resolution_.height + 1);
  case core::FieldLocation::HORIZONTAL_FACE_CENTER:
  case core::FieldLocation::V_FACE_CENTER:
  case core::FieldLocation::X_FACE_CENTER:
    return (resolution_.width + 0) * (resolution_.height + 1);
  case core::FieldLocation::VERTICAL_FACE_CENTER:
  case core::FieldLocation::U_FACE_CENTER:
  case core::FieldLocation::Y_FACE_CENTER:
    return (resolution_.width + 1) * (resolution_.height + 0);
  case core::FieldLocation::VERTEX_CENTER:
    return (resolution_.width + 1) * (resolution_.height + 1);
  default:
    return 0;
  }
}

hermes::size2 RegularGrid2::resolution(core::FieldLocation loc) const {
  switch (loc) {
  case core::FieldLocation::CELL_CENTER:
    return resolution_;
  case core::FieldLocation::FACE_CENTER:
    return resolution_ + hermes::size2(1, 1);
  case core::FieldLocation::HORIZONTAL_FACE_CENTER:
  case core::FieldLocation::V_FACE_CENTER:
  case core::FieldLocation::X_FACE_CENTER:
    return resolution_ + hermes::size2(0, 1);
  case core::FieldLocation::VERTICAL_FACE_CENTER:
  case core::FieldLocation::U_FACE_CENTER:
  case core::FieldLocation::Y_FACE_CENTER:
    return resolution_ + hermes::size2(1, 0);
  case core::FieldLocation::VERTEX_CENTER:
    return resolution_ + hermes::size2(1, 1);
  default:
    return hermes::size2(0, 0);
  }
}

h_size RegularGrid2::flatIndex(core::FieldLocation loc,
                               const hermes::index2 &index) const {
  auto res = resolution(loc);
  return index.j * res.width + index.i;
}

hermes::index2 RegularGrid2::index(core::FieldLocation loc,
                                   h_size flat_index) const {
  auto res = resolution(loc);
  return hermes::index2(flat_index % res.width, flat_index / res.width);
}

hermes::geo::point2 RegularGrid2::position(core::FieldLocation loc,
                                           const hermes::index2 &index) const {
  auto io = gridOffset(loc);
  return {(index.i + io.x) * cell_size_.x, (index.j + io.y) * cell_size_.y};
}

hermes::geo::point2 RegularGrid2::position(core::FieldLocation loc,
                                           h_size flat_index) const {
  return position(loc, index(loc, flat_index));
}

hermes::geo::point2
RegularGrid2::position(core::FieldLocation loc,
                       const hermes::geo::point2 &grid_position) const {
  auto io = gridOffset(loc);
  return {(grid_position.x + io.x) * cell_size_.x,
          (grid_position.y + io.y) * cell_size_.y};
}

hermes::index2 RegularGrid2::safeIndex(core::FieldLocation loc,
                                       const hermes::index2 &index) const {
  auto res = resolution(loc);
  return {hermes::numbers::clamp(index.i, 0, static_cast<int>(res.width - 1)),
          hermes::numbers::clamp(index.j, 0, static_cast<int>(res.height - 1))};
}

h_size RegularGrid2::safeFlatIndex(core::FieldLocation loc,
                                   const hermes::index2 &index) const {
  auto res = resolution(loc);
  return flatIndex(
      loc,
      {hermes::numbers::clamp(index.i, 0, static_cast<int>(res.width - 1)),
       hermes::numbers::clamp(index.j, 0, static_cast<int>(res.height - 1))});
}

hermes::geo::point2
RegularGrid2::gridPosition(core::FieldLocation loc,
                           const hermes::geo::point2 &world_position) const {
  auto o = origin(loc);
  return {(world_position.x - o.x) / cell_size_.x,
          (world_position.y - o.y) / cell_size_.y};
}

} // namespace naiades::geo
