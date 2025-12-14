#include <catch2/catch_test_macros.hpp>

#include <naiades/core/spatial_discretization.h>

using namespace naiades;
using namespace naiades::core;

TEST_CASE("Element", "[core]") {
  SECTION("print") {
#define PRINT(E) HERMES_INFO("{}", naiades::to_string(E));
    PRINT(element_primitive_bits::any)
    PRINT(element_primitive_bits::vertex)
    PRINT(element_primitive_bits::face)
    PRINT(element_primitive_bits::cell)
    PRINT(element_primitive_bits::particle)
    PRINT(element_primitive_bits::point)
    PRINT(element_primitive_bits::custom)

    PRINT(element_alignment_bits::any)
    PRINT(element_alignment_bits::x)
    PRINT(element_alignment_bits::y)
    PRINT(element_alignment_bits::z)
    PRINT(element_alignment_bits::custom)
    PRINT(element_alignment_bits::xy)
    PRINT(element_alignment_bits::xz)
    PRINT(element_alignment_bits::yz)

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
  }
  SECTION("sanity") {
    REQUIRE(Element(element_primitive_bits::any, element_alignment_bits::any) ==
            Element::Type::ANY);
    REQUIRE(Element(element_primitive_bits::face, element_alignment_bits::xz) ==
            Element::Type::XZ_FACE_CENTER);
    REQUIRE(Element::Type::CELL_CENTER ==
            Element(element_primitive_bits::cell, element_alignment_bits::any));
    REQUIRE(Element::Type::FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::any));
    REQUIRE(
        Element::Type::VERTEX_CENTER ==
        Element(element_primitive_bits::vertex, element_alignment_bits::any));
    REQUIRE(Element::Type::POINT == Element(element_primitive_bits::point,
                                            element_alignment_bits::any));
    REQUIRE(Element::Type::CUSTOM == Element(element_primitive_bits::custom,
                                             element_alignment_bits::custom));
    REQUIRE(Element::Type::HORIZONTAL_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xz));
    REQUIRE(Element::Type::V_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xz));
    REQUIRE(Element::Type::X_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xz));
    REQUIRE(Element::Type::XZ_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xz));
    REQUIRE(Element::Type::VERTICAL_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::yz));
    REQUIRE(Element::Type::U_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::yz));
    REQUIRE(Element::Type::Y_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::yz));
    REQUIRE(Element::Type::YZ_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::yz));
    REQUIRE(Element::Type::DEPTH_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xy));
    REQUIRE(Element::Type::W_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xy));
    REQUIRE(Element::Type::Z_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xy));
    REQUIRE(Element::Type::XY_FACE_CENTER ==
            Element(element_primitive_bits::face, element_alignment_bits::xy));

    auto e = Element(
        element_primitive_bits::particle | element_primitive_bits::vertex,
        element_alignment_bits::xz | element_alignment_bits::custom);
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
  }
}
