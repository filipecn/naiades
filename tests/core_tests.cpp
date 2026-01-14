#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <naiades/core/boundary.h>
#include <naiades/core/discretization.h>
#include <naiades/core/field.h>
#include <naiades/core/operators.h>
#include <naiades/geo/grid.h>

using namespace naiades;
using namespace naiades::core;

TEST_CASE("Element", "[core]") {
  SECTION("print") {
#define PRINT(E) HERMES_INFO("{}", naiades::to_string(E));
    PRINT(element_primitive_bits::none)
    PRINT(element_primitive_bits::any)
    PRINT(element_primitive_bits::vertex)
    PRINT(element_primitive_bits::face)
    PRINT(element_primitive_bits::cell)
    PRINT(element_primitive_bits::particle)
    PRINT(element_primitive_bits::point)
    PRINT(element_primitive_bits::custom)

    PRINT(element_alignment_bits::none)
    PRINT(element_alignment_bits::any)
    PRINT(element_alignment_bits::x)
    PRINT(element_alignment_bits::y)
    PRINT(element_alignment_bits::z)
    PRINT(element_alignment_bits::custom)
    PRINT(element_alignment_bits::xy)
    PRINT(element_alignment_bits::xz)
    PRINT(element_alignment_bits::yz)

    PRINT(element_orientation_bits::none)
    PRINT(element_orientation_bits::any)
    PRINT(element_orientation_bits::x)
    PRINT(element_orientation_bits::y)
    PRINT(element_orientation_bits::z)
    PRINT(element_orientation_bits::neg_x)
    PRINT(element_orientation_bits::neg_y)
    PRINT(element_orientation_bits::neg_z)
    PRINT(element_orientation_bits::custom)
    PRINT(element_orientation_bits::xy)
    PRINT(element_orientation_bits::xz)
    PRINT(element_orientation_bits::yz)

    PRINT(Element::Type::NONE)
    PRINT(Element::Type::ANY)
    PRINT(Element::Type::CELL_CENTER)
    PRINT(Element::Type::FACE_CENTER)
    PRINT(Element::Type::VERTEX_CENTER)
    PRINT(Element::Type::POINT)
    PRINT(Element::Type::CUSTOM)
    PRINT(Element::Type::HORIZONTAL_FACE_CENTER)
    PRINT(Element::Type::V_FACE_CENTER)
    PRINT(Element::Type::X_FACE_CENTER)
    PRINT(Element::Type::XZ_FACE_CENTER)
    PRINT(Element::Type::VERTICAL_FACE_CENTER)
    PRINT(Element::Type::U_FACE_CENTER)
    PRINT(Element::Type::Y_FACE_CENTER)
    PRINT(Element::Type::YZ_FACE_CENTER)
    PRINT(Element::Type::DEPTH_FACE_CENTER)
    PRINT(Element::Type::W_FACE_CENTER)
    PRINT(Element::Type::Z_FACE_CENTER)
    PRINT(Element::Type::XY_FACE_CENTER)
#undef PRINT

    HERMES_INFO(
        "{}",
        naiades::to_string(Element(
            element_primitive_bits::particle | element_primitive_bits::vertex,
            element_alignment_bits::xz | element_alignment_bits::custom)));

    Element e(element_primitive_bits::particle | element_primitive_bits::vertex,
              element_alignment_bits::xz | element_alignment_bits::custom);
    HERMES_INFO("{}", hermes::cstr::bits(static_cast<u32>(e)));
    HERMES_INFO("{}", naiades::to_string(e.primitives()));
    HERMES_INFO("{}", naiades::to_string(e.alignments()));
    HERMES_INFO("{}", naiades::to_string(e.orientations()));
  }
  SECTION("sanity") {
    REQUIRE(Element(element_primitive_bits::any, element_alignment_bits::any,
                    element_orientation_bits::any) == Element::Type::ANY);
    REQUIRE(Element(element_primitive_bits::face, element_alignment_bits::xz,
                    element_orientation_bits::xz) ==
            Element::Type::XZ_FACE_CENTER);
    REQUIRE(
        Element::Type::CELL_CENTER ==
        Element(element_primitive_bits::cell, element_alignment_bits::none));
    REQUIRE(Element::Type::FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::any,
                    element_orientation_bits::any));
    REQUIRE(
        Element::Type::VERTEX_CENTER ==
        Element(element_primitive_bits::vertex, element_alignment_bits::none));
    REQUIRE(Element::Type::POINT == Element(element_primitive_bits::point,
                                            element_alignment_bits::none));
    REQUIRE(Element::Type::CUSTOM == Element(element_primitive_bits::custom,
                                             element_alignment_bits::custom,
                                             element_orientation_bits::custom));
    REQUIRE(Element::Type::HORIZONTAL_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xz,
                    element_orientation_bits::xz));
    REQUIRE(Element::Type::V_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xz,
                    element_orientation_bits::xz));
    REQUIRE(Element::Type::X_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xz,
                    element_orientation_bits::xz));
    REQUIRE(Element::Type::XZ_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xz,
                    element_orientation_bits::xz));
    REQUIRE(Element::Type::VERTICAL_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::yz,
                    element_orientation_bits::yz));
    REQUIRE(Element::Type::U_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::yz,
                    element_orientation_bits::yz));
    REQUIRE(Element::Type::Y_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::yz,
                    element_orientation_bits::yz));
    REQUIRE(Element::Type::YZ_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::yz,
                    element_orientation_bits::yz));
    REQUIRE(Element::Type::DEPTH_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xy,
                    element_orientation_bits::xy));
    REQUIRE(Element::Type::W_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xy,
                    element_orientation_bits::xy));
    REQUIRE(Element::Type::Z_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xy,
                    element_orientation_bits::xy));
    REQUIRE(Element::Type::XY_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xy,
                    element_orientation_bits::xy));

    auto e = Element(
        element_primitive_bits::particle | element_primitive_bits::vertex,
        element_alignment_bits::xz | element_alignment_bits::custom,
        element_orientation_bits::xz);
    REQUIRE(e.primitives().contain(element_primitive_bits::vertex));
    REQUIRE(e.is(element_primitive_bits::vertex));
    REQUIRE(e.primitives().contain(element_primitive_bits::particle));
    REQUIRE(e.is(element_primitive_bits::particle));
    REQUIRE(!e.primitives().contain(element_primitive_bits::cell));
    REQUIRE(!e.is(element_primitive_bits::cell));
    REQUIRE(e.alignments().contain(element_alignment_bits::xz));
    REQUIRE(e.has(element_alignment_bits::xz));
    REQUIRE(e.alignments().contain(element_alignment_bits::x));
    REQUIRE(e.has(element_alignment_bits::x));
    REQUIRE(e.alignments().contain(element_alignment_bits::z));
    REQUIRE(e.has(element_alignment_bits::z));
    REQUIRE(e.alignments().contain(element_alignment_bits::custom));
    REQUIRE(e.has(element_alignment_bits::custom));
    REQUIRE(!e.has(element_alignment_bits::y));
    REQUIRE(e.orientations().contain(element_orientation_bits::x));
    REQUIRE(e.orientations().contain(element_orientation_bits::z));
    REQUIRE(e.orientations().contain(element_orientation_bits::neg_x));
    REQUIRE(e.orientations().contain(element_orientation_bits::neg_z));
    REQUIRE(!e.orientations().contain(element_orientation_bits::custom));
    e.addPrimitives(element_primitive_bits::cell);
    REQUIRE(e.primitives().contain(element_primitive_bits::vertex));
    REQUIRE(e.primitives().contain(element_primitive_bits::particle));
    REQUIRE(e.primitives().contain(element_primitive_bits::cell));
    e.addAlignments(element_alignment_bits::z);
    REQUIRE(e.alignments().contain(element_alignment_bits::z));
    REQUIRE(e.alignments().contain(element_alignment_bits::xz));
    REQUIRE(e.alignments().contain(element_alignment_bits::x));
    REQUIRE(e.alignments().contain(element_alignment_bits::z));
    REQUIRE(e.alignments().contain(element_alignment_bits::custom));
    e.addorientations(element_orientation_bits::custom);
    REQUIRE(e.orientations().contain(element_orientation_bits::custom));
    e.setPrimitives(element_primitive_bits::custom);
    REQUIRE(!e.primitives().contain(element_primitive_bits::vertex));
    REQUIRE(!e.primitives().contain(element_primitive_bits::particle));
    REQUIRE(!e.primitives().contain(element_primitive_bits::cell));
    REQUIRE(e.primitives().contain(element_primitive_bits::custom));
    e.setAlignments(element_alignment_bits::custom);
    REQUIRE(!e.alignments().contain(element_alignment_bits::z));
    REQUIRE(!e.alignments().contain(element_alignment_bits::xz));
    REQUIRE(!e.alignments().contain(element_alignment_bits::x));
    REQUIRE(!e.alignments().contain(element_alignment_bits::z));
    REQUIRE(e.alignments().contain(element_alignment_bits::custom));
    e.setOrientations(element_orientation_bits::y);
    REQUIRE(e.orientations().contain(element_orientation_bits::y));
    REQUIRE(!e.orientations().contain(element_orientation_bits::x));
    REQUIRE(!e.orientations().contain(element_orientation_bits::z));
    REQUIRE(!e.orientations().contain(element_orientation_bits::neg_x));
    REQUIRE(!e.orientations().contain(element_orientation_bits::neg_z));
    REQUIRE(!e.orientations().contain(element_orientation_bits::any));
  }
}

