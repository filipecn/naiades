/// This example solves the Poisson equation
///     du   du
/// - ( -- + -- ) = f
///     dx   dy
///
/// with source term f(x,y)=2\pi^2\sin(\pi x)\sin(\pi y)
/// and zero Dirichlet boundary conditions.

#include <naiades/geo/he.h>
#include <naiades/numeric/boundary_conditions.h>
#include <naiades/numeric/linear_solvers.h>
#include <naiades/utils/fields.h>
#include <naiades/utils/io.h>

namespace na = naiades;

int main() {
  // create numerical mesh (a 2d triangle mesh) with RBF-FD discretization

  na::geo::HE2 he_mesh;

  na::utils::io::SVG("grid.svg")
      .setDimensions(he_mesh.bbounds())
      .draw(he_mesh)
      .write();

  return 0;
}
