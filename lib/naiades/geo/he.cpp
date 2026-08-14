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

#include <unordered_set>

namespace naiades::geo {

h_index HE2::null_index_ = 1 << 20;

HE2::HE2() noexcept : boundary_start_he_{HE2::null_index_} {}

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
  // update boundary start he to any
  for (h_index internal_he : internal_half_edges) {
    auto he = heTwin(internal_he);
    if (half_edges_[he].cell_index == null_index_) {
      boundary_start_he_ = he;
    }
  }

  // cell center
  hermes::geo::point2 center;
  for (const auto &v : oriented_vertex_indices)
    center += hermes::geo::vec2(vertices_[v].position);
  center /= static_cast<real_t>(oriented_vertex_indices.size());
  cells_.push_back({.center = center, .he_index = internal_half_edges[0]});
  return cells_.size() - 1;
}

void HE2::optimizeIndices() {
  // For each element (he, vertex, and cell) the boundary is put at the
  // beginning of their respective lists:
  //  indices [0, boundary_count) are boundary elements
  //  indices [boundary_count, element_count) interior elements
  // Boundary elements follow the he order, but interior elements do not follow
  // any order.
  // TODO: interior elements could be sorted to improve cache hits.

  // old -> new
  std::unordered_map<h_index, h_index> new_cell_index_map;
  // old -> new
  std::unordered_map<h_index, h_index> new_vertex_index_map;
  // new -> old
  std::unordered_map<h_index, h_index> old_vertex_index_map;

  // compute vertex and cell boundary index maps
  auto cur_he = boundary_start_he_;
  do {
    auto the = heTwin(cur_he);
    h_index cell_index = half_edges_[the].cell_index;
    if (!new_cell_index_map.contains(cell_index)) {
      new_cell_index_map[cell_index] = new_cell_index_map.size();
    }
    h_index old_vertex_index = half_edges_[cur_he].vertex_index;
    HERMES_ASSERT(!new_vertex_index_map.contains(old_vertex_index));
    old_vertex_index_map[new_vertex_index_map.size()] = old_vertex_index;
    new_vertex_index_map[old_vertex_index] = new_vertex_index_map.size();
    cur_he = heNext(cur_he);
  } while (cur_he != null_index_ && cur_he != boundary_start_he_);

  h_size boundary_vertex_count = new_vertex_index_map.size();
  h_size boundary_cell_count = new_cell_index_map.size();
  h_size boundary_edge_count = boundary_vertex_count;

  // compute interior vertex map
  for (h_index i = 0; i < vertices_.size(); i++) {
    if (!new_vertex_index_map.contains(i)) {
      old_vertex_index_map[new_vertex_index_map.size()] = i;
      new_vertex_index_map[i] = new_vertex_index_map.size();
    }
  }
  HERMES_ASSERT(new_vertex_index_map.size() == vertices_.size());

  // compute interior cell map
  for (h_index i = 0; i < cells_.size(); i++) {
    if (!new_cell_index_map.contains(i)) {
      new_cell_index_map[i] = new_cell_index_map.size();
    }
  }
  HERMES_ASSERT(new_cell_index_map.size() == cells_.size());

  // recompute half-edges maps

  // (min(va, vb), max(va, vb)) -> half_edges even index
  std::unordered_map<EdgeKey, h_index> new_edge_indices;
  // old edge even index -> edge even index
  std::unordered_map<h_index, h_index> new_edge_index_map;

  for (h_index i = 0; i < boundary_vertex_count; i++) {
    h_index new_a = i;
    h_index new_b = (i + 1) % boundary_vertex_count;
    EdgeKey new_key(new_a, new_b);
    EdgeKey old_key(old_vertex_index_map[new_a], old_vertex_index_map[new_b]);
    HERMES_ASSERT(edge_key_indices_.contains(old_key));
    auto new_edge_index = new_edge_indices.size() * 2;
    new_edge_index_map[edge_key_indices_[old_key]] = new_edge_index;
    new_edge_indices[new_key] = new_edge_index;
  }

  for (const auto &item : edge_key_indices_) {
    HERMES_ASSERT(new_vertex_index_map.contains(item.first.a()));
    HERMES_ASSERT(new_vertex_index_map.contains(item.first.b()));
    EdgeKey new_key(new_vertex_index_map[item.first.a()],
                    new_vertex_index_map[item.first.b()]);

    if (!new_edge_indices.contains(new_key)) {
      auto new_edge_index = new_edge_indices.size() * 2;
      new_edge_index_map[item.second] = new_edge_index;
      new_edge_indices[new_key] = new_edge_index;
    }
  }
  HERMES_ASSERT(new_edge_indices.size() == edge_key_indices_.size());

  edge_key_indices_ = std::move(new_edge_indices);

  // compute new half-edges

  std::vector<HE2::HalfEdge> new_half_edges;
  new_half_edges.resize(half_edges_.size());

  auto newHEIndex = [&](h_index old_he) -> h_index {
    auto old_he_index = old_he - (old_he % 2);
    HERMES_ASSERT(new_edge_index_map.contains(old_he_index));
    auto new_he_index = new_edge_index_map[old_he_index];
    return new_he_index + (old_he % 2);
  };

  for (const auto &item : new_edge_index_map) {
    auto new_base_he_index = item.second;
    auto old_base_he_index = item.first;
    HERMES_ASSERT((new_base_he_index % 2) == 0);
    HERMES_ASSERT((old_base_he_index % 2) == 0);
    for (h_index i = 0; i < 2; ++i) {
      h_index old_he_index = old_base_he_index + i;
      new_half_edges[item.second + i].cell_index =
          new_cell_index_map[half_edges_[old_he_index].cell_index];
      new_half_edges[item.second + i].vertex_index =
          new_vertex_index_map[half_edges_[old_he_index].vertex_index];
      new_half_edges[item.second + i].next_he =
          newHEIndex(half_edges_[old_he_index].next_he);
      new_half_edges[item.second + i].prev_he =
          newHEIndex(half_edges_[old_he_index].prev_he);
    }
  }

  std::vector<HE2::Vertex> new_vertices(vertices_.size());
  // compute new vertices
  for (h_index i = 0; i < vertices_.size(); i++) {
    auto new_vertex_id = new_vertex_index_map[i];
    new_vertices[new_vertex_id].he_index = newHEIndex(vertices_[i].he_index);
    new_vertices[new_vertex_id].position = vertices_[i].position;
  }
  // compute new cells
  std::vector<HE2::Cell> new_cells(cells_.size());
  for (h_index i = 0; i < cells_.size(); i++) {
    auto new_cell_id = new_cell_index_map[i];
    new_cells[new_cell_id].he_index = newHEIndex(cells_[i].he_index);
    new_cells[new_cell_id].center = cells_[i].center;
  }

  vertices_ = std::move(new_vertices);
  half_edges_ = std::move(new_half_edges);
  cells_ = std::move(new_cells);

  boundary_cell_count_ = std::move(boundary_cell_count);
  boundary_vertex_count_ = std::move(boundary_vertex_count);
  boundary_edge_count_ = std::move(boundary_edge_count);
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

h_index HE2::faceHe(h_index index) const { return index * 2; }

h_index HE2::heOrigin(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  return half_edges_[he_index].vertex_index;
}

h_index HE2::heDestination(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  return half_edges_[heTwin(he_index)].vertex_index;
}

const hermes::geo::point2 &HE2::heOriginPosition(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  auto ve = half_edges_[he_index].vertex_index;
  HERMES_ASSERT(ve < vertices_.size());
  return vertices_[ve].position;
}

const hermes::geo::point2 &HE2::heDestinationPosition(h_index he_index) const {
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

std::vector<h_index> HE2::heSiblings(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  std::vector<h_index> siblings;
  h_index curr = he_index;
  do {
    siblings.emplace_back(curr);
    curr = half_edges_[heTwin(curr)].next_he;
  } while (curr != he_index);
  return siblings;
}

h_index HE2::addOrientedFace(h_index va, h_index vb) {
  HERMES_ASSERT((va < vertices_.size() && vb < vertices_.size()));
  auto key = EdgeKey(va, vb);
  auto it = edge_key_indices_.find(key);
  if (it != edge_key_indices_.end()) {
    // find out which half edge represents va->vb
    if (half_edges_[it->second].vertex_index == va)
      return it->second;
    return heTwin(it->second);
  }
  // cache face
  edge_key_indices_[key] = half_edges_.size();
  auto ab_he = half_edges_.size();
  auto ba_he = half_edges_.size() + 1;
  // left half-edge
  auto ab = (vertices_[vb].position - vertices_[va].position).normalized();
  auto va_next = nextOutgoingHE(va, ab.angleToX());
  auto va_prev = previousIncomingHE(va, ab.angleToX());
  if (va_next != null_index_) {
    half_edges_[va_next].prev_he = ba_he;
  } else
    va_next = ab_he;
  if (va_prev != null_index_) {
    half_edges_[va_prev].next_he = ab_he;
  } else
    va_prev = ba_he;
  vertices_[va].he_index = ab_he;

  // right half-edge
  auto ba = -ab;
  auto vb_next = nextOutgoingHE(vb, ba.angleToX());
  auto vb_prev = previousIncomingHE(vb, ba.angleToX());
  if (vb_next != null_index_) {
    half_edges_[vb_next].prev_he = ab_he;
  } else
    vb_next = ba_he;
  if (vb_prev != null_index_) {
    half_edges_[vb_prev].next_he = ba_he;
  } else
    vb_prev = ab_he;

  vertices_[vb].he_index = ba_he;

  half_edges_.push_back({.vertex_index = va,
                         .cell_index = null_index_,
                         .next_he = vb_next,
                         .prev_he = va_prev});
  half_edges_.push_back({.vertex_index = vb,
                         .cell_index = null_index_,
                         .next_he = va_next,
                         .prev_he = vb_prev});
  // return left half-edge
  return half_edges_.size() - 2;
}

std::vector<h_index> HE2::outgoingHEs(h_index vertex_index) const {
  HERMES_ASSERT(vertex_index < vertices_.size());
  std::vector<h_index> hes;
  auto start = vertices_[vertex_index].he_index;
  if (start == null_index_)
    return hes;
  auto curr = start;
  auto i = 0;
  do {
    if (i++ == 5)
      break;
    hes.emplace_back(curr);
    curr = heTwin(curr);
    curr = half_edges_[curr].next_he;
  } while (curr != start);
  return hes;
}

std::vector<h_index> HE2::incomingHEs(h_index vertex_index) const {
  HERMES_ASSERT(vertex_index < vertices_.size());
  std::vector<h_index> hes;
  auto start = vertices_[vertex_index].he_index;
  if (start == null_index_)
    return hes;
  start = heTwin(start);
  auto curr = start;
  auto i = 0;
  do {
    if (i++ == 5)
      break;
    hes.emplace_back(curr);
    curr = half_edges_[curr].next_he;
    curr = heTwin(curr);
  } while (curr != start);
  return hes;
}

h_index HE2::nextOutgoingHE(h_index vertex, real_t angle_to_x) const {
  auto outgoing_hes = outgoingHEs(vertex);
  if (outgoing_hes.empty())
    return null_index_;
  // we map all angles of the outgoing half-edges to [0,2pi] (as angle_to_x)
  // and rotate them so angle_to_x becomes zero. Then the next half-edge
  // will the one with the largest value.
  auto next_he = outgoing_hes.front();
  auto next_angle = hermes::math::wrapRadians(
      heVector(outgoing_hes.front()).normalized().angleToX() - angle_to_x);

  for (auto outgoing_he : outgoing_hes) {
    auto w = heVector(outgoing_he).normalized();
    auto angle = hermes::math::wrapRadians(w.angleToX() - angle_to_x);
    if (angle > next_angle) {
      next_he = outgoing_he;
      next_angle = angle;
    }
  }
  return next_he;
}

h_index HE2::previousIncomingHE(h_index vertex, real_t angle_to_x) const {
  auto incoming_hes = incomingHEs(vertex);
  if (incoming_hes.empty())
    return null_index_;
  // we map all angles of the outgoing half-edges to [0,2pi] (as angle_to_x)
  // and rotate them so angle_to_x becomes zero. Then the previous half-edge
  // will the one with the smallest value.
  auto prev_he = incoming_hes.front();
  auto prev_angle = hermes::math::wrapRadians(
      (-heVector(incoming_hes.front())).normalized().angleToX() - angle_to_x);
  for (auto incoming_he : incoming_hes) {
    auto w = -heVector(incoming_he).normalized();
    auto angle = hermes::math::wrapRadians(w.angleToX() - angle_to_x);
    if (angle < prev_angle) {
      prev_he = incoming_he;
      prev_angle = angle;
    }
  }
  return prev_he;
}

h_index HE2::computeNextHE(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  // he_index points towards heDestination(he_index), and the next h-e will be
  // outgoing it. we must consider the opposite direction, as if he_index is
  // also outgoing heDestination(he_index) so we can sort the edge fan properly.
  auto v = hermes::geo::normalize(-heVector(he_index));
  auto v_angle = std::atan2(v.y, v.x);
  if (v_angle < 0.0)
    v_angle += hermes::math::constants::two_pi;
  // we map all angles of the outgoing half-edges to [0,2pi] (as v_angle)
  // and rotate them so v_angle becomes zero. Then the next half-edge
  // will the one if greatest angle value.
  auto next_he = heTwin(he_index);
  auto next_angle = 0.0;
  for (auto outgoing_he : outgoingHEs(heDestination(he_index))) {
    auto w = hermes::geo::normalize(heVector(outgoing_he));
    // compute the rotated angle for this h-e
    auto angle = std::atan2(w.y, w.x) - v_angle;
    while (angle < 0.0)
      angle += hermes::math::constants::two_pi;
    if (angle > next_angle) {
      next_he = outgoing_he;
      next_angle = angle;
    }
  }
  return next_he;
}

h_index HE2::computePreviousHE(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  // he_index originates at heOrigin(he_index), and the previous h-e will be
  // incoming it. we must consider the opposite direction of all incoming edges
  // as if they were also outgoing heOrigin(he_index) so we can sort the edge
  // fan properly.
  auto v = hermes::geo::normalize(heVector(he_index));
  auto v_angle = std::atan2(v.y, v.x);
  if (v_angle < 0.0)
    v_angle += hermes::math::constants::two_pi;
  // we map all angles of the outgoing half-edges to [0,2pi] (as v_angle)
  // and rotate them so v_angle becomes zero. Then the previous half-edge
  // will the one if smallest angle value.
  auto twin_he = heTwin(he_index);
  auto prev_he = twin_he;
  auto prev_angle = hermes::math::constants::two_pi;
  for (auto incoming_he : incomingHEs(heOrigin(he_index))) {
    // for safety, skip the twin of the original edge, as it might give angle
    // zero and is considered as the default answer above
    if (incoming_he == twin_he)
      continue;
    // here we negate to consider w outgoing the vertex
    auto w = hermes::geo::normalize(-heVector(incoming_he));
    // compute the rotated angle for this h-e
    auto angle = std::atan2(w.y, w.x) - v_angle;
    while (angle < 0.0)
      angle += hermes::math::constants::two_pi;
    if (angle < prev_angle) {
      prev_he = incoming_he;
      prev_angle = angle;
    }
  }
  return prev_he;
}

hermes::geo::vec2 HE2::heVector(h_index he_index) const {
  HERMES_ASSERT(he_index < half_edges_.size());
  return vertices_[half_edges_[heTwin(he_index)].vertex_index].position -
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

hermes::geo::normal2 HE2::normal(const core::ElementIndex &iloc) const {
  HERMES_ASSERT(!iloc.element.is(core::element_primitive_bits::cell));
  if (iloc.element.is(core::element_primitive_bits::face)) {
    auto he_index = faceHe(iloc.index);
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

hermes::geo::point2 HE2::center(const core::ElementIndex &iloc) const {
  if (iloc.element.is(core::element_primitive_bits::cell)) {
    HERMES_ASSERT(iloc.index < cells_.size());
    return cells_[iloc.index].center;
  }
  if (iloc.element.is(core::element_primitive_bits::vertex)) {
    HERMES_ASSERT(iloc.index < vertices_.size());
    return vertices_[iloc.index].position;
  }
  if (iloc.element.is(core::element_primitive_bits::face)) {
    auto he_index = faceHe(iloc.index);
    HERMES_ASSERT(he_index < half_edges_.size());
    return (heOriginPosition(he_index) +
            hermes::geo::vec2(heDestinationPosition(he_index))) /
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
          (heOriginPosition(i) + hermes::geo::vec2(heDestinationPosition(i))) /
          2.f);
  }
  return positions;
}

h_size HE2::elementCount(core::Element loc) const {
  auto computeCount = [&](h_size total_count,
                          std::optional<h_size> boundary_count) -> h_size {
    if (loc.inAnySpace())
      return total_count;
    HERMES_ASSERT(boundary_count.has_value());
    if (loc.in(core::element_space_bits::boundary))
      return *boundary_count;
    else
      return total_count - *boundary_count;
  };
  if (loc.is(core::element_primitive_bits::cell))
    return computeCount(cells_.size(), boundary_cell_count_);
  if (loc.is(core::element_primitive_bits::vertex))
    return computeCount(vertices_.size(), boundary_vertex_count_);
  if (loc.is(core::element_primitive_bits::face))
    return computeCount(half_edges_.size() / 2, boundary_edge_count_);
  HERMES_ERROR("HE2 has only cells, vertices and faces. [{}]",
               hermes::to_string(loc));
  return 0;
}

h_size HE2::elementIndexOffset(core::Element loc) const {
  HERMES_UNUSED_VARIABLE(loc);
  return 0;
}

IndexSet HE2::indices(const core::Element &loc) const {
  auto computeIndices = [](const core::Element &loc,
                           std::optional<h_size> boundary_count,
                           h_size total_count) -> IndexSet {
    if (loc.spaces() ==
        (core::element_space_bits::any | core::element_space_bits::boundary |
         core::element_space_bits::interior)) {
      return IndexSet(IndexInterval(0, total_count));
    }
    if (!boundary_count.has_value()) {
      HERMES_NOT_IMPLEMENTED;
      return {};
    }
    if (loc.in(core::element_space_bits::boundary))
      return IndexSet(IndexInterval(0, *boundary_count));
    else
      return IndexSet(IndexInterval(*boundary_count, total_count));
  };
  if (loc.is(core::element_primitive_bits::cell)) {
    return computeIndices(loc, boundary_cell_count_, cells_.size());
  }
  if (loc.is(core::element_primitive_bits::vertex)) {
    return computeIndices(loc, boundary_vertex_count_, vertices_.size());
  }
  if (loc.is(core::element_primitive_bits::face)) {
    return computeIndices(loc, boundary_edge_count_, half_edges_.size() / 2);
  }
  HERMES_NOT_IMPLEMENTED;
  return {};
}

std::vector<h_size> HE2::elementIndices(const core::ElementIndex &iloc,
                                        core::Element sub_element) const {
  // vertices have no sub elements
  if (iloc.element == core::Element::VERTEX)
    return {};
  // faces have only vertices as sub-elements
  if (iloc.element.is(core::element_primitive_bits::face) &&
      !sub_element.is(core::element_primitive_bits::vertex))
    return {};

  std::vector<h_size> is;

  if (iloc.element.is(core::element_primitive_bits::cell)) {
    HERMES_ASSERT(*iloc.index < cells_.size());
    auto loop = heLoop(cells_[iloc.index].he_index);
    if (sub_element.is(core::element_primitive_bits::vertex)) {
      for (auto he : loop)
        is.emplace_back(heOrigin(he));
    } else if (sub_element.is(core::element_primitive_bits::face)) {
      for (auto he : loop)
        is.emplace_back(heFace(he));
    } else {
      HERMES_NOT_IMPLEMENTED;
    }
  } else if (iloc.element.is(core::element_primitive_bits::face)) {
    auto he_index = faceHe(iloc.index);
    HERMES_ASSERT(he_index < half_edges_.size());
    is.emplace_back(heOrigin(he_index));
    is.emplace_back(heDestination(he_index));
  }

  return is;
}

core::element_alignments
HE2::elementAlignment(const core::ElementIndex &iloc) const {
  HERMES_UNUSED_VARIABLE(iloc);
  return core::element_alignment_bits::any;
}

core::element_orientations
HE2::elementOrientation(const core::ElementIndex &iloc) const {
  HERMES_UNUSED_VARIABLE(iloc);
  return core::element_orientation_bits::any;
}

bool HE2::isBoundary(const core::ElementIndex &iloc) const {
  auto he = null_index_;
  if (iloc.element.is(core::element_primitive_bits::cell)) {
    HERMES_ASSERT(iloc.index < cells_.size());
    auto loop = heLoop(cells_[iloc.index].he_index);
    for (auto he : loop)
      if (half_edges_[he].cell_index == null_index_ ||
          half_edges_[heTwin(he)].cell_index == null_index_)
        return true;
    return false;
  } else if (iloc.element.is(core::element_primitive_bits::face)) {
    he = faceHe(iloc.index);
    HERMES_ASSERT(he < half_edges_.size());
  } else if (iloc.element.is(core::element_primitive_bits::vertex)) {
    HERMES_ASSERT(iloc.index < vertices_.size());
    he = vertices_[iloc.index].he_index;
  } else {
    HERMES_NOT_IMPLEMENTED;
    return false;
  }

  HERMES_ASSERT(he != null_index_);
  if (half_edges_[he].cell_index == null_index_ ||
      half_edges_[heTwin(he)].cell_index == null_index_)
    return true;
  return false;
}

h_size HE2::interiorNeighbour(const core::ElementIndex &boundary_element,
                              const core::Element &interior_loc) const {
  if (boundary_element.element.is(core::element_primitive_bits::face)) {
    if (interior_loc.is(core::element_primitive_bits::cell)) {
      auto he = faceHe(*boundary_element.index);
      HERMES_ASSERT(he < half_edges_.size());
      if (half_edges_[he].cell_index != null_index_)
        return half_edges_[he].cell_index;
      HERMES_ASSERT(half_edges_[heTwin(he)].cell_index != null_index_);
      return half_edges_[heTwin(he)].cell_index;
    } else {
      HERMES_NOT_IMPLEMENTED;
    }
  } else {
    HERMES_NOT_IMPLEMENTED;
  }
  return 0;
}

std::vector<core::ElementIndex>
HE2::neighbours(const core::ElementIndex &c_loc,
                std::optional<core::Element> b_loc) const {
  std::vector<core::ElementIndex> nids;
  if (c_loc.element.is(core::element_primitive_bits::cell)) {
    HERMES_ASSERT(c_loc.index < cells_.size());
    HERMES_ASSERT(cells_[c_loc.index].he_index < half_edges_.size());
    for (auto he : heLoop(cells_[c_loc.index].he_index)) {
      auto he_t = heTwin(he);
      auto n_cell = half_edges_[he_t].cell_index;
      if (n_cell != null_index_) {
        nids.emplace_back(core::ElementIndex::global(c_loc.element, n_cell));
      } else if (b_loc) {
        HERMES_ASSERT(*b_loc == core::Element::FACE);
        // the boundary is represented by the center of the face
        auto n_face = heFace(he_t);
        nids.emplace_back(core::ElementIndex::global(*b_loc, n_face));
      }
    }
  } else if (c_loc.element.is(core::element_primitive_bits::vertex)) {
    HERMES_ASSERT(c_loc.index < vertices_.size());
    HERMES_ASSERT(*c_loc.index < vertices_.size());
    for (auto he : heSiblings(vertices_[*c_loc.index].he_index)) {
      auto nid = heDestination(he);
      nids.emplace_back(core::ElementIndex::global(c_loc.element, nid));
    }
  } else {
    HERMES_NOT_IMPLEMENTED;
  }
  return nids;
}

} // namespace naiades::geo
