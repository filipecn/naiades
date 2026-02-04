#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <naiades/geo/grid.h>
#include <naiades/numeric/discrete_operator.h>

using namespace naiades;
using namespace naiades::numeric;

TEST_CASE("Discrete Operator", "[numeric]") {
  SECTION("Sanity") {
    DiscreteOperator op;
    REQUIRE(op.size() == 0);
    REQUIRE_THAT(op.constant(), Catch::Matchers::WithinAbs(0, 1e-8));
    op.add(1, 1.0);
    DiscreteOperator op2;
    op2.add(1, 1.0);
    op2.add(2, 1.0);
    op2.add(2, 1.0);
    op += op2;
    REQUIRE_THAT(op[1], Catch::Matchers::WithinAbs(2.0, 1e-8));
    REQUIRE_THAT(op[2], Catch::Matchers::WithinAbs(2.0, 1e-8));
  }
}
TEST_CASE("Grid2FD", "[numeric]") {
  auto grid = geo::Grid2::Config()
                  .setCellSize({0.1, 0.2})
                  .setSize({3, 4})
                  .build()
                  .value();
  auto grid_fd = numeric::Grid2FD(&grid);

  auto boundary_element_types = {
      core::Element::Type::LEFT_FACE, core::Element::Type::DOWN_FACE,
      core::Element::Type::RIGHT_FACE, core::Element::Type::UP_FACE};
  for (auto b : boundary_element_types)
    grid_fd.addBoundary("p", core::Element::Type::FACE, grid.boundary(b));

  auto dirichlet = bc::Dirichlet::Ptr::shared(10);
  auto neumann = bc::Neumann::Ptr::shared();
  grid_fd.setBoundaryCondition("p", neumann, core::Element::Type::CELL);
  grid_fd.setBoundaryCondition("p", 0, dirichlet, core::Element::Type::CELL);

  SECTION("boundary") {
    REQUIRE(grid_fd.resolveBoundaries() == NaResult::noError());
  }

  SECTION("compute") {
    // auto dirichlet_v = naiades::core::bc::Dirichlet::Ptr::shared(10);
    // auto dirichlet_u = naiades::core::bc::Dirichlet::Ptr::shared(20);
    // bs.addRegion("u", core::Element::Type::FACE,
    //              grid.boundary(core::Element::Type::U_FACE));
    // bs.addRegion("v", core::Element::Type::FACE,
    //              grid.boundary(core::Element::Type::V_FACE));
    // bs.set("u", dirichlet_u, Element::Type::FACE);
    // bs.set("v", dirichlet_v, Element::Type::FACE);

    // REQUIRE(bs["u"].resolve() == NaResult::noError());
    // REQUIRE(bs["v"].resolve() == NaResult::noError());

    // FieldSet fields;
    // fields.add<f32>(core::Element::Type::U_FACE,
    //                 grid.flatIndexOffset(core::Element::Type::U_FACE),
    //                 {"u"});
    // fields.add<f32>(core::Element::Type::V_FACE,
    //                 grid.flatIndexOffset(core::Element::Type::V_FACE),
    //                 {"v"});
    // fields.setElementCountFrom(&grid);
    // auto v = fields.get<f32>("v").value();
    // auto u = fields.get<f32>("u").value();
    // v = 0.0f;
    // u = 0.0f;

    // bs["u"].compute(u, u);
    // bs["v"].compute(v, v);

    // HERMES_WARN("v:\n{}", naiades::spatialFieldString<f32>(grid, v));
    // HERMES_WARN("u:\n{}", naiades::spatialFieldString<f32>(grid, u));
  }

  // SECTION("Laplacian") {
  //   auto op = DiscreteOperator::laplacian(
  //       &grid, bs["p"], core::Element::Type::CELL,
  //       grid.flatIndex(core::Element::Type::CELL, {1, 1}),
  //       core::Element::Type::FACE);
  //   HERMES_WARN("{}", naiades::to_string(op));

  //  op = DiscreteOperator::laplacian(
  //      &grid, bs["p"], core::Element::Type::CELL,
  //      grid.flatIndex(core::Element::Type::CELL, {0, 1}),
  //      core::Element::Type::FACE);
  //  HERMES_WARN("{}", naiades::to_string(op));

  //  op = DiscreteOperator::laplacian(
  //      &grid, bs["p"], core::Element::Type::CELL,
  //      grid.flatIndex(core::Element::Type::CELL, {0, 0}),
  //      core::Element::Type::FACE);
  //  HERMES_WARN("{}", naiades::to_string(op));

  //  op = DiscreteOperator::laplacian(
  //      &grid, bs["p"], core::Element::Type::CELL,
  //      grid.flatIndex(core::Element::Type::CELL, {2, 3}),
  //      core::Element::Type::FACE);
  //  HERMES_WARN("{}", naiades::to_string(op));
  //}
}
