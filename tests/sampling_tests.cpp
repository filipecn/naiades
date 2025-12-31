#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <naiades/sampling/sampler.h>
#include <naiades/sampling/stencil.h>
#include <naiades/utils/fields.h>

#include <hermes/sampling/sampling.h>

using namespace naiades;
using namespace naiades::sampling;

TEST_CASE("stencil", "[sampling]") {
  u32 H = 2; // y
  u32 V = 2; // x
  hermes::geo::vec2 cell_size(0.2, 0.1);
  auto grid = geo::Grid2::Config()
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
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(-1, -1));

      REQUIRE(s.indices()[0] == 0);
      REQUIRE(s.size() == 1);
    }
    { // s1
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(0.1, -1));
      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 0);
      REQUIRE(s.indices()[1] == 1);
    }
    { // s2
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(0.3, -1));

      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 1);
      REQUIRE(s.indices()[1] == 2);
    }
    { // s3
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(1, -1));
      REQUIRE(s.size() == 1);
      REQUIRE(s.indices()[0] == 2);
    }
    { // s4
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(-1, 0.05));
      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 0);
      REQUIRE(s.indices()[1] == 3);
    }
    { // s5
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(0.1, 0.05));
      REQUIRE(s.size() == 4);
      REQUIRE(s.indices()[0] == 0);
      REQUIRE(s.indices()[1] == 3);
      REQUIRE(s.indices()[2] == 1);
      REQUIRE(s.indices()[3] == 4);
    }
    { // s6
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(0.3, 0.05));
      REQUIRE(s.size() == 4);
      REQUIRE(s.indices()[0] == 1);
      REQUIRE(s.indices()[1] == 4);
      REQUIRE(s.indices()[2] == 2);
      REQUIRE(s.indices()[3] == 5);
    }
    { // s7
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(1.0, 0.05));
      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 2);
      REQUIRE(s.indices()[1] == 5);
    }
    { // s8
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(-1, 0.15));
      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 3);
      REQUIRE(s.indices()[1] == 6);
    }
    { // s9
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(0.1, 0.15));
      REQUIRE(s.size() == 4);
      REQUIRE(s.indices()[0] == 3);
      REQUIRE(s.indices()[1] == 6);
      REQUIRE(s.indices()[2] == 4);
      REQUIRE(s.indices()[3] == 7);
    }
    { // s10
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(0.3, 0.15));
      REQUIRE(s.size() == 4);
      REQUIRE(s.indices()[0] == 4);
      REQUIRE(s.indices()[1] == 7);
      REQUIRE(s.indices()[2] == 5);
      REQUIRE(s.indices()[3] == 8);
    }
    { // s11
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(1.0, 0.15));
      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 5);
      REQUIRE(s.indices()[1] == 8);
    }
    { // s12
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(-1, 1));

      REQUIRE(s.size() == 1);
      REQUIRE(s.indices()[0] == 6);
    }
    { // s13
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(0.1, 1));
      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 6);
      REQUIRE(s.indices()[1] == 7);
    }
    { // s14
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(0.3, 1));

      REQUIRE(s.size() == 2);
      REQUIRE(s.indices()[0] == 7);
      REQUIRE(s.indices()[1] == 8);
    }
    { // s15
      auto s = Stencil::bilinear(grid, core::Element::Type::VERTEX_CENTER,
                                 hermes::geo::point2(1, 1));
      REQUIRE(s.size() == 1);
      REQUIRE(s.indices()[0] == 8);
    }
  }
}

struct SampleTestCaseParameters {
  SampleTestCaseParameters(f32 dx, f32 tol, f32 boundary_tol)
      : dx(dx), tol(tol), boundary_tol(boundary_tol) {}
  f32 dx;
  f32 tol;
  f32 boundary_tol;
};

