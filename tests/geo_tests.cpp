#include <catch2/catch_test_macros.hpp>

#include <naiades/geo/grid.h>

using namespace naiades;
using namespace naiades::geo;

TEST_CASE("regular grid 2", "[geo]") {
  SECTION("sanity") {
    u32 H = 3;  // y
    u32 V = 10; // x
    hermes::geo::vec2 cell_size(20, 10);
    auto grid =
        Grid2::Config().setCellSize(cell_size).setSize({V, H}).build().value();
    SECTION("index offset") {
      REQUIRE(grid.flatIndexOffset(core::Element::Type::CELL) == 0);
      REQUIRE(grid.flatIndexOffset(core::Element::Type::VERTEX) == 0);
      REQUIRE(grid.flatIndexOffset(core::Element::Type::HORIZONTAL_FACE) == 0);
      REQUIRE(grid.flatIndexOffset(core::Element::Type::VERTICAL_FACE) ==
              grid.resolution(core::Element::Type::HORIZONTAL_FACE).total());

      REQUIRE(grid.flatIndexOffset(core::Element::Type::HORIZONTAL_FACE) ==
              grid.flatIndexOffset(core::Element::Type::V_FACE));
      REQUIRE(grid.flatIndexOffset(core::Element::Type::HORIZONTAL_FACE) ==
              grid.flatIndexOffset(core::Element::Type::X_FACE));
      REQUIRE(grid.flatIndexOffset(core::Element::Type::VERTICAL_FACE) ==
              grid.flatIndexOffset(core::Element::Type::U_FACE));
      REQUIRE(grid.flatIndexOffset(core::Element::Type::VERTICAL_FACE) ==
              grid.flatIndexOffset(core::Element::Type::Y_FACE));
    }
    SECTION("offset") {
      REQUIRE(grid.gridOffset(core::Element::Type::CELL) ==
              hermes::geo::vec2(0.5, 0.5));
      REQUIRE(grid.gridOffset(core::Element::Type::VERTEX) ==
              hermes::geo::vec2(0.0, 0.0));
      REQUIRE(grid.gridOffset(core::Element::Type::HORIZONTAL_FACE) ==
              hermes::geo::vec2(0.5, 0.0));
      REQUIRE(grid.gridOffset(core::Element::Type::VERTICAL_FACE) ==
              hermes::geo::vec2(0.0, 0.5));

      REQUIRE(grid.gridOffset(core::Element::Type::HORIZONTAL_FACE) ==
              grid.gridOffset(core::Element::Type::V_FACE));
      REQUIRE(grid.gridOffset(core::Element::Type::HORIZONTAL_FACE) ==
              grid.gridOffset(core::Element::Type::X_FACE));

      REQUIRE(grid.gridOffset(core::Element::Type::VERTICAL_FACE) ==
              grid.gridOffset(core::Element::Type::U_FACE));
      REQUIRE(grid.gridOffset(core::Element::Type::VERTICAL_FACE) ==
              grid.gridOffset(core::Element::Type::Y_FACE));
    }
    SECTION("origin") {
      REQUIRE(grid.origin(core::Element::Type::CELL) ==
              hermes::geo::point2(0.5f * cell_size.x, 0.5f * cell_size.y));
      REQUIRE(grid.origin(core::Element::Type::VERTEX) ==
              hermes::geo::point2(0.0f * cell_size.x, 0.0f * cell_size.y));
      REQUIRE(grid.origin(core::Element::Type::HORIZONTAL_FACE) ==
              hermes::geo::point2(0.5f * cell_size.x, 0.0f * cell_size.y));
      REQUIRE(grid.origin(core::Element::Type::VERTICAL_FACE) ==
              hermes::geo::point2(0.0f * cell_size.x, 0.5f * cell_size.y));

      REQUIRE(grid.origin(core::Element::Type::HORIZONTAL_FACE) ==
              grid.origin(core::Element::Type::V_FACE));
      REQUIRE(grid.origin(core::Element::Type::HORIZONTAL_FACE) ==
              grid.origin(core::Element::Type::X_FACE));

      REQUIRE(grid.origin(core::Element::Type::VERTICAL_FACE) ==
              grid.origin(core::Element::Type::U_FACE));
      REQUIRE(grid.origin(core::Element::Type::VERTICAL_FACE) ==
              grid.origin(core::Element::Type::Y_FACE));
    }
    SECTION("resolution") {
      REQUIRE(grid.resolution(core::Element::Type::CELL) ==
              hermes::size2((V + 0), (H + 0)));
      REQUIRE(grid.resolution(core::Element::Type::VERTEX) ==
              hermes::size2((V + 1), (H + 1)));
      REQUIRE(grid.resolution(core::Element::Type::HORIZONTAL_FACE) ==
              hermes::size2((V + 0), (H + 1)));
      REQUIRE(grid.resolution(core::Element::Type::VERTICAL_FACE) ==
              hermes::size2((V + 1), (H + 0)));

      REQUIRE(grid.resolution(core::Element::Type::HORIZONTAL_FACE) ==
              grid.resolution(core::Element::Type::V_FACE));
      REQUIRE(grid.resolution(core::Element::Type::HORIZONTAL_FACE) ==
              grid.resolution(core::Element::Type::X_FACE));

      REQUIRE(grid.resolution(core::Element::Type::VERTICAL_FACE) ==
              grid.resolution(core::Element::Type::U_FACE));
      REQUIRE(grid.resolution(core::Element::Type::VERTICAL_FACE) ==
              grid.resolution(core::Element::Type::Y_FACE));
    }
    SECTION("elementCount") {
      REQUIRE(grid.elementCount(core::Element::Type::CELL) ==
              (H + 0) * (V + 0));
      REQUIRE(grid.elementCount(core::Element::Type::VERTEX) ==
              (H + 1) * (V + 1));
      REQUIRE(grid.elementCount(core::Element::Type::HORIZONTAL_FACE) ==
              (H + 1) * (V + 0));
      REQUIRE(grid.elementCount(core::Element::Type::VERTICAL_FACE) ==
              (H + 0) * (V + 1));

      REQUIRE(grid.elementCount(core::Element::Type::HORIZONTAL_FACE) ==
              grid.elementCount(core::Element::Type::V_FACE));
      REQUIRE(grid.elementCount(core::Element::Type::HORIZONTAL_FACE) ==
              grid.elementCount(core::Element::Type::X_FACE));

      REQUIRE(grid.elementCount(core::Element::Type::VERTICAL_FACE) ==
              grid.elementCount(core::Element::Type::U_FACE));
      REQUIRE(grid.elementCount(core::Element::Type::VERTICAL_FACE) ==
              grid.elementCount(core::Element::Type::Y_FACE));

      REQUIRE(grid.elementCount(core::Element::Type::FACE) ==
              grid.elementCount(core::Element::Type::HORIZONTAL_FACE) +
                  grid.elementCount(core::Element::Type::VERTICAL_FACE));
    }
    SECTION("indices") {
      h_size i = 0;
      for (auto ij :
           hermes::range2(grid.resolution(core::Element::Type::CELL))) {
        REQUIRE(i == grid.flatIndex(core::Element::Type::CELL, ij));
        REQUIRE(ij == grid.index(core::Element::Type::CELL, i));
        i++;
      }
      REQUIRE(i == grid.elementCount(core::Element::Type::CELL));
    }
    SECTION("safe indices") {
      /*    t6   |           t7            |  t8
       *    -----|---------------------------
       *       2 |  20   21  ...  28   29  |
       *         |                         |
       *  t3   1 |  10   11  t4   18   19  |  t5
       *         |                         |
       *       0 |  0    1   ...   8    9  |
       *       -------------------------------
       *    t0   |  0    1         8    9  |  t2
       *                     t1
       */

      REQUIRE(grid.safeIndex(core::Element::Type::CELL, {-1, -1}) ==
              hermes::index2(0, 0));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL, {5, -1}) ==
              hermes::index2(5, 0));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL, {12, -1}) ==
              hermes::index2(9, 0));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL, {-1, 1}) ==
              hermes::index2(0, 1));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL, {5, 1}) ==
              hermes::index2(5, 1));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL, {12, 1}) ==
              hermes::index2(9, 1));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL, {-1, 3}) ==
              hermes::index2(0, 2));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL, {5, 3}) ==
              hermes::index2(5, 2));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL, {12, 3}) ==
              hermes::index2(9, 2));

      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL, {-1, -1}) == 0);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL, {5, -1}) == 5);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL, {12, -1}) == 9);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL, {-1, 1}) == 10);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL, {5, 1}) == 15);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL, {12, 1}) == 19);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL, {-1, 3}) == 20);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL, {5, 3}) == 25);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL, {12, 3}) == 29);
    }
    SECTION("grid position") {
      REQUIRE(grid.gridPosition(core::Element::Type::CELL,
                                hermes::geo::point2(0, 0)) ==
              hermes::geo::point2(-0.5, -0.5));
      REQUIRE(grid.gridPosition(
                  core::Element::Type::CELL,
                  hermes::geo::point2(cell_size.x * 0.5, cell_size.y * 0.5)) ==
              hermes::geo::point2(0, 0));
    }
    SECTION("position") {
      hermes::index2 ij(5, 1);
      int flat_index = grid.flatIndex(core::Element::Type::CELL, ij);
      hermes::geo::point2 p = {(0.5f + ij.i) * cell_size.x,
                               (0.5f + ij.j) * cell_size.y};
      REQUIRE(grid.center(core::Element::Type::CELL, ij) == p);
      REQUIRE(grid.center(core::Element::Type::CELL, flat_index) == p);
      hermes::geo::point2 gp(5.5, 1.5);
      REQUIRE(grid.center(core::Element::Type::CELL, gp) ==
              hermes::geo::point2(5.5 * 20 + 10, 1.5 * 10 + 5));
    }
  }
  SECTION("faces") {
    u32 N = 3;
    u32 M = 5;
    auto grid = Grid2::Config().setSize({M, N}).build().value();
    SECTION("flat index") {
      { // x-faces
        h_size i = 0;
        for (auto ij :
             hermes::range2(grid.resolution(core::Element::Type::X_FACE))) {
          REQUIRE(i == grid.flatIndex(core::Element::Type::X_FACE, ij));
          REQUIRE(ij == grid.index(core::Element::Type::X_FACE, i));
          i++;
        }
      }
      { // y-faces
        h_size i = grid.resolution(core::Element::Type::X_FACE).total();
        for (auto ij :
             hermes::range2(grid.resolution(core::Element::Type::Y_FACE))) {
          REQUIRE(i == grid.flatIndex(core::Element::Type::Y_FACE, ij));
          REQUIRE(ij == grid.index(core::Element::Type::Y_FACE, i));
          i++;
        }
      }
    }
  }
  SECTION("Geometry") {
    u32 N = 3;
    u32 M = 5;
    auto grid = Grid2::Config().setSize({M, N}).build().value();
    SECTION("positions") {
      auto elements = {core::Element::Type::CELL, core::Element::Type::VERTEX,
                       core::Element::Type::X_FACE,
                       core::Element::Type::Y_FACE};
      for (auto element : elements) {
        auto positions = grid.centers(element);
        REQUIRE(positions.size() == grid.resolution(element).total());
      }

      auto all_faces = grid.centers(core::Element::Type::FACE);
      REQUIRE(all_faces.size() ==
              grid.resolution(core::Element::X_FACE).total() +
                  grid.resolution(core::Element::Y_FACE).total());
    }
  }
  SECTION("Topology") {
    u32 N = 3;
    u32 M = 5;
    auto grid = Grid2::Config().setSize({M, N}).build().value();
    SECTION("indices") {
      auto indices =
          grid.indices(core::Element::Type::CELL, core::Element::Type::VERTEX);
      REQUIRE(indices.size() ==
              grid.resolution(core::Element::Type::CELL).total());
      for (h_size i = 0; i < indices.size(); ++i) {
        REQUIRE(indices[i].size() == 4);
      }
    }
    SECTION("boundary") {
      SECTION("sanity") {
        auto elements = {core::Element::Type::CELL,
                         core::Element::Type::VERTEX};
        for (auto element : elements) {
          auto boundary = grid.boundary(element);
          auto res = grid.resolution(element);
          REQUIRE(boundary.size() == res.width * 2 + (res.height - 2) * 2);
        }
      }
      SECTION("isBoundary") {
        auto elements = {core::Element::Type::CELL,
                         core::Element::Type::VERTEX};
        for (auto element : elements) {
          auto range = hermes::range2(grid.resolution(element));
          for (auto ij : range) {
            auto fij = grid.safeFlatIndex(element, ij);
            REQUIRE(grid.isBoundary(element, fij) == range.isBoundary(ij));
          }
        }
      }
      SECTION("faces") {
        SECTION("all") {
          auto boundary = grid.boundary(core::Element::FACE);
          auto cell_res = grid.resolution(core::Element::CELL);
          REQUIRE(boundary.size() == cell_res.width * 2 + cell_res.height * 2);
        }
      }
    }
    SECTION("alignment") {
      auto check_f = [&](core::Element element) {
        for (h_size i = 0; i < grid.elementCount(element); ++i)
          REQUIRE(grid.elementAlignment(element, i) ==
                  core::element_alignment_bits::none);
      };
      auto elements = {core::Element::Type::CELL, core::Element::Type::VERTEX};
      for (auto element : elements)
        check_f(element);
      { // x-faces
        h_size i = 0;
        for (auto ij :
             hermes::range2(grid.resolution(core::Element::Type::X_FACE))) {
          HERMES_UNUSED_VARIABLE(ij);
          REQUIRE(grid.elementAlignment(core::Element::Type::FACE, i) ==
                  core::element_alignment_bits::x);
          i++;
        }
      }
      { // y-faces
        h_size i = grid.resolution(core::Element::Type::X_FACE).total();
        for (auto ij :
             hermes::range2(grid.resolution(core::Element::Type::Y_FACE))) {
          HERMES_UNUSED_VARIABLE(ij);
          REQUIRE(grid.elementAlignment(core::Element::Type::FACE, i) ==
                  core::element_alignment_bits::y);
          i++;
        }
      }
    }
    SECTION("orientation") {
      auto check_f = [&](core::Element element) {
        for (h_size i = 0; i < grid.elementCount(element); ++i)
          REQUIRE(grid.elementOrientation(element, i) ==
                  core::element_orientation_bits::none);
      };
      auto elements = {core::Element::Type::CELL, core::Element::Type::VERTEX};
      for (auto element : elements)
        check_f(element);
      { // x-faces
        h_size i = 0;
        for (auto ij :
             hermes::range2(grid.resolution(core::Element::Type::X_FACE))) {
          HERMES_UNUSED_VARIABLE(ij);
          if (i < M)
            REQUIRE(grid.elementOrientation(core::Element::Type::FACE, i) ==
                    core::element_orientation_bits::neg_y);
          else if (i > M * (N + 1) - M)
            REQUIRE(grid.elementOrientation(core::Element::Type::FACE, i) ==
                    core::element_orientation_bits::y);
          else
            REQUIRE(grid.elementOrientation(core::Element::Type::FACE, i) ==
                    core::element_orientation_bits::any_y);
          i++;
        }
      }
      { // y-faces
        h_size i = grid.resolution(core::Element::Type::X_FACE).total();
        auto fij = 0;
        for (auto ij :
             hermes::range2(grid.resolution(core::Element::Type::Y_FACE))) {
          HERMES_UNUSED_VARIABLE(ij);
          if (fij % (M + 1) == 0)
            REQUIRE(grid.elementOrientation(core::Element::Type::FACE, i) ==
                    core::element_orientation_bits::neg_x);
          else if ((fij + 1) % (M + 1) == 0)
            REQUIRE(grid.elementOrientation(core::Element::Type::FACE, i) ==
                    core::element_orientation_bits::x);
          else
            REQUIRE(grid.elementOrientation(core::Element::Type::FACE, i) ==
                    core::element_orientation_bits::any_x);
          i++;
          fij++;
        }
      }
    }
    // SECTION("start") {
    //   auto interior_star = grid.star(
    //       core::Element::CELL, grid.flatIndex(core::Element::CELL, {1, 1}),
    //       core::Element::FACE);
    //   REQUIRE(interior_star.size() == 4);
    //   for (auto n : interior_star)
    //     HERMES_INFO(
    //         "{} {}", naiades::to_string(n),
    //         hermes::to_string(grid.index(core::Element::CELL, n.index)));
    //   auto boundary_star = grid.star(
    //       core::Element::CELL, grid.flatIndex(core::Element::CELL, {0, 0}),
    //       core::Element::FACE);
    //   for (auto n : boundary_star)
    //     if (n.is_boundary) {
    //       HERMES_INFO(
    //           "{} {}", naiades::to_string(n),
    //           hermes::to_string(grid.index(core::Element::FACE, n.index)));
    //     } else {
    //       HERMES_INFO(
    //           "{} {}", naiades::to_string(n),
    //           hermes::to_string(grid.index(core::Element::CELL, n.index)));
    //     }
    // }
    // SECTION("neighbours") {
    //   {
    //     auto element = core::Element::Type::X_FACE;
    //     auto res = grid.resolution(element);
    //     for (auto ij : hermes::range2(res)) {
    //       auto ns = grid.neighbours(element, grid.flatIndex(element, ij),
    //                                 core::Element::Type::CELL);
    //       if (ij.j == 0 || ij.j == static_cast<i32>(res.height) - 1)
    //         REQUIRE(ns.size() == 1);
    //       else
    //         REQUIRE(ns.size() == 2);
    //     }
    //   }
    // }
  }
}
