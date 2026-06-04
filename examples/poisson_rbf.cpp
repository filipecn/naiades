/// This example solves the Poisson equation
///     du   du
/// - ( -- + -- ) = f
///     dx   dy
///
/// with source term f(x,y)=2\pi^2\sin(\pi x)\sin(\pi y)
/// and zero Dirichlet boundary conditions.

#include <naiades/geo/he.h>
#include <naiades/geo/utils.h>
#include <naiades/numeric/boundary_conditions.h>
#include <naiades/numeric/linear_solvers.h>
#include <naiades/utils/fields.h>
#include <naiades/utils/io.h>

namespace na = naiades;

int main() {
  // create numerical mesh (a 2d triangle mesh) with RBF-FD discretization

  na::geo::HE2 he_mesh;

  he_mesh.addVertex({0.f, 0.f});
  he_mesh.addVertex({0.1f, 0.f});
  he_mesh.addVertex({0.f, 0.1f});
  he_mesh.addVertex({0.1f, 0.1f});

  he_mesh.addCell({0, 1, 2});
  he_mesh.addCell({1, 3, 2});

  HERMES_WARN("{}", hermes::to_string(he_mesh));

  std::vector<hermes::geo::point2> domain = {
      hermes::geo::point2(0, 0),
      hermes::geo::point2(1, 0),
      hermes::geo::point2(1, 1),
      hermes::geo::point2(0, 1),
  };
  auto tri = *na::geo::triangulate(domain);
  na::utils::io::SVG("tri.svg")
      .setDimensions((tri).bbounds())
      .disable(na::utils::io::draw_option_bits::indices |
               na::utils::io::draw_option_bits::normals |
               na::utils::io::draw_option_bits::faces |
               na::utils::io::draw_option_bits::cells |
               na::utils::io::draw_option_bits::vertices)
      .draw(static_cast<const na::core::Mesh2 &>(tri))
      .draw(static_cast<const na::core::Mesh2 &>(tri),
            tri.star({na::core::Element::cell(), na::core::Index::global(0)},
                     na::core::Element::face()))
      .draw(static_cast<const na::core::Mesh2 &>(tri),
            tri.star({na::core::Element::cell(), na::core::Index::global(50)},
                     na::core::Element::face()))
      .draw(static_cast<const na::core::Mesh2 &>(tri),
            tri.star({na::core::Element::cell(), na::core::Index::global(100)},
                     na::core::Element::face()))
      // .draw(*tri)
      .write();

  na::utils::io::SVG("mesh.svg")
      .setDimensions(he_mesh.bbounds())
      .draw(static_cast<const na::core::Mesh2 &>(he_mesh))
      .draw(he_mesh)
      .write();

  return 0;
}
