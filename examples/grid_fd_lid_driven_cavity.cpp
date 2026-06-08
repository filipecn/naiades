/// This example solves the Laplace equation
///     du   du
///   ( -- + -- ) = 0
///     dx   dy
///
/// with u = 1 at y = 1
///      u = 0 otherwise

#include <naiades/geo/grid.h>
#include <naiades/numeric/boundary_conditions.h>
#include <naiades/numeric/linear_solvers.h>
#include <naiades/utils/fields.h>
#include <naiades/utils/io.h>

namespace na = naiades;

int main() {
  // create numerical mesh (a 2d grid) with finite differences discretization
  auto fd = *na::numeric::Grid2FD::Config()
                 .setDomain(hermes::geo::bounds::bbox2::unit())
                 .setResolution({40, 40})
                 .build();

  // define symbols for the equation
  auto u = na::core::DiscreteSymbol::cell("u");
  // create fields for the symbols
  fd.addFields<f32>({u.symbol});

  // define a single boundary containting all faces
  // region 0 -> lid faces
  // regions 1,2,3 -> rest
  fd.addBoundary(u.boundary_symbol,
                 fd.mesh().boundaryIndices(na::core::Element::UP_FACE));
  fd.addBoundary(u.boundary_symbol,
                 fd.mesh().boundaryIndices(na::core::Element::LEFT_FACE));
  fd.addBoundary(u.boundary_symbol,
                 fd.mesh().boundaryIndices(na::core::Element::DOWN_FACE));
  fd.addBoundary(u.boundary_symbol,
                 fd.mesh().boundaryIndices(na::core::Element::RIGHT_FACE));

  // set Dirichlet boundary condition at the boundary
  auto lid_bc = na::numeric::bc::Dirichlet::Ptr::shared(1);
  fd.setBoundaryCondition(u.boundary_symbol, 0, lid_bc);
  auto walls_bc = na::numeric::bc::Dirichlet::Ptr::shared(0);
  fd.setBoundaryCondition(u.boundary_symbol, 1, walls_bc);
  fd.setBoundaryCondition(u.boundary_symbol, 2, walls_bc);
  fd.setBoundaryCondition(u.boundary_symbol, 3, walls_bc);

  // resolve boundary stencils
  fd.resolveBoundaries();

  auto u_field = *fd.getField<f32>(u.symbol);

  na::numeric::solvers::CG()
      .setUnknown(u) //
      .build(fd.L(u), 0)
      .solve(u_field);

  // compute rmse error

  auto x = fd.mesh().x(na::core::Element::cell());
  auto y = fd.mesh().y(na::core::Element::cell());

  h_index n = 100;
  real_t npi = hermes::math::constants::pi * n;
  real_t k = 4.0 / (npi * std::sinh(npi));
  auto sol = na::numeric::Scalar::zero(x.size());
  for (h_index i = 0; i < n; ++i)
    sol += k * na::numeric::sin(npi * x) * na::numeric::sinh(npi * y);

  // take the difference
  auto diff = na::numeric::abs(u_field - sol);

  f32 rmse = std::sqrt(na::numeric::sum(na::numeric::sqr(diff))) / sol.size();

  HERMES_LOG_VARIABLE(rmse);

  na::utils::io::SVG("grid.svg")
      .setDimensions(fd.mesh().bbounds())
      //   .draw(fd.mesh(), na::core::Element::cell(), sol)
      //   .draw(fd.mesh())
      .draw(fd.mesh(), static_cast<na::core::FieldCRef<f32>>(u_field))
      // .drawText(fd.mesh(), na::core::Element::cell(), x)
      // .draw(fd.mesh(), fd.boundaries())
      .write();

  return 0;
}
