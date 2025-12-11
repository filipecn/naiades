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

/// \file   sim_mesh.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07

#include <hermes/math/space_filling.h>
#include <naiades/core/sim_mesh.h>

namespace naiades::core {

SimMesh::Config SimMesh::Config::from(const geo::RegularGrid2 &grid) {
  SimMesh::Config config;

  auto cell_res = grid.resolution(FieldLocation::CELL_CENTER);
  for (auto o : hermes::math::space_filling::OnionRange(cell_res, 1))
    config.cell_boundary_.primitives.emplace_back(
        grid.flatIndex(FieldLocation::CELL_CENTER, o.coord2()));

  auto h_face_res = grid.resolution(FieldLocation::HORIZONTAL_FACE_CENTER);
  for (h_size i = 0; i < h_face_res.width; ++i)
    config.face_boundary_.primitives.emplace_back(grid.flatIndex(
        FieldLocation::HORIZONTAL_FACE_CENTER, hermes::index2(i, 0)));
  for (h_size i = 0; i < h_face_res.width; ++i)
    config.face_boundary_.primitives.emplace_back(
        grid.flatIndex(FieldLocation::HORIZONTAL_FACE_CENTER,
                       hermes::index2(i, h_face_res.height - 1)));

  auto v_face_res = grid.resolution(FieldLocation::VERTICAL_FACE_CENTER);
  HERMES_LOG_VARIABLE(h_face_res);
  HERMES_LOG_VARIABLE(v_face_res);
  for (h_size i = 0; i < v_face_res.height; ++i)
    config.face_boundary_.primitives.emplace_back(grid.flatIndex(
        FieldLocation::VERTICAL_FACE_CENTER, hermes::index2(0, i)));
  for (h_size i = 0; i < v_face_res.height; ++i)
    config.face_boundary_.primitives.emplace_back(
        grid.flatIndex(FieldLocation::VERTICAL_FACE_CENTER,
                       hermes::index2(h_face_res.width - 1, i)));

  return config;
}

Result<SimMesh> SimMesh::Config::build() const {
  SimMesh sm;
  sm.cell_boundary_ = cell_boundary_;
  sm.face_boundary_ = face_boundary_;
  return Result<SimMesh>(std::move(sm));
}

const Boundary &SimMesh::faceBoundary() const { return face_boundary_; }

const Boundary &SimMesh::cellBoundary() const { return cell_boundary_; }

} // namespace naiades::core