TEST_CASE("sample", "[sampling]") {

  auto f = [](const hermes::geo::point2 &p) -> f32 {
    return p.x * p.x - p.x * p.y + p.y * p.y;
  };

  auto reset = [](const hermes::geo::point2 &p) -> f32 { return 0; };

  auto param = GENERATE(SampleTestCaseParameters(0.001, 1e-4, 1e-3));

  CAPTURE(param);

  auto grid = geo::Grid2::Config()
                  .setCellSize({param.dx, param.dx})
                  .setSize({10, 10})
                  .build()
                  .value();
  core::FieldSet fields;
  fields.add<f32>(core::Element::Type::CELL_CENTER,
                  {"cell_samples", "cell_exact"});
  fields.add<f32>(core::Element::Type::VERTEX_CENTER,
                  {"vertex_samples", "vertex_exact"});
  fields.add<f32>(core::Element::Type::X_FACE_CENTER,
                  {"x_face_samples", "x_face_exact"});
  fields.add<f32>(core::Element::Type::Y_FACE_CENTER,
                  {"y_face_samples", "y_face_exact"});
  fields.setElementCountFrom(&grid);

  core::FieldRef<f32> exact_fields[] = {
      *fields.get<f32>("vertex_exact"), *fields.get<f32>("cell_exact"),
      *fields.get<f32>("x_face_exact"), *fields.get<f32>("y_face_exact")};

  for (auto exact : exact_fields)
    utils::setField<f32>(grid, exact, f);

  core::FieldRef<f32> sample_fields[] = {
      *fields.get<f32>("vertex_samples"), *fields.get<f32>("cell_samples"),
      *fields.get<f32>("x_face_samples"), *fields.get<f32>("y_face_samples")};

  SECTION("between fields " + std::to_string(param.dx)) {
    for (h_size i = 0; i < 4; ++i) {
      auto &src_field = exact_fields[i];
      for (h_size j = 0; j < 4; ++j) {
        auto exact_dst_field = exact_fields[j];
        auto dst_field = sample_fields[j];
        utils::setField<f32>(grid, dst_field, reset);
        sample<f32>(grid, src_field, dst_field);
        for (h_size k = 0; k < dst_field.size(); ++k) {
          auto err =
              grid.isBoundary(dst_field.element(),
                              k + grid.flatIndexOffset(dst_field.element()))
                  ? param.boundary_tol
                  : param.tol;
          REQUIRE_THAT(dst_field[k],
                       Catch::Matchers::WithinAbs(exact_dst_field[k], err));
        }
      }
    }
  }
  SECTION("continum " + std::to_string(param.dx)) {
    hermes::geo::vec2 sample_dx(param.dx / 2, param.dx / 2);
    hermes::range2 range({0, 0}, {21, 21});
    auto sample_positions = hermes::sampling::sampleGrid(
        hermes::geo::bounds::bbox2::from<i32>(range, sample_dx), range.size());
    std::vector<f32> samples_exact;
    for (auto s : sample_positions)
      samples_exact.emplace_back(f(s));
    for (h_size i = 0; i < 4; ++i) {
      auto src_field = exact_fields[i];
      auto samples = sample<f32>(grid, src_field, sample_positions).value();
      REQUIRE(samples.size() == sample_positions.size());
      auto samples_acc = samples.get<f32>(0);
      for (h_size j = 0; j < samples.size(); ++j) {
        auto err = 1e-3f;
        REQUIRE_THAT(samples_acc[j],
                     Catch::Matchers::WithinAbs(samples_exact[j], err));
      }
    }
  }
}

