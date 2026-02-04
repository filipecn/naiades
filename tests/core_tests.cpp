#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <naiades/core/field.h>
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
    PRINT(Element::Type::CELL)
    PRINT(Element::Type::FACE)
    PRINT(Element::Type::VERTEX)
    PRINT(Element::Type::POINT)
    PRINT(Element::Type::CUSTOM)
    PRINT(Element::Type::HORIZONTAL_FACE)
    PRINT(Element::Type::V_FACE)
    PRINT(Element::Type::X_FACE)
    PRINT(Element::Type::XZ_FACE)
    PRINT(Element::Type::VERTICAL_FACE)
    PRINT(Element::Type::U_FACE)
    PRINT(Element::Type::Y_FACE)
    PRINT(Element::Type::YZ_FACE)
    PRINT(Element::Type::DEPTH_FACE)
    PRINT(Element::Type::W_FACE)
    PRINT(Element::Type::Z_FACE)
    PRINT(Element::Type::XY_FACE)
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
                    element_orientation_bits::any_y) == Element::Type::XZ_FACE);
    REQUIRE(Element::Type::CELL == Element(element_primitive_bits::cell,
                                           element_alignment_bits::none));
    REQUIRE(Element::Type::FACE == Element(element_primitive_bits::face,
                                           element_alignment_bits::any,
                                           element_orientation_bits::any));
    REQUIRE(Element::Type::VERTEX == Element(element_primitive_bits::vertex,
                                             element_alignment_bits::none));
    REQUIRE(Element::Type::POINT == Element(element_primitive_bits::point,
                                            element_alignment_bits::none));
    REQUIRE(Element::Type::CUSTOM == Element(element_primitive_bits::custom,
                                             element_alignment_bits::custom,
                                             element_orientation_bits::custom));
    REQUIRE(Element::Type::HORIZONTAL_FACE ==
            Element(element_primitive_bits::face, element_alignment_bits::xz,
                    element_orientation_bits::any_y));
    REQUIRE(Element::Type::V_FACE == Element(element_primitive_bits::face,
                                             element_alignment_bits::xz,
                                             element_orientation_bits::any_y));
    REQUIRE(Element::Type::X_FACE == Element(element_primitive_bits::face,
                                             element_alignment_bits::xz,
                                             element_orientation_bits::any_y));
    REQUIRE(Element::Type::XZ_FACE == Element(element_primitive_bits::face,
                                              element_alignment_bits::xz,
                                              element_orientation_bits::any_y));
    REQUIRE(Element::Type::VERTICAL_FACE ==
            Element(element_primitive_bits::face, element_alignment_bits::yz,
                    element_orientation_bits::any_x));
    REQUIRE(Element::Type::U_FACE == Element(element_primitive_bits::face,
                                             element_alignment_bits::yz,
                                             element_orientation_bits::any_x));
    REQUIRE(Element::Type::Y_FACE == Element(element_primitive_bits::face,
                                             element_alignment_bits::yz,
                                             element_orientation_bits::any_x));
    REQUIRE(Element::Type::YZ_FACE == Element(element_primitive_bits::face,
                                              element_alignment_bits::yz,
                                              element_orientation_bits::any_x));
    REQUIRE(Element::Type::DEPTH_FACE ==
            Element(element_primitive_bits::face, element_alignment_bits::xy,
                    element_orientation_bits::any_z));
    REQUIRE(Element::Type::W_FACE == Element(element_primitive_bits::face,
                                             element_alignment_bits::xy,
                                             element_orientation_bits::any_z));
    REQUIRE(Element::Type::Z_FACE == Element(element_primitive_bits::face,
                                             element_alignment_bits::xy,
                                             element_orientation_bits::any_z));
    REQUIRE(Element::Type::XY_FACE == Element(element_primitive_bits::face,
                                              element_alignment_bits::xy,
                                              element_orientation_bits::any_z));

    REQUIRE(Element::Type::UP_FACE == Element(element_primitive_bits::face,
                                              element_alignment_bits::xz,
                                              element_orientation_bits::y));
    REQUIRE(Element::Type::DOWN_FACE ==
            Element(element_primitive_bits::face, element_alignment_bits::xz,
                    element_orientation_bits::neg_y));
    REQUIRE(Element::Type::LEFT_FACE ==
            Element(element_primitive_bits::face, element_alignment_bits::yz,
                    element_orientation_bits::neg_x));
    REQUIRE(Element::Type::RIGHT_FACE == Element(element_primitive_bits::face,
                                                 element_alignment_bits::yz,
                                                 element_orientation_bits::x));
    REQUIRE(Element::Type::FRONT_FACE == Element(element_primitive_bits::face,
                                                 element_alignment_bits::xy,
                                                 element_orientation_bits::z));
    REQUIRE(Element::Type::BACK_FACE ==
            Element(element_primitive_bits::face, element_alignment_bits::xy,
                    element_orientation_bits::neg_z));

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
  field_set.add<i32>(Element::Type::ANY, 0, {"i32"});
  field_set.add<hermes::geo::vec2>(Element::Type::ANY, 0, {"vec2"});
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
