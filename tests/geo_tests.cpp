#include <catch2/catch_test_macros.hpp>

#include <naiades/geo/grid.h>

using namespace naiades;
using namespace naiades::geo;

TEST_CASE("regular grid 2", "[geo]") {
  SECTION("sanity") {
    u32 H = 3;  // y
    u32 V = 10; // x
    hermes::geo::vec2 cell_size(20, 10);
    RegularGrid2 grid = RegularGrid2::Config()
                            .setCellSize(cell_size)
                            .setSize({V, H})
                            .build()
                            .value();
    SECTION("offset") {
      REQUIRE(grid.gridOffset(core::FieldLocation::CELL_CENTER) ==
              hermes::geo::vec2(0.5, 0.5));
      REQUIRE(grid.gridOffset(core::FieldLocation::VERTEX_CENTER) ==
              hermes::geo::vec2(0.0, 0.0));
      REQUIRE(grid.gridOffset(core::FieldLocation::HORIZONTAL_FACE_CENTER) ==
              hermes::geo::vec2(0.5, 0.0));
      REQUIRE(grid.gridOffset(core::FieldLocation::VERTICAL_FACE_CENTER) ==
              hermes::geo::vec2(0.0, 0.5));

      REQUIRE(grid.gridOffset(core::FieldLocation::HORIZONTAL_FACE_CENTER) ==
              grid.gridOffset(core::FieldLocation::V_FACE_CENTER));
      REQUIRE(grid.gridOffset(core::FieldLocation::HORIZONTAL_FACE_CENTER) ==
              grid.gridOffset(core::FieldLocation::X_FACE_CENTER));

      REQUIRE(grid.gridOffset(core::FieldLocation::VERTICAL_FACE_CENTER) ==
              grid.gridOffset(core::FieldLocation::U_FACE_CENTER));
      REQUIRE(grid.gridOffset(core::FieldLocation::VERTICAL_FACE_CENTER) ==
              grid.gridOffset(core::FieldLocation::Y_FACE_CENTER));
    }
    SECTION("origin") {
      REQUIRE(grid.origin(core::FieldLocation::CELL_CENTER) ==
              hermes::geo::point2(0.5f * cell_size.x, 0.5f * cell_size.y));
      REQUIRE(grid.origin(core::FieldLocation::VERTEX_CENTER) ==
              hermes::geo::point2(0.0f * cell_size.x, 0.0f * cell_size.y));
      REQUIRE(grid.origin(core::FieldLocation::HORIZONTAL_FACE_CENTER) ==
              hermes::geo::point2(0.5f * cell_size.x, 0.0f * cell_size.y));
      REQUIRE(grid.origin(core::FieldLocation::VERTICAL_FACE_CENTER) ==
              hermes::geo::point2(0.0f * cell_size.x, 0.5f * cell_size.y));

      REQUIRE(grid.origin(core::FieldLocation::HORIZONTAL_FACE_CENTER) ==
              grid.origin(core::FieldLocation::V_FACE_CENTER));
      REQUIRE(grid.origin(core::FieldLocation::HORIZONTAL_FACE_CENTER) ==
              grid.origin(core::FieldLocation::X_FACE_CENTER));

      REQUIRE(grid.origin(core::FieldLocation::VERTICAL_FACE_CENTER) ==
              grid.origin(core::FieldLocation::U_FACE_CENTER));
      REQUIRE(grid.origin(core::FieldLocation::VERTICAL_FACE_CENTER) ==
              grid.origin(core::FieldLocation::Y_FACE_CENTER));
    }
    SECTION("resolution") {
      REQUIRE(grid.resolution(core::FieldLocation::CELL_CENTER) ==
              hermes::size2((V + 0), (H + 0)));
      REQUIRE(grid.resolution(core::FieldLocation::VERTEX_CENTER) ==
              hermes::size2((V + 1), (H + 1)));
      REQUIRE(grid.resolution(core::FieldLocation::X_FACE_CENTER) ==
              hermes::size2((V + 1), (H + 0)));
      REQUIRE(grid.resolution(core::FieldLocation::Y_FACE_CENTER) ==
              hermes::size2((V + 0), (H + 1)));

      REQUIRE(grid.resolution(core::FieldLocation::HORIZONTAL_FACE_CENTER) ==
              grid.resolution(core::FieldLocation::V_FACE_CENTER));
      REQUIRE(grid.resolution(core::FieldLocation::HORIZONTAL_FACE_CENTER) ==
              grid.resolution(core::FieldLocation::X_FACE_CENTER));

      REQUIRE(grid.resolution(core::FieldLocation::VERTICAL_FACE_CENTER) ==
              grid.resolution(core::FieldLocation::U_FACE_CENTER));
      REQUIRE(grid.resolution(core::FieldLocation::VERTICAL_FACE_CENTER) ==
              grid.resolution(core::FieldLocation::Y_FACE_CENTER));
    }
    SECTION("locationCount") {
      REQUIRE(grid.locationCount(core::FieldLocation::CELL_CENTER) ==
              (H + 0) * (V + 0));
      REQUIRE(grid.locationCount(core::FieldLocation::VERTEX_CENTER) ==
              (H + 1) * (V + 1));
      REQUIRE(grid.locationCount(core::FieldLocation::X_FACE_CENTER) ==
              (H + 0) * (V + 1));
      REQUIRE(grid.locationCount(core::FieldLocation::Y_FACE_CENTER) ==
              (H + 1) * (V + 0));

      REQUIRE(grid.locationCount(core::FieldLocation::HORIZONTAL_FACE_CENTER) ==
              grid.locationCount(core::FieldLocation::V_FACE_CENTER));
      REQUIRE(grid.locationCount(core::FieldLocation::HORIZONTAL_FACE_CENTER) ==
              grid.locationCount(core::FieldLocation::X_FACE_CENTER));

      REQUIRE(grid.locationCount(core::FieldLocation::VERTICAL_FACE_CENTER) ==
              grid.locationCount(core::FieldLocation::U_FACE_CENTER));
      REQUIRE(grid.locationCount(core::FieldLocation::VERTICAL_FACE_CENTER) ==
              grid.locationCount(core::FieldLocation::Y_FACE_CENTER));
    }
    SECTION("indices") {
      h_size i = 0;
      for (auto ij :
           hermes::range2(grid.resolution(core::FieldLocation::CELL_CENTER))) {
        REQUIRE(i == grid.flatIndex(core::FieldLocation::CELL_CENTER, ij));
        REQUIRE(ij == grid.index(core::FieldLocation::CELL_CENTER, i));
        i++;
      }
      REQUIRE(i == grid.locationCount(core::FieldLocation::CELL_CENTER));
    }
    SECTION("safe indices") {
      ///    t6   |           t7            |  t8
      ///    -----|---------------------------
      ///       2 |  20   21  ...  28   29  |
      ///         |                         |
      ///  t3   1 |  10   11  t4   18   19  |  t5
      ///         |                         |
      ///       0 |  0    1   ...   8    9  |
      ///       -------------------------------
      ///    t0   |  0    1         8    9  |  t2
      ///                     t1

      REQUIRE(grid.safeIndex(core::FieldLocation::CELL_CENTER, {-1, -1}) ==
              hermes::index2(0, 0));
      REQUIRE(grid.safeIndex(core::FieldLocation::CELL_CENTER, {5, -1}) ==
              hermes::index2(5, 0));
      REQUIRE(grid.safeIndex(core::FieldLocation::CELL_CENTER, {12, -1}) ==
              hermes::index2(9, 0));
      REQUIRE(grid.safeIndex(core::FieldLocation::CELL_CENTER, {-1, 1}) ==
              hermes::index2(0, 1));
      REQUIRE(grid.safeIndex(core::FieldLocation::CELL_CENTER, {5, 1}) ==
              hermes::index2(5, 1));
      REQUIRE(grid.safeIndex(core::FieldLocation::CELL_CENTER, {12, 1}) ==
              hermes::index2(9, 1));
      REQUIRE(grid.safeIndex(core::FieldLocation::CELL_CENTER, {-1, 3}) ==
              hermes::index2(0, 2));
      REQUIRE(grid.safeIndex(core::FieldLocation::CELL_CENTER, {5, 3}) ==
              hermes::index2(5, 2));
      REQUIRE(grid.safeIndex(core::FieldLocation::CELL_CENTER, {12, 3}) ==
              hermes::index2(9, 2));

      REQUIRE(grid.safeFlatIndex(core::FieldLocation::CELL_CENTER, {-1, -1}) ==
              0);
      REQUIRE(grid.safeFlatIndex(core::FieldLocation::CELL_CENTER, {5, -1}) ==
              5);
      REQUIRE(grid.safeFlatIndex(core::FieldLocation::CELL_CENTER, {12, -1}) ==
              9);
      REQUIRE(grid.safeFlatIndex(core::FieldLocation::CELL_CENTER, {-1, 1}) ==
              10);
      REQUIRE(grid.safeFlatIndex(core::FieldLocation::CELL_CENTER, {5, 1}) ==
              15);
      REQUIRE(grid.safeFlatIndex(core::FieldLocation::CELL_CENTER, {12, 1}) ==
              19);
      REQUIRE(grid.safeFlatIndex(core::FieldLocation::CELL_CENTER, {-1, 3}) ==
              20);
      REQUIRE(grid.safeFlatIndex(core::FieldLocation::CELL_CENTER, {5, 3}) ==
              25);
      REQUIRE(grid.safeFlatIndex(core::FieldLocation::CELL_CENTER, {12, 3}) ==
              29);
    }
    SECTION("grid position") {
    // TODO
      }
    SECTION("position") {
  // TODO
    }
  }
}