TEST_CASE("sample vectors", "[sampling]") {

  auto fx = [](const hermes::geo::point2 &p) -> f32 {
    return p.x * p.x - p.x * p.y + p.y * p.y;
  };

  auto resetx = [](const hermes::geo::point2 &p) -> f32 { return 0; };

  auto f = [](const hermes::geo::point2 &p) -> hermes::geo::vec2 {
    return {p.x * p.x - p.x * p.y + p.y * p.y, p.x * p.x + p.y * p.y};
  };

  auto reset = [](const hermes::geo::point2 &p) -> hermes::geo::vec2 {
    return {};
  };

  auto param = GENERATE(SampleTestCaseParameters(0.001, 1e-4, 1e-3));

  CAPTURE(param);

  auto grid = geo::Grid2::Config()
                  .setCellSize({param.dx, param.dx})
                  .setSize({10, 10})
                  .build()
                  .value();
  core::FieldSet fields;
  fields.add<hermes::geo::vec2>(core::Element::Type::CELL_CENTER,
                                {"cell_samples", "cell_exact"});
  fields.add<hermes::geo::vec2>(core::Element::Type::VERTEX_CENTER,
                                {"vertex_samples", "vertex_exact"});
  fields.add<hermes::geo::vec2>(core::Element::Type::X_FACE_CENTER,
                                {"x_face_samples", "x_face_exact"});
  fields.add<hermes::geo::vec2>(core::Element::Type::Y_FACE_CENTER,
                                {"y_face_samples", "y_face_exact"});

  fields.add<f32>(core::Element::Type::CELL_CENTER,
                  {"cell_scalar_samples", "cell_scalar_exact"});
  fields.add<f32>(core::Element::Type::VERTEX_CENTER,
                  {"vertex_scalar_samples", "vertex_scalar_exact"});
  fields.add<f32>(core::Element::Type::X_FACE_CENTER,
                  {"x_face_scalar_samples", "x_face_scalar_exact"});
  fields.add<f32>(core::Element::Type::Y_FACE_CENTER,
                  {"y_face_scalar_samples", "y_face_scalar_exact"});
  fields.setElementCountFrom(&grid);

  core::FieldRef<hermes::geo::vec2> exact_fields[] = {
      *fields.get<hermes::geo::vec2>("vertex_exact"),
      *fields.get<hermes::geo::vec2>("cell_exact"),
      *fields.get<hermes::geo::vec2>("x_face_exact"),
      *fields.get<hermes::geo::vec2>("y_face_exact")};

  for (auto exact : exact_fields)
    utils::setField<hermes::geo::vec2>(grid, exact, f);

  core::FieldRef<hermes::geo::vec2> sample_fields[] = {
      *fields.get<hermes::geo::vec2>("vertex_samples"),
      *fields.get<hermes::geo::vec2>("cell_samples"),
      *fields.get<hermes::geo::vec2>("x_face_samples"),
      *fields.get<hermes::geo::vec2>("y_face_samples")};

  core::FieldRef<f32> sample_scalar_fields[] = {
      *fields.get<f32>("vertex_scalar_samples"),
      *fields.get<f32>("cell_scalar_samples"),
      *fields.get<f32>("x_face_scalar_samples"),
      *fields.get<f32>("y_face_scalar_samples")};

  SECTION("between fields " + std::to_string(param.dx)) {
    for (h_size i = 0; i < 4; ++i) {
      auto src_field = exact_fields[i];
      // into vector fields
      for (h_size j = 0; j < 4; ++j) {
        auto exact_dst_field = exact_fields[j];
        auto dst_field = sample_fields[j];
        utils::setField<hermes::geo::vec2>(grid, dst_field, reset);
        sample<hermes::geo::vec2>(grid, src_field, dst_field);
        for (h_size k = 0; k < dst_field.size(); ++k) {
          auto err =
              grid.isBoundary(dst_field.element(),
                              k + grid.flatIndexOffset(dst_field.element()))
                  ? param.boundary_tol
                  : param.tol;
          for (h_size c = 0; c < 2; ++c)
            REQUIRE_THAT(dst_field[k][c], Catch::Matchers::WithinAbs(
                                              exact_dst_field[k][c], err));
        }
      }
      // into scalar fields
      for (h_size j = 0; j < 4; ++j) {
        auto exact_dst_field = exact_fields[j];
        auto dst_scalar_field = sample_scalar_fields[j];
        for (h_size component = 0; component < 2; component++) {
          utils::setField<f32>(grid, dst_scalar_field, resetx);
          sample<hermes::geo::vec2>(grid, src_field, component,
                                    dst_scalar_field);
          for (h_size k = 0; k < dst_scalar_field.size(); ++k) {
            auto err = grid.isBoundary(
                           dst_scalar_field.element(),
                           k + grid.flatIndexOffset(dst_scalar_field.element()))
                           ? param.boundary_tol
                           : param.tol;
            REQUIRE_THAT(
                dst_scalar_field[k],
                Catch::Matchers::WithinAbs(exact_dst_field[k][component], err));
          }
        }
      }
    }
  }
  SECTION("continum " + std::to_string(param.dx)) {
    hermes::geo::vec2 sample_dx(param.dx / 2, param.dx / 2);
    hermes::range2 range({0, 0}, {21, 21});
    auto sample_positions = hermes::sampling::sampleGrid(
        hermes::geo::bounds::bbox2::from<i32>(range, sample_dx), range.size());
    std::vector<hermes::geo::vec2> samples_exact;
    for (auto s : sample_positions)
      samples_exact.emplace_back(f(s));
    for (h_size i = 0; i < 4; ++i) {
      auto src_field = exact_fields[i];
      auto samples =
          sample<hermes::geo::vec2>(grid, src_field, sample_positions).value();
      REQUIRE(samples.size() == sample_positions.size());
      auto samples_acc = samples.get<hermes::geo::vec2>(0);
      for (h_size j = 0; j < samples.size(); ++j) {
        auto err = 1e-3f;
        for (h_size c = 0; c < 2; ++c)
          REQUIRE_THAT(samples_acc[j][c],
                       Catch::Matchers::WithinAbs(samples_exact[j][c], err));
      }
    }
  }
}
