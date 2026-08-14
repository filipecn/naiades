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

/// \file   fd.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-07-31
/// \brief  Finite Differences.

#pragma once

#include <naiades/geo/grid.h>
#include <naiades/numeric/boundary.h>
#include <naiades/numeric/discrete_operator.h>
#include <naiades/numeric/spatial_discretization.h>

namespace naiades::numeric {

template <typename GridType> class FD2 {
public:
  using Ptr = hermes::Ref<FD2<GridType>>;

  /// Compute the derivative operator centered at the given element.
  /// \param d Derivative direction.
  /// \param index
  /// \param sym
  DiscreteOperator derivative(const GridType::Ptr &grid,
                              const Boundary &boundary, derivative_bits d,
                              h_size index,
                              const core::DiscreteSymbol &sym) const {
    DiscreteOperator op(index);

    auto addNeighbour = [&](const core::Neighbour &n, real_t k) {
      if (n.element_index.element != sym.symbol.loc)
        op += boundary.stencil(n.element_index.index) * k;
      else
        op.add(*n.element_index.index, k);
    };
    auto ij = grid->index(core::ElementIndex::global(sym.symbol.loc, index));
    if (d == derivative_bits::x) {
      auto left = grid->neighbour(sym.symbol.loc, ij,
                                  core::element_orientation_bits::left,
                                  sym.boundary_symbol.loc);
      auto right = grid->neighbour(sym.symbol.loc, ij,
                                   core::element_orientation_bits::right,
                                   sym.boundary_symbol.loc);
      // TODO assuming ghost point, so boundary distance is hx
      // TODO if we want to allow non-uniform distances we need to consider hx/2
      auto hx = grid->cellSize().x;
      auto k = 1 / (hx * hx);
      addNeighbour(left, k);
      addNeighbour(right, k);
      op.add(index, -2 * k);
    } else if (d == derivative_bits::y) {
      auto down = grid->neighbour(sym.symbol.loc, ij,
                                  core::element_orientation_bits::down,
                                  sym.boundary_symbol.loc);
      auto up = grid->neighbour(sym.symbol.loc, ij,
                                core::element_orientation_bits::up,
                                sym.boundary_symbol.loc);
      // TODO assuming ghost point, so boundary distance is hy
      // TODO if we want to allow non-uniform distances we need to consider hy/2
      auto hy = grid->cellSize().y;
      auto k = 1 / (hy * hy);
      addNeighbour(down, k);
      addNeighbour(up, k);
      op.add(index, -2 * k);
    } else {
      // err
    }
    return op;
  }
  /// Compute the discrete Laplacian operator centered at the given element.
  /// \param index
  /// \param sym
  DiscreteOperator laplacian(const GridType::Ptr &grid,
                             const Boundary &boundary, h_size index,
                             const core::DiscreteSymbol &sym) const {
    DiscreteOperator op(index);
    op += derivative(grid, boundary, derivative_bits::x, index, sym);
    op += derivative(grid, boundary, derivative_bits::y, index, sym);
    return op;
  }
  /// Compute the discrete Divergence operator centered at the given element.
  /// \tparam DiscretizationType Discretization type.
  /// \param boundary
  /// \param loc
  /// \param index
  DiscreteOperator divergence(const GridType::Ptr &grid,
                              const Boundary &boundary,
                              const core::Element &loc, h_size index,
                              const core::Element &vector_loc,
                              bool staggered) const {
    //   HERMES_ASSERT(u.element() == Element::Type::Y_FACE);
    //   HERMES_ASSERT(v.element() == Element::Type::X_FACE);
    //   HERMES_ASSERT(f.element() == Element::Type::CELL);
    //
    // #d efine AT(F, IJ) \
    //  F[grid.flatIndex(F.element(), IJ) -
    //  grid.elementIndexOffset(F.element())]
    //
    //   const auto d = grid.cellSize();
    //   for (auto ij : hermes::range2(grid.resolution(f.element()))) {
    //     AT(f, ij) = -0.5 * (d.y * (AT(v, ij.up()) - AT(v, ij)) +
    //                         d.x * (AT(u, ij.right()) - AT(u, ij)));
    //   }
    // #undef AT
    DiscreteOperator op;
    return op;
  }
};

using Grid2FD = SpatialDiscretization<geo::Grid2, FD2<geo::Grid2>>;

} // namespace naiades::numeric
