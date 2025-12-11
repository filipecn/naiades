#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <naiades/sampling/stencil.h>

using namespace naiades;
using namespace naiades::sampling;

TEST_CASE("stencil", "[sampling]") {
  u32 H = 2; // y
  u32 V = 2; // x
  hermes::geo::vec2 cell_size(0.2, 0.1);
  auto grid = geo::RegularGrid2::Config()
                  .setCellSize(cell_size)
                  .setSize({V, H})
                  .build()
                  .value();
  SECTION("bilinear - regular grid 2") {
    /*
     *   s12    s13      s14      s15
     *       6 ------ 7 ------ 8
     *       |        |        |
     *   s8  |   s9   |  s10   |  s11
     *       |        |        |
     *       3 ------ 4 ------ 5
     *       |        |        |
     *   s4  |   s5   |   s6   |  s7
     *       |        |        |
     *       0 ------ 1 ------ 2
     *   s0      s1       s2      s3
     */
    { // s0
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(-1, -1));

      REQUIRE(s.indices()[0] == 0);
      REQUIRE(s.size() == 1);
    }
    { // s1
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(0.1, -1));
      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 0);
      REQUIRE(s.indices()[1] == 1);
    }
    { // s2
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(0.3, -1));

      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 1);
      REQUIRE(s.indices()[1] == 2);
    }
    { // s3
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(1, -1));
      REQUIRE(s.size() == 1);
      REQUIRE(s.indices()[0] == 2);
    }
    { // s4
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(-1, 0.05));
      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 0);
      REQUIRE(s.indices()[1] == 3);
    }
    { // s5
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(0.1, 0.05));
      REQUIRE(s.size() == 4);
      REQUIRE(s.indices()[0] == 0);
      REQUIRE(s.indices()[1] == 3);
      REQUIRE(s.indices()[2] == 1);
      REQUIRE(s.indices()[3] == 4);
    }
    { // s6
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(0.3, 0.05));
      REQUIRE(s.size() == 4);
      REQUIRE(s.indices()[0] == 1);
      REQUIRE(s.indices()[1] == 4);
      REQUIRE(s.indices()[2] == 2);
      REQUIRE(s.indices()[3] == 5);
    }
    { // s7
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(1.0, 0.05));
      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 2);
      REQUIRE(s.indices()[1] == 5);
    }
    { // s8
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(-1, 0.15));
      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 3);
      REQUIRE(s.indices()[1] == 6);
    }
    { // s9
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(0.1, 0.15));
      REQUIRE(s.size() == 4);
      REQUIRE(s.indices()[0] == 3);
      REQUIRE(s.indices()[1] == 6);
      REQUIRE(s.indices()[2] == 4);
      REQUIRE(s.indices()[3] == 7);
    }
    { // s10
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(0.3, 0.15));
      REQUIRE(s.size() == 4);
      REQUIRE(s.indices()[0] == 4);
      REQUIRE(s.indices()[1] == 7);
      REQUIRE(s.indices()[2] == 5);
      REQUIRE(s.indices()[3] == 8);
    }
    { // s11
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(1.0, 0.15));
      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 5);
      REQUIRE(s.indices()[1] == 8);
    }
    { // s12
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(-1, 1));

      REQUIRE(s.size() == 1);
      REQUIRE(s.indices()[0] == 6);
    }
    { // s13
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(0.1, 1));
      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 6);
      REQUIRE(s.indices()[1] == 7);
    }
    { // s14
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(0.3, 1));

      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 7);
      REQUIRE(s.indices()[1] == 8);
    }
    { // s15
      auto s = Stencil::bilinear(grid, core::FieldLocation::VERTEX_CENTER,
                                 hermes::geo::point2(1, 1));
      REQUIRE(s.size() == 1);
      REQUIRE(s.indices()[0] == 8);
    }
  }
}