TEST_CASE("FieldGroup", "[core]") {
  FieldGroup field_group;
  field_group.pushField<i32>();
  REQUIRE(field_group.resize(20) == HeError::NO_ERROR);
  auto acc = field_group.get<i32>(0);
  for (int i = 0; i < 20; ++i)
    acc[i] = i;

  // test contigous memory
  i32 *ptr = reinterpret_cast<i32 *>(field_group.getPtr(0, 0));
  for (int i = 0; i < 20; ++i)
    REQUIRE(ptr[i] == i);

  const auto &c_field_group = field_group;
  const i32 *c_ptr = reinterpret_cast<const i32 *>(c_field_group.getPtr(0, 0));
  for (int i = 0; i < 20; ++i)
    REQUIRE(c_ptr[i] == i);

  HERMES_INFO("{}", naiades::to_string(field_group));
}

TEST_CASE("FieldSet", "[core]") {
  FieldSet field_set;
  field_set.add<i32>(Element::Type::ANY, {"i32"});
  field_set.add<hermes::geo::vec2>(Element::Type::ANY, {"vec2"});
  h_size count = 20;
  REQUIRE(field_set.setElementCount(Element::Type::ANY, count) ==
          NaResult::noError());
  auto i32_acc = *field_set.get<i32>("i32");
  auto vec2_acc = *field_set.get<hermes::geo::vec2>("vec2");
  for (h_size i = 0; i < count; ++i) {
    i32_acc[i] = i;
    vec2_acc[i] = {i * 100.f, i * 10.f};
  }

  auto const_check = [](const FieldCRef<i32> &i32_acc,
                        const FieldCRef<hermes::geo::vec2> &vec2_acc,
                        h_size count) {
    for (i32 i = 0; i < static_cast<i32>(count); ++i) {
      REQUIRE(i32_acc[i] == i);
      REQUIRE(vec2_acc[i] == hermes::geo::vec2{i * 100.f, i * 10.f});
    }
  };
  const_check(i32_acc, vec2_acc, count);
}

