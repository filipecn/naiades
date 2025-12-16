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
      REQUIRE(grid.flatIndexOffset(core::Element::Type::CELL_CENTER) == 0);
      REQUIRE(grid.flatIndexOffset(core::Element::Type::VERTEX_CENTER) == 0);
      REQUIRE(grid.flatIndexOffset(
                  core::Element::Type::HORIZONTAL_FACE_CENTER) == 0);
      REQUIRE(
          grid.flatIndexOffset(core::Element::Type::VERTICAL_FACE_CENTER) ==
          grid.resolution(core::Element::Type::HORIZONTAL_FACE_CENTER).total());

      REQUIRE(
          grid.flatIndexOffset(core::Element::Type::HORIZONTAL_FACE_CENTER) ==
          grid.flatIndexOffset(core::Element::Type::V_FACE_CENTER));
      REQUIRE(
          grid.flatIndexOffset(core::Element::Type::HORIZONTAL_FACE_CENTER) ==
          grid.flatIndexOffset(core::Element::Type::X_FACE_CENTER));
      REQUIRE(grid.flatIndexOffset(core::Element::Type::VERTICAL_FACE_CENTER) ==
              grid.flatIndexOffset(core::Element::Type::U_FACE_CENTER));
      REQUIRE(grid.flatIndexOffset(core::Element::Type::VERTICAL_FACE_CENTER) ==
              grid.flatIndexOffset(core::Element::Type::Y_FACE_CENTER));
    }
    SECTION("offset") {
      REQUIRE(grid.gridOffset(core::Element::Type::CELL_CENTER) ==
              hermes::geo::vec2(0.5, 0.5));
      REQUIRE(grid.gridOffset(core::Element::Type::VERTEX_CENTER) ==
              hermes::geo::vec2(0.0, 0.0));
      REQUIRE(grid.gridOffset(core::Element::Type::HORIZONTAL_FACE_CENTER) ==
              hermes::geo::vec2(0.5, 0.0));
      REQUIRE(grid.gridOffset(core::Element::Type::VERTICAL_FACE_CENTER) ==
              hermes::geo::vec2(0.0, 0.5));

      REQUIRE(grid.gridOffset(core::Element::Type::HORIZONTAL_FACE_CENTER) ==
              grid.gridOffset(core::Element::Type::V_FACE_CENTER));
      REQUIRE(grid.gridOffset(core::Element::Type::HORIZONTAL_FACE_CENTER) ==
              grid.gridOffset(core::Element::Type::X_FACE_CENTER));

      REQUIRE(grid.gridOffset(core::Element::Type::VERTICAL_FACE_CENTER) ==
              grid.gridOffset(core::Element::Type::U_FACE_CENTER));
      REQUIRE(grid.gridOffset(core::Element::Type::VERTICAL_FACE_CENTER) ==
              grid.gridOffset(core::Element::Type::Y_FACE_CENTER));
    }
    SECTION("origin") {
      REQUIRE(grid.origin(core::Element::Type::CELL_CENTER) ==
              hermes::geo::point2(0.5f * cell_size.x, 0.5f * cell_size.y));
      REQUIRE(grid.origin(core::Element::Type::VERTEX_CENTER) ==
              hermes::geo::point2(0.0f * cell_size.x, 0.0f * cell_size.y));
      REQUIRE(grid.origin(core::Element::Type::HORIZONTAL_FACE_CENTER) ==
              hermes::geo::point2(0.5f * cell_size.x, 0.0f * cell_size.y));
      REQUIRE(grid.origin(core::Element::Type::VERTICAL_FACE_CENTER) ==
              hermes::geo::point2(0.0f * cell_size.x, 0.5f * cell_size.y));

      REQUIRE(grid.origin(core::Element::Type::HORIZONTAL_FACE_CENTER) ==
              grid.origin(core::Element::Type::V_FACE_CENTER));
      REQUIRE(grid.origin(core::Element::Type::HORIZONTAL_FACE_CENTER) ==
              grid.origin(core::Element::Type::X_FACE_CENTER));

      REQUIRE(grid.origin(core::Element::Type::VERTICAL_FACE_CENTER) ==
              grid.origin(core::Element::Type::U_FACE_CENTER));
      REQUIRE(grid.origin(core::Element::Type::VERTICAL_FACE_CENTER) ==
              grid.origin(core::Element::Type::Y_FACE_CENTER));
    }
    SECTION("resolution") {
      REQUIRE(grid.resolution(core::Element::Type::CELL_CENTER) ==
              hermes::size2((V + 0), (H + 0)));
      REQUIRE(grid.resolution(core::Element::Type::VERTEX_CENTER) ==
              hermes::size2((V + 1), (H + 1)));
      REQUIRE(grid.resolution(core::Element::Type::HORIZONTAL_FACE_CENTER) ==
              hermes::size2((V + 0), (H + 1)));
      REQUIRE(grid.resolution(core::Element::Type::VERTICAL_FACE_CENTER) ==
              hermes::size2((V + 1), (H + 0)));

      REQUIRE(grid.resolution(core::Element::Type::HORIZONTAL_FACE_CENTER) ==
              grid.resolution(core::Element::Type::V_FACE_CENTER));
      REQUIRE(grid.resolution(core::Element::Type::HORIZONTAL_FACE_CENTER) ==
              grid.resolution(core::Element::Type::X_FACE_CENTER));

      REQUIRE(grid.resolution(core::Element::Type::VERTICAL_FACE_CENTER) ==
              grid.resolution(core::Element::Type::U_FACE_CENTER));
      REQUIRE(grid.resolution(core::Element::Type::VERTICAL_FACE_CENTER) ==
              grid.resolution(core::Element::Type::Y_FACE_CENTER));
    }
    SECTION("elementCount") {
      REQUIRE(grid.elementCount(core::Element::Type::CELL_CENTER) ==
              (H + 0) * (V + 0));
      REQUIRE(grid.elementCount(core::Element::Type::VERTEX_CENTER) ==
              (H + 1) * (V + 1));
      REQUIRE(grid.elementCount(core::Element::Type::HORIZONTAL_FACE_CENTER) ==
              (H + 1) * (V + 0));
      REQUIRE(grid.elementCount(core::Element::Type::VERTICAL_FACE_CENTER) ==
              (H + 0) * (V + 1));

      REQUIRE(grid.elementCount(core::Element::Type::HORIZONTAL_FACE_CENTER) ==
              grid.elementCount(core::Element::Type::V_FACE_CENTER));
      REQUIRE(grid.elementCount(core::Element::Type::HORIZONTAL_FACE_CENTER) ==
              grid.elementCount(core::Element::Type::X_FACE_CENTER));

      REQUIRE(grid.elementCount(core::Element::Type::VERTICAL_FACE_CENTER) ==
              grid.elementCount(core::Element::Type::U_FACE_CENTER));
      REQUIRE(grid.elementCount(core::Element::Type::VERTICAL_FACE_CENTER) ==
              grid.elementCount(core::Element::Type::Y_FACE_CENTER));

      REQUIRE(grid.elementCount(core::Element::Type::FACE_CENTER) ==
              grid.elementCount(core::Element::Type::HORIZONTAL_FACE_CENTER) +
                  grid.elementCount(core::Element::Type::VERTICAL_FACE_CENTER));
    }
    SECTION("indices") {
      h_size i = 0;
      for (auto ij :
           hermes::range2(grid.resolution(core::Element::Type::CELL_CENTER))) {
        REQUIRE(i == grid.flatIndex(core::Element::Type::CELL_CENTER, ij));
        REQUIRE(ij == grid.index(core::Element::Type::CELL_CENTER, i));
        i++;
      }
      REQUIRE(i == grid.elementCount(core::Element::Type::CELL_CENTER));
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

      REQUIRE(grid.safeIndex(core::Element::Type::CELL_CENTER, {-1, -1}) ==
              hermes::index2(0, 0));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL_CENTER, {5, -1}) ==
              hermes::index2(5, 0));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL_CENTER, {12, -1}) ==
              hermes::index2(9, 0));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL_CENTER, {-1, 1}) ==
              hermes::index2(0, 1));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL_CENTER, {5, 1}) ==
              hermes::index2(5, 1));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL_CENTER, {12, 1}) ==
              hermes::index2(9, 1));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL_CENTER, {-1, 3}) ==
              hermes::index2(0, 2));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL_CENTER, {5, 3}) ==
              hermes::index2(5, 2));
      REQUIRE(grid.safeIndex(core::Element::Type::CELL_CENTER, {12, 3}) ==
              hermes::index2(9, 2));

      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL_CENTER, {-1, -1}) ==
              0);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL_CENTER, {5, -1}) ==
              5);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL_CENTER, {12, -1}) ==
              9);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL_CENTER, {-1, 1}) ==
              10);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL_CENTER, {5, 1}) ==
              15);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL_CENTER, {12, 1}) ==
              19);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL_CENTER, {-1, 3}) ==
              20);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL_CENTER, {5, 3}) ==
              25);
      REQUIRE(grid.safeFlatIndex(core::Element::Type::CELL_CENTER, {12, 3}) ==
              29);
    }
    SECTION("grid position") {
      REQUIRE(grid.gridPosition(core::Element::Type::CELL_CENTER,
                                hermes::geo::point2(0, 0)) ==
              hermes::geo::point2(-0.5, -0.5));
      REQUIRE(grid.gridPosition(
                  core::Element::Type::CELL_CENTER,
                  hermes::geo::point2(cell_size.x * 0.5, cell_size.y * 0.5)) ==
              hermes::geo::point2(0, 0));
    }
    SECTION("position") {
      hermes::index2 ij(5, 1);
      int flat_index = grid.flatIndex(core::Element::Type::CELL_CENTER, ij);
      hermes::geo::point2 p = {(0.5f + ij.i) * cell_size.x,
                               (0.5f + ij.j) * cell_size.y};
      REQUIRE(grid.position(core::Element::Type::CELL_CENTER, ij) == p);
      REQUIRE(grid.position(core::Element::Type::CELL_CENTER, flat_index) == p);
      hermes::geo::point2 gp(5.5, 1.5);
      REQUIRE(grid.position(core::Element::Type::CELL_CENTER, gp) ==
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
        for (auto ij : hermes::range2(
                 grid.resolution(core::Element::Type::X_FACE_CENTER))) {
          REQUIRE(i == grid.flatIndex(core::Element::Type::X_FACE_CENTER, ij));
          REQUIRE(ij == grid.index(core::Element::Type::X_FACE_CENTER, i));
          i++;
        }
      }
      { // y-faces
        h_size i = grid.resolution(core::Element::Type::X_FACE_CENTER).total();
        for (auto ij : hermes::range2(
                 grid.resolution(core::Element::Type::Y_FACE_CENTER))) {
          REQUIRE(i == grid.flatIndex(core::Element::Type::Y_FACE_CENTER, ij));
          REQUIRE(ij == grid.index(core::Element::Type::Y_FACE_CENTER, i));
          i++;
        }
      }
    }
  }
  SECTION("interface") {
    u32 N = 3;
    u32 M = 5;
    auto grid = Grid2::Config().setSize({M, N}).build().value();
    SECTION("positions") {
      auto elements = {core::Element::Type::CELL_CENTER,
                       core::Element::Type::VERTEX_CENTER,
                       core::Element::Type::X_FACE_CENTER,
                       core::Element::Type::Y_FACE_CENTER};
      for (auto element : elements) {
        auto positions = grid.positions(element);
        REQUIRE(positions.size() == grid.resolution(element).total());
      }

      auto all_faces = grid.positions(core::Element::Type::FACE_CENTER);
      REQUIRE(all_faces.size() ==
              grid.resolution(core::Element::X_FACE_CENTER).total() +
                  grid.resolution(core::Element::Y_FACE_CENTER).total());
    }
    SECTION("indices") {
      auto indices = grid.indices(core::Element::Type::CELL_CENTER,
                                  core::Element::Type::VERTEX_CENTER);
      REQUIRE(indices.size() ==
              grid.resolution(core::Element::Type::CELL_CENTER).total());
      for (h_size i = 0; i < indices.size(); ++i) {
        REQUIRE(indices[i].size() == 4);
      }
    }
    SECTION("boundary") {
      SECTION("sanity") {
        auto elements = {core::Element::Type::CELL_CENTER,
                         core::Element::Type::VERTEX_CENTER};
        for (auto element : elements) {
          auto boundary = grid.boundary(element);
          auto res = grid.resolution(element);
          REQUIRE(boundary.size() == res.width * 2 + (res.height - 2) * 2);
        }
      }
      SECTION("faces") {
        SECTION("all") {
          auto boundary = grid.boundary(core::Element::FACE_CENTER);
          auto cell_res = grid.resolution(core::Element::CELL_CENTER);
          REQUIRE(boundary.size() == cell_res.width * 2 + cell_res.height * 2);
        }
      }
    }
    SECTION("alignment") {
      auto check_f = [&](core::Element element) {
        for (h_size i = 0; i < grid.elementCount(element); ++i)
          REQUIRE(grid.elementAlignment(element, i) ==
                  core::element_alignment_bits::any);
      };
      auto elements = {core::Element::Type::CELL_CENTER,
                       core::Element::Type::VERTEX_CENTER};
      for (auto element : elements)
        check_f(element);
      { // x-faces
        h_size i = 0;
        for (auto ij : hermes::range2(
                 grid.resolution(core::Element::Type::X_FACE_CENTER))) {
          REQUIRE(grid.elementAlignment(core::Element::Type::FACE_CENTER, i) ==
                  core::element_alignment_bits::x);
          i++;
        }
      }
      { // y-faces
        h_size i = grid.resolution(core::Element::Type::X_FACE_CENTER).total();
        for (auto ij : hermes::range2(
                 grid.resolution(core::Element::Type::Y_FACE_CENTER))) {
          REQUIRE(grid.elementAlignment(core::Element::Type::FACE_CENTER, i) ==
                  core::element_alignment_bits::y);
          i++;
        }
      }
    }
  }
}
