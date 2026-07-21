#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <naiades/geo/grid.h>
#include <naiades/geo/point_set.h>
#include <naiades/numeric/discrete_operator.h>
#include <naiades/numeric/rbf.h>
#include <naiades/numeric/stencil.h>

#include <hermes/numeric/interpolation.h>
#include <naiades/utils/io.h>

#include <random>

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
  auto fd = numeric::Grid2FD::Config()
                .setCellSize({0.1f, 0.2f})
                .setResolution({3, 4})
                .build()
                .value();

  auto boundary_element_types = {
      core::Element::Type::LEFT_FACE, core::Element::Type::DOWN_FACE,
      core::Element::Type::RIGHT_FACE, core::Element::Type::UP_FACE};
  // for (auto b : boundary_element_types)
  //   fd.addBoundary("p", core::Element::Type::FACE,
  //                  fd.mesh().boundaryIndices(b));

  // auto dirichlet = bc::Dirichlet::Ptr::shared(10);
  // auto neumann = bc::Neumann::Ptr::shared();
  // fd.setBoundaryCondition("p", neumann, core::Element::Type::CELL);
  // fd.setBoundaryCondition("p", 0, dirichlet, core::Element::Type::CELL);

  // SECTION("boundary") {
  //   REQUIRE(fd.resolveBoundaries() == NaResult::noError());
  // }

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
TEST_CASE("Differential RBF", "[numeric]") {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<f32> distrib(-1.0f, 1.0f);
  SECTION("2D - derivatives") {
    auto f = [](const hermes::geo::point2 &p) -> f32 {
      return p.x * p.x - p.x * p.y + p.y * p.y;
    };
    auto fx = [](const hermes::geo::point2 &p) -> f32 { return 2 * p.x - p.y; };
    auto fy = [](const hermes::geo::point2 &p) -> f32 {
      return -p.x + 2 * p.y;
    };
    auto loc = naiades::core::Element::point();
    std::vector<f32> scales = {1.0f, 0.1f, 0.01f};
    auto polynomial_type = naiades::numeric::PolynomialType::CUBIC;
    for (auto scale : scales) {
      auto svg = naiades::utils::io::SVG();
      h_index index = 0;
      for (h_index stencil_size = 20; stencil_size < 25; ++stencil_size) {
        hermes::geo::Transform2 transform =
            hermes::geo::Transform2::translate(
                {0.0f, static_cast<f32>(index++) * 4.0f * scale}) *
            hermes::geo::Transform2::scale({scale, scale});
        naiades::geo::PointSet2 ps;
        ps.emplace_back(hermes::geo::point2(0, 0));
        for (h_index i = 1; i < stencil_size; ++i)
          ps.emplace_back(hermes::geo::point2(distrib(gen), distrib(gen)));

        // ps.emplace_back(hermes::geo::point2(-1, 0));
        // ps.emplace_back(hermes::geo::point2(1, 0));
        // // ps.emplace_back(hermes::geo::point2(0, 1));
        // // ps.emplace_back(hermes::geo::point2(0, -1));
        ps.applyTransform(transform);

        std::vector<naiades::core::Neighbour> neighbors;
        for (h_index i = 0; i < ps.centers(loc).size(); ++i) {
          neighbors.emplace_back(naiades::core::Neighbour{
              naiades::core::ElementIndex(loc, naiades::core::Index::global(i)),
              hermes::geo::distance(
                  ps.center(naiades::core::ElementIndex(
                      loc, naiades::core::Index::global(i))),
                  ps.center(naiades::core::ElementIndex(
                      loc, naiades::core::Index::global(0))))});
        }

        auto stencil = naiades::numeric::Stencil2::build(&ps, neighbors);
        naiades::numeric::rbf::CubicKernel kernel;
        auto A = naiades::numeric::DifferentialRBF2::computeA(stencil, &kernel,
                                                              polynomial_type);
        REQUIRE(A);

        auto solver = naiades::numeric::DifferentialRBF2::buildSolver(*A);
        REQUIRE(solver);

        naiades::core::FieldSet fields;
        REQUIRE(fields.add<f32>(loc, 0, {"f"}) == NaResult::noError());
        fields.setElementCount(loc, ps.size());
        for (h_index i = 0; i < ps.size(); ++i) {
          auto ei =
              naiades::core::ElementIndex(loc, naiades::core::Index::global(i));
          fields.get<f32>("f").value()[*ei.index] = f(ps.center(ei));
        }

        auto dop = naiades::numeric::DifferentialRBF2::derivative(
            naiades::numeric::derivative_bits::x, *solver, stencil, &kernel,
            polynomial_type);

        REQUIRE(dop);

        // HERMES_LOG_VARIABLE(ps);

        svg.setPointSize(scale)
            .setTextSize(5 + 10 * hermes::numeric::smoothStep(
                                      scales.front(), scales.back(), scale))
            .draw(stencil)
            .text(hermes::cstr::format("scale={}", scale),
                  ps.bbounds().corner(2), naiades::utils::io::SVG::z_color)
            .text(hermes::cstr::format(
                      "err={}", std::fabs((*dop)(fields.get<f32>("f").value()) -
                                          fx(stencil[0]))),
                  ps.bbounds().corner(0), naiades::utils::io::SVG::z_color);
      }
      svg.write(hermes::cstr::format("test_{}.svg", scale));
    }
  }
  SECTION("2D - laplacian") {
    auto f = [](const hermes::geo::point2 &p) -> f32 {
      return std::exp(p.x) * std::sin(p.x);
    };
    auto fx = [](const hermes::geo::point2 &p) -> f32 {
      return std::exp(p.x) * (std::sin(p.x) + std::cos(p.x));
    };
    auto Lf = [](const hermes::geo::point2 &p) -> f32 {
      return 2.0f * std::exp(p.x) * std::cos(p.x);
    };
    auto loc = naiades::core::Element::point();
    std::vector<f32> scales = {1.0f, 0.1f, 0.01f};
    auto polynomial_type = naiades::numeric::PolynomialType::CUBIC;
    for (auto scale : scales) {
      auto svg = naiades::utils::io::SVG();
      h_index index = 0;
      for (h_index stencil_size = 20; stencil_size < 21; ++stencil_size) {
        hermes::geo::Transform2 transform =
            hermes::geo::Transform2::translate({0.5f, 0.5f}) *
            hermes::geo::Transform2::scale({scale, scale});
        naiades::geo::PointSet2 ps;
        ps.emplace_back(hermes::geo::point2(0, 0));
        for (h_index i = 1; i < stencil_size; ++i)
          ps.emplace_back(hermes::geo::point2(distrib(gen), distrib(gen)));

        ps.applyTransform(transform);

        std::vector<naiades::core::Neighbour> neighbors;
        for (h_index i = 0; i < ps.centers(loc).size(); ++i) {
          neighbors.emplace_back(naiades::core::Neighbour{
              naiades::core::ElementIndex(loc, naiades::core::Index::global(i)),
              hermes::geo::distance(
                  ps.center(naiades::core::ElementIndex(
                      loc, naiades::core::Index::global(i))),
                  ps.center(naiades::core::ElementIndex(
                      loc, naiades::core::Index::global(0))))});
        }

        auto stencil = naiades::numeric::Stencil2::build(&ps, neighbors);
        naiades::numeric::rbf::CubicKernel kernel;
        auto A = naiades::numeric::DifferentialRBF2::computeA(stencil, &kernel,
                                                              polynomial_type);
        REQUIRE(A);

        auto solver = naiades::numeric::DifferentialRBF2::buildSolver(*A);
        REQUIRE(solver);

        naiades::core::FieldSet fields;
        REQUIRE(fields.add<f32>(loc, 0, {"f"}) == NaResult::noError());
        fields.setElementCount(loc, ps.size());
        for (h_index i = 0; i < ps.size(); ++i) {
          auto ei =
              naiades::core::ElementIndex(loc, naiades::core::Index::global(i));
          fields.get<f32>("f").value()[*ei.index] = f(ps.center(ei));
        }

        auto dop = naiades::numeric::DifferentialRBF2::laplacian(
            *solver, stencil, &kernel, polynomial_type);

        // auto dop = naiades::numeric::DifferentialRBF2::derivative(
        //     naiades::numeric::derivative_bits::x, *solver, stencil, &kernel,
        //     polynomial_type);

        REQUIRE(dop);

        // HERMES_LOG_VARIABLE(ps);

        svg.setPointSize(scale)
            .setTextSize(5 + 10 * hermes::numeric::smoothStep(
                                      scales.front(), scales.back(), scale))
            .draw(stencil)
            .text(hermes::cstr::format("scale={}", scale),
                  ps.bbounds().corner(2), naiades::utils::io::SVG::z_color)
            .text(hermes::cstr::format(
                      "err={}", std::fabs((*dop)(fields.get<f32>("f").value()) -
                                          Lf(stencil[0]))),
                  ps.bbounds().corner(0), naiades::utils::io::SVG::z_color);
      }
      svg.write(hermes::cstr::format("test_laplacian_{}.svg", scale));
    }
  }
}