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

/// \file   rbf_fd.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-07-25
/// \brief  .

#include <naiades/core/neighbourhood.h>
#include <naiades/geo/he.h>
#include <naiades/numeric/rbf.h>
#include <naiades/numeric/spatial_discretization.h>
#include <naiades/utils/io.h>

namespace naiades::numeric {

template <typename MeshType, typename KernelFunction> class RBFFD2 {
public:
  using Ptr = hermes::Ref<RBFFD2<MeshType, KernelFunction>>;

  RBFFD2<MeshType, KernelFunction> &
  setCriteria(core::NeighbourhoodCriteria criteria) {
    neighbourhood_criteria_ = criteria;
    return *this;
  }

  RBFFD2<MeshType, KernelFunction> &
  setPolynomialType(PolynomialType polynomial_type) {
    polynomial_type_ = polynomial_type;
    return *this;
  }

  /// Compute the derivative operator centered at the given element.
  /// \param d Derivative direction.
  /// \param index
  /// \param sym
  DiscreteOperator derivative(const MeshType::Ptr &mesh,
                              const Boundary &boundary, derivative_bits d,
                              h_size index,
                              const core::DiscreteSymbol &sym) const {

    core::Neighbourhood<MeshType> neighbourhood(mesh);
    // auto neighbours =
    //     neighbourhood.star(core::ElementIndex::global(sym.symbol.loc, index),
    //                        sym.boundary_symbol.loc);
    auto neighbours =
        neighbourhood.knn(5, core::ElementIndex::global(sym.symbol.loc, index),
                          sym.boundary_symbol.loc);

    Stencil2 stencil = Stencil2::build(mesh, neighbours);
    auto A = DifferentialRBF2::computeA(stencil, &rbf_, polynomial_type_);
    HERMES_ASSERT(A);
    auto solver = DifferentialRBF2::buildSolver(*A);
    HERMES_ASSERT(solver);
    auto op = DifferentialRBF2::derivative(d, *solver, stencil, &rbf_);
    op->setCenterIndex(index);

    HERMES_LOG_VARIABLE_IF(index == 30, *op);

    NAIADES_PLOT_IF(index == 30,
                    disable(naiades::utils::io::draw_option_bits::normals |
                            naiades::utils::io::draw_option_bits::faces |
                            naiades::utils::io::draw_option_bits::cells
                            //  naiades::utils::io::draw_option_bits::vertices
                            )
                        .setPointSize(0.001)
                        .draw(mesh.get())
                        .draw(stencil));

    //                               DiscreteOperator op(index);
    // for (const auto &n : neighbours) {
    //   if (n.element_index.element != sym.symbol.loc)
    //     op += boundary.stencil(n.element_index.index) * n.distance;
    //   else
    //     op.add(*n.element_index.index, n.distance);
    // }

    return *op;
  }
  /// Compute the discrete Laplacian operator centered at the given element.
  /// \param index
  /// \param sym
  DiscreteOperator laplacian(const MeshType::Ptr &mesh,
                             const Boundary &boundary, h_size index,
                             const core::DiscreteSymbol &sym) const {
    DiscreteOperator op(index);
    op += derivative(mesh, boundary, derivative_bits::x, index, sym);
    op += derivative(mesh, boundary, derivative_bits::y, index, sym);
    return op;
  }
  /// Compute the discrete Laplacian operator centered at the given element.
  /// Compute the discrete Divergence operator centered at the given element.
  /// \tparam DiscretizationType Discretization type.
  /// \param boundary
  /// \param loc
  /// \param index
  /// \param staggered
  DiscreteOperator divergence(const MeshType::Ptr &mesh,
                              const Boundary &boundary,
                              const core::Element &loc, h_size index,
                              const core::Element &vector_loc,
                              bool staggered) const {
    HERMES_UNUSED_VARIABLE(mesh);
    HERMES_UNUSED_VARIABLE(boundary);
    HERMES_UNUSED_VARIABLE(loc);
    HERMES_UNUSED_VARIABLE(index);
    HERMES_UNUSED_VARIABLE(vector_loc);
    HERMES_UNUSED_VARIABLE(staggered);
    DiscreteOperator op;
    return op;
  }

private:
  KernelFunction rbf_;
  PolynomialType polynomial_type_;
  core::NeighbourhoodCriteria neighbourhood_criteria_;

#ifdef NAIADES_INCLUDE_DEBUG_TRAIT
  friend struct hermes::DebugTraits<RBFFD2>;
#endif
};

template <typename KernelFunction>
using HE2RBFFD =
    SpatialDiscretization<geo::HE2, RBFFD2<geo::HE2, KernelFunction>>;

} // namespace naiades::numeric

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS

namespace hermes {

template <typename StructureType, typename KernelFunction>
struct DebugTraits<naiades::numeric::RBFFD2<StructureType, KernelFunction>> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage
  message(const naiades::numeric::RBFFD2<StructureType, KernelFunction> &data) {
    auto m = DebugMessage();
    m.addTitle("RBF - FD");
    m.addMap("fields", data.fields_);
    m.addMap("boundaries", data.boundaries_);
    return m;
  }
};

} // namespace hermes

#endif
