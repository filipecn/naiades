/// This example solves the Poisson equation
///     du   du
/// - ( -- + -- ) = f
///     dx   dy
///
/// with source term f(x,y)=2\pi^2\sin(\pi x)\sin(\pi y)
/// and zero Dirichlet boundary conditions.

#include <naiades/geo/grid.h>
#include <naiades/numeric/boundary_conditions.h>
#include <naiades/utils/fields.h>

namespace na = naiades;

int main() {
  // create the geometry
  auto grid = na::geo::Grid2::Config()
                  .setDomain(hermes::geo::bounds::bbox2::unit())
                  .setResolution({10, 10})
                  .build()
                  .value();

  // create numerical mesh from the geometry
  auto grid_fd = na::numeric::Grid2FD(&grid);

  // define the boundary faces (all faces in this case)
  grid_fd.addBoundary("u", naiades::core::Element::Type::FACE,
                      grid.boundary(na::core::Element::Type::FACE));

  // set Dirichlet boundary condition at the boundary
  auto dirichlet = na::numeric::bc::Dirichlet::Ptr::shared(0);
  grid_fd.setBoundaryCondition("u", 0, dirichlet,
                               na::core::Element::Type::CELL);

  // resolve boundary stencils
  grid_fd.resolveBoundaries();

  HERMES_INFO("{}", hermes::to_string(grid_fd));

  // create fields
  na::core::FieldSet fields;
  fields.add<f32>(na::core::Element::Type::CELL,
                  grid.flatIndexOffset(na::core::Element::Type::CELL),
                  {"u", "f"});
  fields.setElementCountFrom(&grid);

  // set source term
  auto f = fields.get<f32>("f").value();
  na::utils::setField<f32>(grid, f, [](const hermes::geo::point2 &p) -> f32 {
    return 2 * hermes::math::constants::pi * hermes::math::constants::pi *
           std::sin(hermes::math::constants::pi * p.x) *
           std::sin(hermes::math::constants::pi * p.y);
  });

  HERMES_WARN("{}", na::spatialFieldString(grid, na::core::FieldCRef<f32>(f)));

  return 0;
}
