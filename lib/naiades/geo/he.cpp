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

/// \file   he.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-04-08

#include <naiades/geo/he.h>

namespace naiades::geo {

h_index HE2::null_index_ = 1 << 20;

h_index HE2::addVertex(const hermes::geo::point2 &position) {
  vertices_.push_back({.position = position, .he_index = null_index_});
  return vertices_.size() - 1;
}

h_index HE2::addCell(const std::vector<h_index> &oriented_vertex_indices) {
  const h_index cell_index = cells_.size();
  const h_size cell_size = oriented_vertex_indices.size();
  std::vector<h_index> internal_half_edges(cell_size);
  for (h_index e = 0; e < cell_size; ++e) {
    // here we receive the internal half-edge index for this face.
    internal_half_edges[e] =
        addOrientedFace(oriented_vertex_indices[e],
                        oriented_vertex_indices[(e + 1) % cell_size]);
    // since we do not accept non-manifold surfaces, he_index must be a boundary
    // face or a new face that has not been assigned to a cell yet.
    HERMES_ASSERT(half_edges_[internal_half_edges[e]].cell_index ==
                  null_index_);
    // assign he_edge to cell
    half_edges_[internal_half_edges[e]].cell_index = cell_index;
  }
  // now we connect the internal half-edges
  for (h_index i = 0; i < internal_half_edges.size(); ++i) {
    half_edges_[internal_half_edges[i]].next_he =
        internal_half_edges[(i + 1) % internal_half_edges.size()];
    half_edges_[internal_half_edges[(i + 1) % internal_half_edges.size()]]
        .prev_he = internal_half_edges[i];
  }
  // for the outer half-edges, we must connect those that are boudary faces
  // because existent internal half-edges should already be connected
  for (h_index internal_he : internal_half_edges) {
    h_index outer_he = heTwin(internal_he);
    // boundary he have no cell associated
    if (half_edges_[outer_he].cell_index == null_index_) {
    }
  }
  return 0;
}

h_index HE2::heTwin(h_index he_index) const {
  return he_index + (he_index % 2 ? -1 : 1);
}

h_index HE2::heFace(h_index he_index) const { return he_index / 2; }

h_index HE2::heCell(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  return half_edges_[he_index].cell_index;
}

h_index HE2::heNext(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  return half_edges_[he_index].next_he;
}

h_index HE2::hePrev(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  return half_edges_[he_index].prev_he;
}

h_index HE2::heStart(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  return half_edges_[he_index].vertex_index;
}

h_index HE2::heEnd(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  return half_edges_[heTwin(he_index)].vertex_index;
}

const hermes::geo::point2 &HE2::heStartPosition(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  auto ve = half_edges_[he_index].vertex_index;
  HERMES_ASSERT(ve < vertices_.size());
  return vertices_[ve].position;
}

const hermes::geo::point2 &HE2::heEndPosition(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  auto ve = half_edges_[heTwin(he_index)].vertex_index;
  HERMES_ASSERT(ve < vertices_.size());
  return vertices_[ve].position;
}

std::vector<h_index> HE2::heLoop(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  std::vector<h_index> loop;
  h_index curr = he_index;
  do {
    loop.emplace_back(curr);
    curr = half_edges_[curr].next_he;
  } while (curr != he_index);
  return loop;
}

h_index HE2::addOrientedFace(h_index va, h_index vb) {
  HERMES_ASSERT(va < vertices_.size() && vb < vertices_.size());
  auto key = HE2::EdgeKey(va, vb);
  auto it = edge_vertices_to_edge_index_map_.find(key);
  if (it != edge_vertices_to_edge_index_map_.end()) {
    // find out which half edge represents va->vb
    return heFace(it->second);
  }
  // left half-edge
  half_edges_.push_back(
      {.vertex_index = va, .cell_index = null_index_, .next_he = null_index_});
  // right half-edge
  half_edges_.push_back(
      {.vertex_index = vb, .cell_index = null_index_, .next_he = null_index_});

  // return left half-edge
  return half_edges_.size() - 2;
}

std::vector<h_index> HE2::leavingHEs(h_index vertex_index) const {
  HERMES_ASSERT(vertex_index < vertices_.size());
  std::vector<h_index> hes;
  auto start = vertices_[vertex_index].he_index;
  HERMES_ASSERT(start != null_index_);
  auto curr = start;
  do {
    hes.emplace_back(curr);
    curr = heTwin(curr);
    curr = half_edges_[curr].next_he;
  } while (curr != start);
  return hes;
}

std::vector<h_index> HE2::arrivingHEs(h_index vertex_index) const {
  HERMES_ASSERT(vertex_index < vertices_.size());
  std::vector<h_index> hes;
  auto start = vertices_[vertex_index].he_index;
  HERMES_ASSERT(start != null_index_);
  start = heTwin(start);
  auto curr = start;
  do {
    hes.emplace_back(curr);
    curr = half_edges_[curr].next_he;
    curr = heTwin(curr);
  } while (curr != start);
  return hes;
}

h_index HE2::computeNextHE(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  auto v = hermes::geo::normalize(heVector(he_index));
  auto next_he = heTwin(he_index);
  auto next_angle = hermes::math::constants::two_pi;
  // the next he is the one that makes the smallest angle with the given
  // edge
  for (auto leaving_he : leavingHEs(heEnd(he_index))) {
    auto w = hermes::geo::normalize(heVector(leaving_he));
    auto angle = std::atan2(hermes::geo::cross(v, w), hermes::geo::dot(v, w)) +
                 hermes::math::constants::two_pi;
    if (angle < next_angle) {
      next_he = leaving_he;
      next_angle = angle;
    }
  }
  return next_he;
}

h_index HE2::computePreviousHE(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  auto v = hermes::geo::normalize(heVector(he_index));
  auto prev_he = heTwin(he_index);
  auto prev_angle = hermes::math::constants::two_pi;
  // the next he is the one that makes the smallest angle with the given
  // edge
  for (auto leaving_he : arrivingHEs(heEnd(he_index))) {
    auto w = -hermes::geo::normalize(heVector(leaving_he));
    auto angle = std::atan2(hermes::geo::cross(v, w), hermes::geo::dot(v, w)) +
                 hermes::math::constants::two_pi;
    if (angle < prev_angle) {
      prev_he = leaving_he;
      prev_angle = angle;
    }
  }
  return 0;
}

hermes::geo::vec2 HE2::heVector(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  return vertices_[half_edges_[heEnd(he_index)].vertex_index].position -
         vertices_[half_edges_[he_index].vertex_index].position;
}

hermes::geo::normal2 HE2::heNormal(h_index he_index) const {
  return hermes::geo::normalize(heVector(he_index).left());
}

hermes::geo::bounds::bbox2 HE2::bbounds() const {
  hermes::geo::bounds::bbox2 bounds;
  for (const auto &vertex : vertices_)
    bounds = hermes::geo::bounds::make_union(bounds, vertex.position);
  return bounds;
}

hermes::geo::normal2 HE2::normal(core::Element loc, h_index index) const {
  HERMES_ASSERT(!loc.is(core::element_primitive_bits::cell));
  if (loc.is(core::element_primitive_bits::face)) {
    auto he_index = index * 2;
    HERMES_ASSERT(he_index < half_edges_.size());
    // boudary normals point outside
    if (half_edges_[he_index + 1].cell_index == null_index_) {
      return heNormal(he_index + 1);
    }
    // internal normals follow the half-edge order
    return heNormal(he_index);
  }
  HERMES_NOT_IMPLEMENTED;
  return {};
}

hermes::geo::point2 HE2::center(core::Element loc, h_size flat_index) const {
  if (loc.is(core::element_primitive_bits::cell)) {
    HERMES_ASSERT(flat_index < cells_.size());
    return cells_[flat_index].center;
  }
  if (loc.is(core::element_primitive_bits::vertex)) {
    HERMES_ASSERT(flat_index < vertices_.size());
    return vertices_[flat_index].position;
  }
  if (loc.is(core::element_primitive_bits::face)) {
    auto he_index = flat_index * 2;
    HERMES_ASSERT(he_index < half_edges_.size());
    return (heStartPosition(he_index) +
            hermes::geo::vec2(heEndPosition(he_index))) /
           2.f;
  }
  HERMES_NOT_IMPLEMENTED;
  return hermes::geo::point2();
}

std::vector<hermes::geo::point2> HE2::centers(core::Element loc) const {
  std::vector<hermes::geo::point2> positions;
  if (loc.is(core::element_primitive_bits::cell)) {
    for (const auto &cell : cells_)
      positions.emplace_back(cell.center);
  }
  if (loc.is(core::element_primitive_bits::vertex)) {
    for (const auto &vertex : vertices_)
      positions.emplace_back(vertex.position);
  }
  if (loc.is(core::element_primitive_bits::face)) {
    for (h_index i = 0; i < half_edges_.size(); i += 2)
      positions.emplace_back(
          (heStartPosition(i) + hermes::geo::vec2(heEndPosition(i))) / 2.f);
  }
  return positions;
}

h_size HE2::elementCount(core::Element loc) const {
  if (loc.is(core::element_primitive_bits::cell)) {
    return cells_.size();
  }
  if (loc.is(core::element_primitive_bits::vertex)) {
    return vertices_.size();
  }
  if (loc.is(core::element_primitive_bits::face)) {
    return half_edges_.size() / 2;
  }
  return 0;
}

h_size HE2::elementIndexOffset(core::Element loc) const {
  HERMES_UNUSED_VARIABLE(loc);
  return 0;
}

std::vector<h_size> HE2::indices(core::Element element, h_index index,
                                 core::Element sub_element) const {
  // vertices have no sub elements
  if (element == core::Element::VERTEX)
    return {};
  // faces have only vertices as sub-elements
  if (element.is(core::element_primitive_bits::face) &&
      !sub_element.is(core::element_primitive_bits::vertex))
    return {};

  std::vector<h_size> is;

  if (element.is(core::element_primitive_bits::cell)) {
    HERMES_ASSERT(index < cells_.size());
    auto loop = heLoop(cells_[index].he_index);
    if (sub_element.is(core::element_primitive_bits::vertex)) {
      for (auto he : loop)
        is.emplace_back(heStart(he));
    } else if (sub_element.is(core::element_primitive_bits::face)) {
      for (auto he : loop)
        is.emplace_back(heFace(he));
    } else {
      HERMES_NOT_IMPLEMENTED;
    }
  } else if (element.is(core::element_primitive_bits::face)) {
    auto he_index = index * 2;
    HERMES_ASSERT(he_index < half_edges_.size());
    is.emplace_back(heStart(he_index));
    is.emplace_back(heEnd(he_index));
  }

  return is;
}

std::vector<h_size> HE2::boundaryIndices(core::Element loc) const {
  HERMES_NOT_IMPLEMENTED;
  return {};
}

core::element_alignments HE2::elementAlignment(core::Element loc,
                                               h_size index) const {
  HERMES_NOT_IMPLEMENTED;
  return {};
}

core::element_orientations HE2::elementOrientation(core::Element loc,
                                                   h_size index) const {
  HERMES_NOT_IMPLEMENTED;
  return {};
}

bool HE2::isBoundary(core::Element loc, h_size index) const {
  HERMES_NOT_IMPLEMENTED;
  return false;
}

h_size HE2::interiorNeighbour(const core::ElementIndex &boundary_element,
                              const core::Element &interior_loc) const {
  HERMES_NOT_IMPLEMENTED;
  return 0;
}

} // namespace naiades::geo