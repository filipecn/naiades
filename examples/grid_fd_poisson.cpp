/// This example solves the Poisson equation
///     du   du
/// - ( -- + -- ) = f
///     dx   dy
///
/// with source term f(x,y)=2\pi^2\sin(\pi x)\sin(\pi y)
/// and zero Dirichlet boundary conditions.

#include <naiades/geo/grid.h>
#include <naiades/numeric/boundary_conditions.h>
#include <naiades/numeric/fd.h>
#include <naiades/numeric/linear_solvers.h>
#include <naiades/utils/fields.h>
#include <naiades/utils/io.h>

namespace na = naiades;

int main() {
  // create numerical mesh (a 2d grid) with finite differences discretization
  auto grid = *na::geo::Grid2::Config()
                   .setDomain(hermes::geo::bounds::bbox2::unit())
                   .setResolution({40, 40})
                   .buildPtr();
  na::numeric::Grid2FD fd(grid);

  // define symbols for the equation
  auto f = na::core::DiscreteSymbol::cell("f");
  auto u = na::core::DiscreteSymbol::cell("u");
  // create fields for the symbols
  fd.addFields<f32>({u.symbol, f.symbol});

  // define a single boundary containting all faces
  fd.addBoundary(u.boundary_symbol, grid->indices(u.boundary_symbol.loc));
  //  grid->boundaryIndices(u.boundary_symbol.loc));

  // set Dirichlet boundary condition at the boundary
  auto dirichlet = na::numeric::bc::Dirichlet::Ptr::shared(0);
  fd.setBoundaryCondition(u.boundary_symbol, 0, dirichlet);

  // resolve boundary stencils
  fd.resolveBoundaries();

  // get mesh position fields for the equations
  auto x = grid->x(na::core::Element::cell());
  auto y = grid->y(na::core::Element::cell());

  // set source term
  auto f_field = *fd.getField<f32>(f.symbol);
  f_field = 2.f * hermes::math::constants::pi * hermes::math::constants::pi *
            na::numeric::sin(hermes::math::constants::pi * x) *
            na::numeric::sin(hermes::math::constants::pi * y);

  auto u_field = *fd.getField<f32>(u.symbol);

  na::numeric::solvers::CG()
      .setUnknown(u) //
      .build(-fd.L(u), f)
      .solve(u_field, {f_field});

  // compute rmse error

  auto sol = na::numeric::sin(hermes::math::constants::pi * x) *
             na::numeric::sin(hermes::math::constants::pi * y);

  // take the difference
  auto diff = na::numeric::abs(u_field - sol);

  f32 rmse = std::sqrt(na::numeric::sum(na::numeric::sqr(diff))) / sol.size();

  HERMES_LOG_VARIABLE(rmse);

  na::utils::io::SVG()
      .setPointSize(0.001)
      .draw(grid.get(), na::core::Element::cell(), sol)
      .draw(grid.get())
      // .draw(fd.mesh(), static_cast<na::core::FieldCRef<f32>>(u_field))
      // .drawText(fd.mesh(), na::core::Element::cell(), x)
      // .draw(fd.mesh(), fd.boundaries())
      .write("grid.svg");

  return 0;
}
