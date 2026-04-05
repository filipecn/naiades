/// This example solves the Poisson equation
///     du   du
/// - ( -- + -- ) = f
///     dx   dy
///
/// with source term f(x,y)=2\pi^2\sin(\pi x)\sin(\pi y)
/// and zero Dirichlet boundary conditions.

#include <naiades/geo/grid.h>
#include <naiades/numeric/boundary_conditions.h>
#include <naiades/numeric/linear_solvers.h>
#include <naiades/utils/fields.h>
#include <naiades/utils/io.h>

namespace na = naiades;

int main() {
  // create numerical mesh from the geometry
  auto fd = *na::numeric::Grid2FD::Config()
                 .setDomain(hermes::geo::bounds::bbox2::unit())
                 .setResolution({50, 50})
                 .build();

  // define symbols for the equation
  auto f = na::core::DiscreteSymbol::cell("f");
  auto u = na::core::DiscreteSymbol::cell("u");
  // create fields for the symbols
  fd.addFields<f32>({u.symbol, f.symbol});

  // define a single boundary containting all faces
  fd.addBoundary(u.boundary_symbol,
                 fd.mesh().boundaryIndices(u.boundary_symbol.loc));

  // set Dirichlet boundary condition at the boundary
  auto dirichlet = na::numeric::bc::Dirichlet::Ptr::shared(0);
  fd.setBoundaryCondition(u.boundary_symbol, 0, dirichlet);

  // get mesh position fields for the equations
  auto x = fd.mesh().x(na::core::Element::cell());
  auto y = fd.mesh().y(na::core::Element::cell());

  // set source term
  auto f_field = *fd.getField<f32>(f.symbol);
  f_field = 2.f * hermes::math::constants::pi * hermes::math::constants::pi *
            na::numeric::sin(hermes::math::constants::pi * x) *
            na::numeric::sin(hermes::math::constants::pi * y);

  // resolve boundary stencils
  fd.resolveBoundaries();

  auto u_field = *fd.getField<f32>(u.symbol);

  na::numeric::solvers::CG()
      .setUnknown(u, &u_field) //
      .addVariable(&f_field)   //
      .solve(-fd.L(u), f);

  // compute rmse error

  auto sol = na::numeric::sin(hermes::math::constants::pi * x) *
             na::numeric::sin(hermes::math::constants::pi * y);

  // take the difference
  auto diff = na::numeric::abs(u_field - sol);

  f32 rmse = std::sqrt(na::numeric::sum(na::numeric::sqr(diff))) / sol.size();

  HERMES_LOG_VARIABLE(rmse);

  na::utils::io::SVG("grid.svg")
      .setDimensions(fd.mesh().bbounds())
      // .draw(fd.mesh())
      .draw(fd.mesh(), static_cast<na::core::FieldCRef<f32>>(u_field))
      // .draw(fd.mesh(), na::core::Element::cell(), diff)
      // .drawText(fd.mesh(), na::core::Element::cell(), x)
      // .draw(fd.mesh(), fd.boundaries())
      .write();

  return 0;
}
