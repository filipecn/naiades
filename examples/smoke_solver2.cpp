#include <naiades/solvers/sim_control.h>
#include <naiades/solvers/smoke_solver.h>

int main() {

  // setup simulation mesh

  naiades::geo::Grid2 grid;
  grid.setSize({50, 50});
  grid.setCellSize(1.f / 50);

  // create solver

  naiades::solvers::SmokeSolver2 solver =
      naiades::solvers::SmokeSolver2::Config().setGrid(grid).build().value();

  //                      b2
  //
  //                    Gd = 0
  //                    Gp = 0
  //                     v = 0       1,1
  //               -----------------
  //              |                 |          d = gaussian
  //       Gd = 0 |      dddd       | Gd = 0
  //  b3    u = 0 |     dddddd      |  u = 0    b1
  //       Gp = 0 |      dddd       | Gp = 0
  //              |                 |
  //               -----------------
  //           0,0       v = 0
  //                    Gp = 0
  //                    Gd = 0
  //
  //                      b0

  // setup boundary conditions

  auto dirichlet = naiades::core::bc::Dirichlet<f32>::Ptr::shared(0);
  solver.boundary().set(0, "v", dirichlet);
  solver.boundary().set(2, "v", dirichlet);
  solver.boundary().set(1, "u", dirichlet);
  solver.boundary().set(3, "u", dirichlet);
  solver.boundary().set("density", dirichlet);
  auto neumann = naiades::core::bc::Neumann<f32>::Ptr::shared();
  solver.boundary().set("p", neumann);

  HERMES_INFO("{}", naiades::to_string(solver.boundary()));

  naiades::solvers::SimControl().run(&solver);

  return 0;
}
