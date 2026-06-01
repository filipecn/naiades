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

/// \file   he.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-04-08

#pragma once

#include <naiades/core/mesh.h>
#include <naiades/numeric/spatial_discretization.h>

#include <hermes/geometry/vector.h>
#include <hermes/numeric/numeric.h>

namespace naiades::geo {
/// \class HE2
/// \brief 2-dimensional Half-Edge structure mesh
///
/// The Half-Edge data structure (also known as a Doubly Connected Edge
/// List) is an edge-centered topological representation for polygonal meshes.
/// It decomposes every edge into two directed "half-edges" with opposite
/// orientations, which serves as the "glue" to connect mesh entities (vertices,
/// edges, and faces).
///
/// This represents a manifold with oriented elements so the internal half-edges
/// of a cell follow the right hand rule:
///             --->
///     0 ----------------   2
///       \     <---     /                This way, boundary faces will have
///        \   |    ^   /                 normals pointing to the "left" of
///         \  V --->  /                  the external half-edge.
///      ^   \        /    |
///      |    \      /     V              It means that cells must be created
///            \    /                     with their vertices oriented.
///             \  /
///              \/  1
class HE2 : public core::Mesh2 {
public:
  HE2() noexcept;
  virtual ~HE2() = default;

  /// \param position Vertex position.
  /// \return The index of the newly added vertex.
  h_index addVertex(const hermes::geo::point2 &position);
  /// \brief
  /// \param vertex_indices
  /// \return The index of the newly created cell.
  /// \note This creates the faces for the new cell if necessary.
  h_index addCell(const std::vector<h_index> &oriented_vertex_indices);

  /// \return The twin half-edge index.
  h_index heTwin(h_index he_index) const;
  h_index heFace(h_index he_index) const;
  h_index heCell(h_index he_index) const;
  h_index heNext(h_index he_index) const;
  h_index hePrev(h_index he_index) const;
  h_index faceHe(h_index face_index) const;
  hermes::geo::normal2 heNormal(h_index he_index) const;
  /// \return Edge vector of the given half-edge.
  hermes::geo::vec2 heVector(h_index he_index) const;
  /// \return Vertex at the end of the given half-edge.
  h_index heEnd(h_index he_index) const;
  /// \return Vertex at the end of the given half-edge.
  h_index heStart(h_index he_index) const;
  const hermes::geo::point2 &heStartPosition(h_index he_index) const;
  const hermes::geo::point2 &heEndPosition(h_index he_index) const;
  ///
  std::vector<h_index> heLoop(h_index he_index) const;

  //  geometry interface

  hermes::geo::bounds::bbox2 bbounds() const override;
  hermes::geo::normal2 normal(core::Element loc, h_index index) const override;
  hermes::geo::point2 center(core::Element loc,
                             h_size flat_index) const override;
  std::vector<hermes::geo::point2> centers(core::Element loc) const override;

  //  topology interface

  /// Grid location counts
  h_size elementCount(core::Element loc) const override;
  /// Grid flat index offset.
  /// \note The flat index offset is zero for all elements, except for faces.
  h_size elementIndexOffset(core::Element loc) const override;
  /// \brief Get the list of indices of a given element instance.
  /// \param element
  /// \param index
  /// \param sub_element
  /// \return The lists of sub-elements of the given element instance.
  std::vector<h_size> indices(core::Element element, h_index index,
                              core::Element sub_element) const override;
  std::vector<h_size> boundaryIndices(core::Element loc) const override;
  core::element_alignments elementAlignment(core::Element loc,
                                            h_size index) const override;
  core::element_orientations elementOrientation(core::Element loc,
                                                h_size index) const override;
  bool isBoundary(core::Element loc, h_size index) const override;
  h_size interiorNeighbour(const core::ElementIndex &boundary_element,
                           const core::Element &interior_loc) const override;

  // neighbourhood set interface

  /// The star neighbourhood of a given element.
  /// \param loc Element type.
  /// \param index Center index.
  /// \param boundary_loc Boundary elements included in the star.
  /// \return List of neighbours of the given element.
  std::vector<core::Neighbour> star(core::Element loc, h_size index,
                                    core::Element boundary_loc) const override;
  /// The ring neighbourhood of a given element.
  /// \param loc Element type.
  /// \param index Center index.
  /// \param boundary_loc Boundary elements included in the ring.
  /// \return List of neighbours of the given element.
  std::vector<Neighbour> ring(core::Element loc, h_size index,
                              core::Element boundary_loc) const override;
  /// The direct neighbourhood of elements for a given element.
  /// \param loc Element type.
  /// \param index Center index.
  /// \param neighbour_loc neighbour element type.
  /// \return List of pairs neighbour <index, distance> of the given element.
  std::vector<std::pair<h_size, real_t>>
  neighbours(core::Element loc, h_size index,
             core::Element neighbour_loc) const override;

private:
  /// \brief Add a oriented face.
  /// A oriented face is an edge whose (va,vb) order is found in the cell it
  /// belongs to, i.e., the "internal half-edge" is on the left side if
  /// following the same order.
  /// \return The index of the internal half-edge relative to the oriented face.
  /// \note The internal half-edge relative to the oriented face is the
  ///       half-edge that is "located" to the left of the face.
  h_index addOrientedFace(h_index va, h_index vb);
  /// \return List of half-edges leaving the given vertex.
  std::vector<h_index> outgoingHEs(h_index vertex_index) const;
  /// \return List of half-edges arriving at the given vertex.
  std::vector<h_index> incomingHEs(h_index vertex_index) const;
  /// Compute the next half-edge index for the given half-edge.
  /// \note This finds the next half-edge even if there is a index stored
  ///       for the next half-edge already stored in the given half-edge struct.
  h_index computeNextHE(h_index he_index) const;
  /// Compute the previous half-edge index for the given half-edge.
  /// \note This finds the previous half-edge even if there is a index stored
  ///       for the previous half-edge already stored in the given half-edge
  ///       struct.
  h_index computePreviousHE(h_index he_index) const;
  /// Find would be the next outgoing half-edge in the sequence for an incoming
  /// angle.
  /// \param angle [0,2pi] angle to abscissa
  h_index nextOutgoingHE(h_index vertex, real_t angle_to_x) const;
  /// Find would be the previous incoming half-edge in the sequence for an
  /// outgoing angle.
  /// \param angle [0,2pi] angle to abscissa
  h_index previousIncomingHE(h_index vertex, real_t angle_to_x) const;

