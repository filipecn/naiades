/// This example solves the Poisson equation
///     du   du
/// - ( -- + -- ) = f
///     dx   dy
///
/// with source term f(x,y)=2\pi^2\sin(\pi x)\sin(\pi y)
/// and zero Dirichlet boundary conditions.

#include <naiades/geo/grid.h>
#include <naiades/geo/utils.h>
#include <naiades/numeric/boundary_conditions.h>
#include <naiades/numeric/linear_solvers.h>
#include <naiades/numeric/rbf.h>
#include <naiades/utils/fields.h>
#include <naiades/utils/io.h>

namespace na = naiades;

int main() {
  auto grid = *na::geo::Grid2::Config()
                   .setCellSize(0.01f)
                   .setResolution({2, 2})
                   .build();
  auto mesh = na::geo::HE2::Ptr::shared();
  *mesh = *na::geo::convert2HE(grid);

  auto rbf_fd = *na::numeric::HE2RBFFD::Config().build(mesh);

  // define symbols for the equation
  auto f = na::core::DiscreteSymbol::vertex("f");
  auto u = na::core::DiscreteSymbol::vertex("u");
  // create fields for the symbols
  rbf_fd.addFields<f32>({u.symbol, f.symbol});

  // define a single boundary containing all faces
  rbf_fd.addBoundary(u.boundary_symbol,
                     rbf_fd.mesh().boundaryIndices(u.boundary_symbol.loc));

  // set Dirichlet boundary condition at the boundary
  auto dirichlet = na::numeric::bc::Dirichlet::Ptr::shared(0);
  rbf_fd.setBoundaryCondition(u.boundary_symbol, 0, dirichlet);

  // resolve boundary stencils
  rbf_fd.resolveBoundaries();

  // get mesh position fields for the equations
  auto x = rbf_fd.mesh().x(na::core::Element::vertex());
  auto y = rbf_fd.mesh().y(na::core::Element::vertex());

  // set source term
  auto f_field = *rbf_fd.getField<f32>(f.symbol);
  f_field = 2.f * hermes::math::constants::pi * hermes::math::constants::pi *
            na::numeric::sin(hermes::math::constants::pi * x) *
            na::numeric::sin(hermes::math::constants::pi * y);

  // auto u_field = *rbf_fd.getField<f32>(u.symbol);
  // na::numeric::solvers::CG()
  //     .setUnknown(u) //
  //     .build(-fd.L(u), f)
  //     .solve(u_field, {f_field});

  // compute rmse error

  auto sol = na::numeric::sin(hermes::math::constants::pi * x) *
             na::numeric::sin(hermes::math::constants::pi * y);

  // take the difference
  // auto diff = na::numeric::abs(u_field - sol);

  // f32 rmse = std::sqrt(na::numeric::sum(na::numeric::sqr(diff))) /
  // sol.size();

  // HERMES_LOG_VARIABLE(rmse);

  auto star = rbf_fd.mesh().star(
      {na::core::Element::vertex(), na::core::Index::global(4)},
      na::core::Element::vertex());

  auto stencil = na::numeric::Stencil2::build(mesh, star);
  auto kernel = na::numeric::rbf::CubicKernel();
  auto A = na::numeric::DifferentialRBF2::computeA(
      stencil, &kernel, na::numeric::PolynomialType::ZERO);
  if (!A) {
    return -1;
  }
  std::cout << *A << std::endl;
  auto solver = na::numeric::DifferentialRBF2::buildSolver(*A);
  if (!solver) {
    return -1;
  }
  auto dop = na::numeric::DifferentialRBF2::derivative(
      na::numeric::derivative_bits::x, *solver, stencil, &kernel,
      na::numeric::PolynomialType::ZERO);
  if (!dop) {
    return -1;
  }
  HERMES_LOG_VARIABLE(*dop);

  HERMES_LOG_VARIABLE((*dop)(f_field));

  for (const auto &v : star) {
    HERMES_LOG_VARIABLE(v);
  }

  HERMES_LOG_VARIABLE(rbf_fd);
  na::utils::io::SVG()
      .disable(
          // na::utils::io::draw_option_bits::indices |
          na::utils::io::draw_option_bits::normals |
          na::utils::io::draw_option_bits::faces |
          na::utils::io::draw_option_bits::cells |
          na::utils::io::draw_option_bits::vertices)
      //.draw(rbf_fd.mesh(), na::core::Element::cell(), sol)
      .draw(rbf_fd.mesh())
      .draw(rbf_fd.mesh(), rbf_fd.mesh().star({na::core::Element::vertex(),
                                               na::core::Index::global(4)},
                                              na::core::Element::vertex()))
      // .draw(rbf_fd.mesh(), static_cast<na::core::FieldCRef<f32>>(f_field))
      // .drawText(fd.mesh(), na::core::Element::cell(), x)
      // .draw(fd.mesh(), fd.boundaries())
      .write("rbf_grid.svg");

  return 0;
}
