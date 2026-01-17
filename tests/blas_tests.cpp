#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <naiades/core/blas.h>
#include <naiades/core/boundary.h>
#include <naiades/core/discretization.h>
#include <naiades/core/field.h>
#include <naiades/core/operators.h>
#include <naiades/geo/grid.h>

using namespace naiades;
using namespace naiades::core;

TEST_CASE("blas", "[core]") {
  SECTION("akb") {
    FieldSet fields;
    fields.add<f32>(Element::Type::CELL, 0, {"a", "b"});
    fields.setElementCount(Element::Type::CELL, 100);
    auto a = *fields.get<f32>("a");
    auto b = *fields.get<f32>("b");
    for (h_size i = 0; i < a.size(); ++i) {
      a[i] = i * 3;
      b[i] = i * 7;
    }
    REQUIRE(blas::akb<f32>(a, 13, b) == NaResult::noError());
    for (h_size i = 0; i < a.size(); ++i) {
      REQUIRE_THAT(a[i], Catch::Matchers::WithinAbs(i * 3 + 13 * i * 7, 1e-8f));
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

TEST_CASE("solve", "[core]") {

  auto param = GENERATE(SampleTestCaseParameters(0.001, 1e-4, 1e-3));

  CAPTURE(param);

  auto grid = geo::Grid2::Config()
                  .setCellSize({param.dx, param.dx})
                  .setSize({10, 10})
                  .build()
                  .value();

  core::BoundarySet bs = BoundarySet::Config().setTopology(&grid).build();

  auto boundary_element_types = {
      core::Element::Type::LEFT_FACE, core::Element::Type::DOWN_FACE,
      core::Element::Type::RIGHT_FACE, core::Element::Type::UP_FACE};
  for (auto b : boundary_element_types)
    bs.addRegion("p", core::Element::Type::FACE, grid.boundary(b));
}
