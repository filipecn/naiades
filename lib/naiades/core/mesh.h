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

/// \file   mesh.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-03-28

#include <naiades/core/geometry.h>
#include <naiades/core/topology.h>
#include <naiades/numeric/blas.h>

#pragma once

namespace naiades::core {

/// \brief Interface for 2-dimensional numeric discretization meshes.
/// A discretization mesh holds geometric and topological information that fully
/// represents a spatial discretization structure.
class Mesh2 : public Topology, public Geometry2 {
public:
  using Ptr = hermes::Ref<Mesh2>;

  class element_view;
  ///
  class iterator {
  public:
    struct ElementInstance {
      hermes::geo::point2 center;
      h_index local_index;
      h_index global_index;
    };

    ElementInstance operator*() const;

    iterator &operator++();
    bool operator==(const iterator &rhs) const;

  private:
    friend class element_view;
    iterator(const Mesh2 *mesh, const Element &loc, h_index index);

    const Mesh2 *mesh_;
    Element loc_;
    h_index local_index_;
  };

  class element_view {
  public:
    iterator begin() const;
    iterator end() const;

  private:
    friend class Mesh2;
    element_view(const Mesh2 *mesh, const Element &loc);

    const Mesh2 *mesh_;
    Element loc_;
  };

  /// \param loc Element filter.
  /// \return View for iterating over instances of the given element.
  element_view elements(const Element &loc) const;

  /// \return center's x coordinate.
  numeric::Scalar x(const Element &loc) const;
  /// \return center's y coordinate.
  numeric::Scalar y(const Element &loc) const;
};

} // namespace naiades::core