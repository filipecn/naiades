#include <catch2/catch_test_macros.hpp>

#include <naiades/core/sim_mesh.h>

using namespace naiades;
using namespace naiades::core;

TEST_CASE("sim mesh", "[core]") {
  SECTION("from RegularGrid2") {
    hermes::size2 cell_res(10, 5);
    auto grid = geo::RegularGrid2::Config().setSize(cell_res).build().value();
    auto sm = SimMesh::Config::from(grid).build().value();
    REQUIRE(sm.cellBoundary().primitives.size() ==
            cell_res.width * 2 + (cell_res.height - 2) * 2);
    REQUIRE(sm.faceBoundary().primitives.size() ==
            cell_res.width * 2 + cell_res.height * 2);
  }
}