TEST_CASE("Discrete Operator", "[core]") {
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
  SECTION("grid") {
    auto grid = geo::Grid2::Config()
                    .setCellSize({0.1, 0.2})
                    .setSize({3, 4})
                    .build()
                    .value();
    core::BoundarySet bs = BoundarySet::Config()
                               .setElement(core::Element::Type::FACE_CENTER)
                               .setTopology(&grid)
                               .build();

    auto boundary_element_types = {
        core::Element(core::Element::Type::X_FACE_CENTER)
            .setOrientations(core::element_orientation_bits::neg_y),
        core::Element(core::Element::Type::X_FACE_CENTER)
            .setOrientations(core::element_orientation_bits::y),
        core::Element(core::Element::Type::Y_FACE_CENTER)
            .setOrientations(core::element_orientation_bits::x),
        core::Element(core::Element::Type::Y_FACE_CENTER)
            .setOrientations(core::element_orientation_bits::neg_x)};
    for (auto b : boundary_element_types)
      bs.addRegion("p", grid.boundary(b));

    auto dirichlet = naiades::core::bc::Dirichlet::Ptr::shared(10);
    auto neumann = naiades::core::bc::Neumann::Ptr::shared();
    bs.set("p", neumann);
    bs.set("p", 0, dirichlet);

    REQUIRE(bs["p"].resolve() == NaResult::noError());

    SECTION("compute") {
      auto dirichlet_v = naiades::core::bc::Dirichlet::Ptr::shared(10);
      auto dirichlet_u = naiades::core::bc::Dirichlet::Ptr::shared(20);
      bs.addRegion("u", grid.boundary(core::Element::Type::U_FACE_CENTER));
      bs.addRegion("v", grid.boundary(core::Element::Type::V_FACE_CENTER));
      bs.set("u", dirichlet_u);
      bs.set("v", dirichlet_v);

      REQUIRE(bs["u"].resolve() == NaResult::noError());
      REQUIRE(bs["v"].resolve() == NaResult::noError());

      FieldSet fields;
      fields.add<f32>(core::Element::Type::V_FACE_CENTER, {"v"});
      fields.add<f32>(core::Element::Type::U_FACE_CENTER, {"u"});
      fields.setElementCountFrom(&grid);
      auto v = fields.get<f32>("v").value();
      auto u = fields.get<f32>("u").value();
      v = 0.0f;
      u = 0.0f;

      HERMES_WARN("u:\n{}", naiades::spatialFieldString<f32>(grid, u));
      HERMES_WARN("v:\n{}", naiades::spatialFieldString<f32>(grid, v));

      bs["u"].compute(u, u);
      bs["v"].compute(v, v);

      HERMES_WARN("v:\n{}", naiades::spatialFieldString<f32>(grid, v));
      HERMES_WARN("u:\n{}", naiades::spatialFieldString<f32>(grid, u));
    }

    SECTION("Laplacian") {
      auto op = DiscreteOperator::laplacian(
          &grid, bs["p"], core::Element::Type::CELL_CENTER,
          grid.flatIndex(core::Element::Type::CELL_CENTER, {1, 1}),
          core::Element::Type::FACE_CENTER);
      HERMES_WARN("{}", naiades::to_string(op));

      op = DiscreteOperator::laplacian(
          &grid, bs["p"], core::Element::Type::CELL_CENTER,
          grid.flatIndex(core::Element::Type::CELL_CENTER, {0, 1}),
          core::Element::Type::FACE_CENTER);
      HERMES_WARN("{}", naiades::to_string(op));

      op = DiscreteOperator::laplacian(
          &grid, bs["p"], core::Element::Type::CELL_CENTER,
          grid.flatIndex(core::Element::Type::CELL_CENTER, {0, 0}),
          core::Element::Type::FACE_CENTER);
      HERMES_WARN("{}", naiades::to_string(op));
    }
  }
}