  // Since half-edges exist in pairs and are created together whenever a new
  // full-edge is created, we convensionate that the pair of half-edges is
  // always a pair of even-odd indices. Therefore the index of the twin can
  // always be computed based on the index.
  // This implies that the index of a full-edge is given by the integer
  // division of the half edge index by 2.

  struct HalfEdge {
    h_index vertex_index;
    h_index cell_index;
    h_index next_he;
    h_index prev_he;
  };
  struct Vertex {
    hermes::geo::point2 position;
    h_index he_index;
  };
  struct Cell {
    hermes::geo::point2 center;
    h_index he_index;
  };

  static h_index null_index_;

  std::vector<HalfEdge> half_edges_;
  std::vector<Vertex> vertices_;
  std::vector<Cell> cells_;
  h_index boundary_start_he_;

  struct EdgeKey {
    EdgeKey() = default;
    EdgeKey(h_index a, h_index b) noexcept {
      a_ = std::min(a, b);
      b_ = std::max(a, b);
    }

    bool operator==(const EdgeKey &rhs) const {
      return a_ == rhs.a_ && b_ == rhs.b_;
    }

    std::size_t operator()(const EdgeKey &key) const {
      return key.a_ ^ (key.b_ << 1);
    }

  private:
    h_index a_{0};
    h_index b_{0};
  };
  // (min(va, vb), max(va, vb)) -> half_edges even index
  std::unordered_map<EdgeKey, h_index, EdgeKey>
      edge_vertices_to_edge_index_map_;
  h_index boundary_start_;

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS
  friend struct hermes::DebugTraits<HE2>;
#endif
};

} // namespace naiades::geo

namespace naiades::numeric {

class HE2RBFFD : public SpatialDiscretization {

  /// \brief
  const geo::HE2 &mesh() const;

  /// Compute the derivative operator centered at the given element.
  /// \param d Derivative direction.
  /// \param index
  /// \param sym
  virtual DiscreteOperator
  derivative(derivative_bits d, h_size index,
             const core::DiscreteSymbol &sym) const override;
  /// Compute the discrete Laplacian operator centered at the given element.
  /// \param index
  /// \param sym
  virtual DiscreteOperator
  laplacian(h_size index, const core::DiscreteSymbol &sym) const override;
  /// Compute the discrete Laplacian operator centered at the given element.
  /// Compute the discrete Divergence operator centered at the given element.
  /// \tparam DiscretizationType Discretization type.
  /// \param boundary
  /// \param loc
  /// \param index
  /// \param staggered
  virtual DiscreteOperator divergence(const core::Element &loc, h_size index,
                                      const core::Element &vector_loc,
                                      bool staggered) const override;
};

} // namespace naiades::numeric

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS

namespace hermes {

template <> struct DebugTraits<naiades::geo::HE2> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::geo::HE2 &data) {
    auto m = DebugMessage();
    m.add("bounds", data.bbounds());
    m.add("vertex count", data.elementCount(naiades::core::Element::vertex()));
    m.add("face count", data.elementCount(naiades::core::Element::face()));
    m.add("cell count", data.elementCount(naiades::core::Element::cell()));

    m.addFmt("Half-Edges");
    for (auto face : data.elements(naiades::core::Element::face())) {
      auto he = face.global_index * 2;
      auto het = data.heTwin(he);
      m.addFmt("face[{}]: he[{}]({}, {})|{},{}| he[{}]({},{})|{},{}|",
               face.global_index,                                  //
               he, data.heStart(he), data.heEnd(he),               //
               data.computePreviousHE(he), data.computeNextHE(he), //
               het, data.heStart(het), data.heEnd(het),            //
               data.computePreviousHE(het), data.computeNextHE(het));
    }
    m.addFmt("Vertices");
    for (auto vertex : data.elements(naiades::core::Element::vertex())) {
      m.addFmt("vertex[{}]({}): he[{}] in[{}] out[{}]", vertex.global_index,
               hermes::to_string(vertex.center),
               data.vertices_[vertex.global_index].he_index,
               hermes::cstr::join(data.incomingHEs(vertex.global_index), ","),
               hermes::cstr::join(data.outgoingHEs(vertex.global_index), ","));
    }

    return m;
  }
};

} // namespace hermes

#endif