/* Copyright (c) 2025, FilipeCN.
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/// \file   element.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Discretization Element.

#pragma once

#include <naiades/base/index_set.h>

#include <hermes/base/flags.h>
#include <hermes/core/types.h>

#define NAIADES_ELEMENT_MASK(PRIMITIVE, ALIGNMENT, orientation, spaces)        \
  (static_cast<u32>(PRIMITIVE) | (static_cast<u32>(ALIGNMENT) << 8) |          \
   (static_cast<u32>(orientation) << 16) | (static_cast<u32>(spaces) << 24))

#define NAIADES_ELEMENT_MASK_SET_PRIMTIVE(M, P) (M | (static_cast<u32>(P)))

#define NAIADES_ELEMENT_MASK_SET_ALIGNMENT(M, A)                               \
  (M | (static_cast<u32>(A) << 8))

#define NAIADES_ELEMENT_MASK_SET_ORIENTATION(M, D)                             \
  (M | (static_cast<u32>(D) << 16))

#define NAIADES_ELEMENT_MASK_SET_SPACES(M, S) (M | (static_cast<u32>(S) << 24))

#define NAIADES_ELEMENT_MASK_GET_PRIMITIVE(M) (M & 0xff)

#define NAIADES_ELEMENT_MASK_GET_ALIGNMENT(M) ((M >> 8) & 0xff)

#define NAIADES_ELEMENT_MASK_GET_ORIENTATION(M) ((M >> 16) & 0xff)

#define NAIADES_ELEMENT_MASK_GET_SPACES(M) (M >> 24)

namespace naiades::core {

enum class element_space_bits : u32 {
  none = 0,
  boundary = 1 << 0,
  interior = 1 << 1,
  custom = 1 << 2,
  any = 0xff
};

enum class element_primitive_bits : u32 {
  none = 0,
  vertex = 1 << 0,
  face = 1 << 1,
  cell = 1 << 2,
  particle = 1 << 3,
  point = 1 << 4,
  custom = 1 << 5,
  any = 0xff
};

enum class element_alignment_bits : u32 {
  none = 0,
  x = 1 << 0,
  y = 1 << 1,
  z = 1 << 2,
  custom = 1 << 4,
  xy = x | y,
  xz = x | z,
  yz = y | z,
  any = 0xff,
};

enum class element_orientation_bits : u32 {
  none = 0,
  x = 1 << 0,
  y = 1 << 1,
  z = 1 << 2,
  neg_x = 1 << 3,
  neg_y = 1 << 4,
  neg_z = 1 << 5,
  custom = 1 << 6,
  any_x = x | neg_x,
  any_y = y | neg_y,
  any_z = z | neg_z,
  xy = x | y | neg_x | neg_y,
  xz = x | z | neg_x | neg_z,
  yz = y | z | neg_y | neg_z,
  right = x,
  left = neg_x,
  up = y,
  down = neg_y,
  front = z,
  back = neg_z,
  any = 0xff,
};

using element_spaces = hermes::Flags<core::element_space_bits>;
using element_primitives = hermes::Flags<core::element_primitive_bits>;
using element_alignments = hermes::Flags<core::element_alignment_bits>;
using element_orientations = hermes::Flags<core::element_orientation_bits>;

} // namespace naiades::core

namespace hermes {

template <> struct FlagTraits<naiades::core::element_space_bits> {
  static HERMES_CONST_OR_CONSTEXPR bool is_bitmask = true;
  static HERMES_CONST_OR_CONSTEXPR naiades::core::element_spaces all_flags =
      naiades::core::element_space_bits::any |
      naiades::core::element_space_bits::boundary |
      naiades::core::element_space_bits::interior |
      naiades::core::element_space_bits::custom;
};

template <> struct FlagTraits<naiades::core::element_primitive_bits> {
  static HERMES_CONST_OR_CONSTEXPR bool is_bitmask = true;
  static HERMES_CONST_OR_CONSTEXPR naiades::core::element_primitives all_flags =
      naiades::core::element_primitive_bits::any |
      naiades::core::element_primitive_bits::vertex |
      naiades::core::element_primitive_bits::face |
      naiades::core::element_primitive_bits::cell |
      naiades::core::element_primitive_bits::particle |
      naiades::core::element_primitive_bits::point |
      naiades::core::element_primitive_bits::custom;
};

template <> struct FlagTraits<naiades::core::element_alignment_bits> {
  static HERMES_CONST_OR_CONSTEXPR bool is_bitmask = true;
  static HERMES_CONST_OR_CONSTEXPR naiades::core::element_alignments all_flags =
      naiades::core::element_alignment_bits::any |
      naiades::core::element_alignment_bits::x |
      naiades::core::element_alignment_bits::y |
      naiades::core::element_alignment_bits::z |
      naiades::core::element_alignment_bits::custom;
};

template <> struct FlagTraits<naiades::core::element_orientation_bits> {
  static HERMES_CONST_OR_CONSTEXPR bool is_bitmask = true;
  static HERMES_CONST_OR_CONSTEXPR naiades::core::element_orientations
      all_flags = naiades::core::element_orientation_bits::any |
                  naiades::core::element_orientation_bits::x |
                  naiades::core::element_orientation_bits::y |
                  naiades::core::element_orientation_bits::z |
                  naiades::core::element_orientation_bits::neg_x |
                  naiades::core::element_orientation_bits::neg_y |
                  naiades::core::element_orientation_bits::neg_z |
                  naiades::core::element_orientation_bits::custom;
};

} // namespace hermes

namespace naiades::core {

class Element {
public:
  static Element cell();
  static Element face();
  static Element uFace();
  static Element vFace();
  static Element wFace();
  static Element vertex();
  static Element point();

  static Element boundaryCell();
  static Element boundaryFace();
  static Element boundaryVertex();

  static Element interiorCell();
  static Element interiorFace();
  static Element interiorVertex();

  static Element horizontalFace();
  static Element verticalFace();
  static Element depthFace();

  static Element xFace();
  static Element yFace();
  static Element zFace();

  ///      v --- V ---- v    v - VERTEX
  ///      |            |    C - CELL
  ///      U     C      U    V - [V|X|HORIZONTAL]_FACE
  ///      |            |    U - [U|Y|VERTICAL]_FACE
  ///      v --- V ---- v
  enum Type : u32 {
    NONE = 0,
    CELL = NAIADES_ELEMENT_MASK(
        element_primitive_bits::cell, element_alignment_bits::none,
        element_orientation_bits::none, element_space_bits::any),
    FACE = NAIADES_ELEMENT_MASK(
        element_primitive_bits::face, element_alignment_bits::any,
        element_orientation_bits::any, element_space_bits::any),
    VERTEX = NAIADES_ELEMENT_MASK(
        element_primitive_bits::vertex, element_alignment_bits::none,
        element_orientation_bits::none, element_space_bits::any),
    POINT = NAIADES_ELEMENT_MASK(
        element_primitive_bits::point, element_alignment_bits::none,
        element_orientation_bits::none, element_space_bits::any),
    CUSTOM = NAIADES_ELEMENT_MASK(
        element_primitive_bits::custom, element_alignment_bits::custom,
        element_orientation_bits::custom, element_space_bits::custom),

    BOUNDARY_CELL = NAIADES_ELEMENT_MASK(
        element_primitive_bits::cell, element_alignment_bits::none,
        element_orientation_bits::none, element_space_bits::boundary),
    BOUNDARY_FACE = NAIADES_ELEMENT_MASK(
        element_primitive_bits::face, element_alignment_bits::any,
        element_orientation_bits::any, element_space_bits::boundary),
    BOUNDARY_VERTEX = NAIADES_ELEMENT_MASK(
        element_primitive_bits::vertex, element_alignment_bits::none,
        element_orientation_bits::none, element_space_bits::boundary),

    INTERIOR_CELL = NAIADES_ELEMENT_MASK(
        element_primitive_bits::cell, element_alignment_bits::none,
        element_orientation_bits::none, element_space_bits::interior),
    INTERIOR_FACE = NAIADES_ELEMENT_MASK(
        element_primitive_bits::face, element_alignment_bits::any,
        element_orientation_bits::any, element_space_bits::interior),
    INTERIOR_VERTEX = NAIADES_ELEMENT_MASK(
        element_primitive_bits::vertex, element_alignment_bits::none,
        element_orientation_bits::none, element_space_bits::interior),

    HORIZONTAL_FACE = NAIADES_ELEMENT_MASK(
        element_primitive_bits::face, element_alignment_bits::xz,
        element_orientation_bits::any_y, element_space_bits::any),
    V_FACE = HORIZONTAL_FACE,
    X_FACE = HORIZONTAL_FACE,
    XZ_FACE = HORIZONTAL_FACE,

    BOUNDARY_HORIZONTAL_FACE = NAIADES_ELEMENT_MASK(
        element_primitive_bits::face, element_alignment_bits::xz,
        element_orientation_bits::any_y, element_space_bits::boundary),
    BOUNDARY_V_FACE = BOUNDARY_HORIZONTAL_FACE,
    BOUNDARY_X_FACE = BOUNDARY_HORIZONTAL_FACE,
    BOUNDARY_XZ_FACE = BOUNDARY_HORIZONTAL_FACE,

    UP_FACE = NAIADES_ELEMENT_MASK(
        element_primitive_bits::face, element_alignment_bits::xz,
        element_orientation_bits::y, element_space_bits::any),
    DOWN_FACE = NAIADES_ELEMENT_MASK(
        element_primitive_bits::face, element_alignment_bits::xz,
        element_orientation_bits::neg_y, element_space_bits::any),

    VERTICAL_FACE = NAIADES_ELEMENT_MASK(
        element_primitive_bits::face, element_alignment_bits::yz,
        element_orientation_bits::any_x, element_space_bits::any),
    U_FACE = VERTICAL_FACE,
    Y_FACE = VERTICAL_FACE,
    YZ_FACE = VERTICAL_FACE,

    BOUNDARY_VERTICAL_FACE = NAIADES_ELEMENT_MASK(
        element_primitive_bits::face, element_alignment_bits::yz,
        element_orientation_bits::any_x, element_space_bits::boundary),
    BOUNDARY_U_FACE = BOUNDARY_VERTICAL_FACE,
    BOUNDARY_Y_FACE = BOUNDARY_VERTICAL_FACE,
    BOUNDARY_YZ_FACE = BOUNDARY_VERTICAL_FACE,

    LEFT_FACE = NAIADES_ELEMENT_MASK(
        element_primitive_bits::face, element_alignment_bits::yz,
        element_orientation_bits::neg_x, element_space_bits::any),
    RIGHT_FACE = NAIADES_ELEMENT_MASK(
        element_primitive_bits::face, element_alignment_bits::yz,
        element_orientation_bits::x, element_space_bits::any),

    DEPTH_FACE = NAIADES_ELEMENT_MASK(
        element_primitive_bits::face, element_alignment_bits::xy,
        element_orientation_bits::any_z, element_space_bits::any),
    W_FACE = DEPTH_FACE,
    Z_FACE = DEPTH_FACE,
    XY_FACE = DEPTH_FACE,
    FRONT_FACE = NAIADES_ELEMENT_MASK(
        element_primitive_bits::face, element_alignment_bits::xy,
        element_orientation_bits::z, element_space_bits::any),
    BACK_FACE = NAIADES_ELEMENT_MASK(
        element_primitive_bits::face, element_alignment_bits::xy,
        element_orientation_bits::neg_z, element_space_bits::any),

    ANY = NAIADES_ELEMENT_MASK(
        element_primitive_bits::any, element_alignment_bits::any,
        element_orientation_bits::any, element_space_bits::any),
  };

  Element() noexcept = default;
  Element(element_primitives prim,
          element_alignments a = element_alignment_bits::none,
          element_orientations d = element_orientation_bits::none,
          element_spaces s = element_space_bits::any) noexcept
      : mask_(NAIADES_ELEMENT_MASK(prim, a, d, s)) {}
  Element(Type type) noexcept : mask_(static_cast<u32>(type)) {}
  Element(const Element &rhs) noexcept : mask_{rhs.mask_} {}
  Element(Element &&rhs) noexcept : mask_{rhs.mask_} {}

  Element &operator=(const Type &type) {
    mask_ = static_cast<u32>(type);
    return *this;
  }
  Element &operator=(const Element &element) {
    mask_ = element.mask_;
    return *this;
  }
  Element &operator=(Element &&element) {
    mask_ = element.mask_;
    return *this;
  }

  operator u32() const { return mask_; }

  bool operator==(const Type &type) const {
    return mask_ == static_cast<u32>(type);
  }

  bool operator==(const element_primitives &primitive) const {
    return NAIADES_ELEMENT_MASK_GET_PRIMITIVE(mask_) ==
           static_cast<u32>(primitive);
  }

  bool operator==(const element_alignments &alignment) const {
    return NAIADES_ELEMENT_MASK_GET_ALIGNMENT(mask_) ==
           static_cast<u32>(alignment);
  }
  bool operator==(const element_orientations &orientation) const {
    return NAIADES_ELEMENT_MASK_GET_ORIENTATION(mask_) ==
           static_cast<u32>(orientation);
  }
  bool operator==(const element_spaces &spaces) const {
    return NAIADES_ELEMENT_MASK_GET_SPACES(mask_) == static_cast<u32>(spaces);
  }
  inline Element &addPrimitives(element_primitives prim) {
    auto mask_prim = primitives();
    mask_ = NAIADES_ELEMENT_MASK_SET_PRIMTIVE(mask_, mask_prim | prim);
    return *this;
  }
  inline Element &addAlignments(element_alignments a) {
    auto mask_a = alignments();
    mask_ = NAIADES_ELEMENT_MASK_SET_ALIGNMENT(mask_, mask_a | a);
    return *this;
  }
  inline Element &addOrientations(element_orientations d) {
    auto mask_d = orientations();
    mask_ = NAIADES_ELEMENT_MASK_SET_ORIENTATION(mask_, mask_d | d);
    return *this;
  }
  inline Element &addSpaces(element_spaces s) {
    auto mask_s = spaces();
    mask_ = NAIADES_ELEMENT_MASK_SET_SPACES(mask_, mask_s | s);
    return *this;
  }
  inline Element &setPrimitives(element_primitives prim) {
    mask_ &= ~0xff;
    mask_ = NAIADES_ELEMENT_MASK_SET_PRIMTIVE(mask_, prim);
    return *this;
  }
  inline Element &setAlignments(element_alignments a) {
    mask_ &= 0xffff00ff;
    mask_ = NAIADES_ELEMENT_MASK_SET_ALIGNMENT(mask_, a);
    return *this;
  }
  inline Element &setOrientations(element_orientations d) {
    mask_ &= 0xff00ffff;
    mask_ = NAIADES_ELEMENT_MASK_SET_ORIENTATION(mask_, d);
    return *this;
  }
  inline Element &setSpaces(element_spaces s) {
    mask_ &= 0x00ffffff;
    mask_ = NAIADES_ELEMENT_MASK_SET_SPACES(mask_, s);
    return *this;
  }
  inline Element withAlignment(element_alignments a) const {
    auto e = *this;
    e.addAlignments(a);
    return e;
  }
  inline Element withOrientation(element_orientations d) const {
    auto e = *this;
    e.addOrientations(d);
    return e;
  }
  inline Element withAlignmentFrom(Type type) const {
    auto type_alignments = NAIADES_ELEMENT_MASK_GET_ALIGNMENT(type);
    Element e = *this;
    e.mask_ &= 0xffff00ff;
    e.mask_ = NAIADES_ELEMENT_MASK_SET_ALIGNMENT(e.mask_, type_alignments);
    return e;
  }
  inline Element withOrientationFrom(Type type) const {
    auto type_orientations = NAIADES_ELEMENT_MASK_GET_ORIENTATION(type);
    Element e = *this;
    e.mask_ &= 0xff00ffff;
    e.mask_ = NAIADES_ELEMENT_MASK_SET_ORIENTATION(e.mask_, type_orientations);
    return e;
  }
  inline Element withAnySpace() const {
    auto e = *this;
    e.mask_ &= 0x00ffffff;
    e.mask_ = NAIADES_ELEMENT_MASK_SET_SPACES(e.mask_, element_space_bits::any);
    return e;
  }

  inline element_primitives primitives() const {
    return static_cast<element_primitives>(
        NAIADES_ELEMENT_MASK_GET_PRIMITIVE(mask_));
  }
  inline element_alignments alignments() const {
    return static_cast<element_alignments>(
        NAIADES_ELEMENT_MASK_GET_ALIGNMENT(mask_));
  }
  inline element_orientations orientations() const {
    return static_cast<element_orientations>(
        NAIADES_ELEMENT_MASK_GET_ORIENTATION(mask_));
  }
  inline element_spaces spaces() const {
    return static_cast<element_spaces>(NAIADES_ELEMENT_MASK_GET_SPACES(mask_));
  }
  inline bool is(element_primitives prim) const {
    auto p = NAIADES_ELEMENT_MASK_GET_PRIMITIVE(mask_);
    return (p & static_cast<u32>(prim)) == static_cast<u32>(prim);
  }
  inline bool alignsTo(element_alignments a) const {
    auto _a = NAIADES_ELEMENT_MASK_GET_ALIGNMENT(mask_);
    return (_a & static_cast<u32>(a)) == static_cast<u32>(a);
  }
  inline bool orientsTo(element_orientations d) const {
    auto _d = NAIADES_ELEMENT_MASK_GET_ORIENTATION(mask_);
    return (_d & static_cast<u32>(d)) == static_cast<u32>(d);
  }
  inline bool in(element_spaces s) const {
    auto _s = NAIADES_ELEMENT_MASK_GET_SPACES(mask_);
    return (_s & static_cast<u32>(s)) == static_cast<u32>(s);
  }
  inline bool inAnySpace() const { return spaces() == element_space_bits::any; }

private:
  u32 mask_{};

  friend struct std::hash<naiades::core::Element>;
};

struct ElementIndex {
  static ElementIndex global(Element loc, h_size i);
  static ElementIndex local(Element loc, h_size i);

  Element element{Element::Type::ANY};
  Index index{Index::invalid()};

  ElementIndex();
  ElementIndex(Element loc, Index i);

  ElementIndex &operator++();
  bool operator==(const ElementIndex &rhs) const;

  friend struct std::hash<ElementIndex>;
};

#undef NAIADES_ELEMENT_MASK
#undef NAIADES_ELEMENT_MASK_SET_PRIMTIVE
#undef NAIADES_ELEMENT_MASK_SET_ALIGNMENT
#undef NAIADES_ELEMENT_MASK_SET_ORIENTATION
#undef NAIADES_ELEMENT_MASK_SET_SPACES
#undef NAIADES_ELEMENT_MASK_GET_PRIMITIVE
#undef NAIADES_ELEMENT_MASK_GET_ALIGNMENT
#undef NAIADES_ELEMENT_MASK_GET_ORIENTATION
#undef NAIADES_ELEMENT_MASK_GET_SPACES

} // namespace naiades::core

namespace std {
template <> struct hash<naiades::core::Element> {
  inline size_t operator()(const naiades::core::Element &x) const {
    return x.mask_;
  }
};

template <> struct hash<naiades::core::element_alignments> {
  inline size_t operator()(const naiades::core::element_alignments &x) const {
    return static_cast<u32>(x);
  }
};

template <> struct hash<naiades::core::element_orientations> {
  inline size_t operator()(const naiades::core::element_orientations &x) const {
    return static_cast<u32>(x);
  }
};

template <> struct hash<naiades::core::ElementIndex> {
  inline size_t operator()(const naiades::core::ElementIndex &x) const {
    std::size_t seed = std::hash<naiades::Index>{}(x.index);
    seed ^= std::hash<naiades::core::Element>{}(x.element) + 0x9e3779b9 +
            (seed << 6) + (seed >> 2);
    return seed;
  }
};

} // namespace std

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS

namespace hermes {
#define NAIADES_ENUM_TO_STRING_CASE(TYPE, NAME)                                \
  case TYPE::NAME: {                                                           \
    m.addFmt(#NAME);                                                           \
    break;                                                                     \
  }

#define NAIADES_ENUM_TO_STRING_APPEND(TYPE, NAME)                              \
  if (data.contain(TYPE::NAME))                                                \
    ss.emplace_back(#NAME);

template <> struct DebugTraits<naiades::core::element_spaces> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::core::element_spaces &data) {
    using namespace naiades::core;
    std::vector<std::string> ss;
    NAIADES_ENUM_TO_STRING_APPEND(element_space_bits, any)
    NAIADES_ENUM_TO_STRING_APPEND(element_space_bits, boundary)
    NAIADES_ENUM_TO_STRING_APPEND(element_space_bits, interior)
    NAIADES_ENUM_TO_STRING_APPEND(element_space_bits, custom)
    if (data == element_space_bits::none)
      ss.emplace_back("none");
    return DebugMessage().addFmt("{}", hermes::cstr::join(ss, "|"));
  }
};

template <> struct DebugTraits<naiades::core::element_primitives> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::core::element_primitives &data) {
    using namespace naiades::core;
    std::vector<std::string> ss;
    NAIADES_ENUM_TO_STRING_APPEND(element_primitive_bits, any)
    NAIADES_ENUM_TO_STRING_APPEND(element_primitive_bits, vertex)
    NAIADES_ENUM_TO_STRING_APPEND(element_primitive_bits, face)
    NAIADES_ENUM_TO_STRING_APPEND(element_primitive_bits, cell)
    NAIADES_ENUM_TO_STRING_APPEND(element_primitive_bits, particle)
    NAIADES_ENUM_TO_STRING_APPEND(element_primitive_bits, point)
    NAIADES_ENUM_TO_STRING_APPEND(element_primitive_bits, custom)
    if (data == element_primitive_bits::none)
      ss.emplace_back("none");
    return DebugMessage().addFmt("{}", hermes::cstr::join(ss, "|"));
  }
};

template <> struct DebugTraits<naiades::core::element_alignments> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::core::element_alignments &data) {
    using namespace naiades::core;
    std::vector<std::string> ss;
    NAIADES_ENUM_TO_STRING_APPEND(element_alignment_bits, any)
    NAIADES_ENUM_TO_STRING_APPEND(element_alignment_bits, x)
    NAIADES_ENUM_TO_STRING_APPEND(element_alignment_bits, y)
    NAIADES_ENUM_TO_STRING_APPEND(element_alignment_bits, z)
    NAIADES_ENUM_TO_STRING_APPEND(element_alignment_bits, custom)
    NAIADES_ENUM_TO_STRING_APPEND(element_alignment_bits, xy)
    NAIADES_ENUM_TO_STRING_APPEND(element_alignment_bits, xz)
    NAIADES_ENUM_TO_STRING_APPEND(element_alignment_bits, yz)
    if (data == element_alignment_bits::none)
      ss.emplace_back("none");
    return DebugMessage().addFmt("{}", hermes::cstr::join(ss, "|"));
  }
};

template <> struct DebugTraits<naiades::core::element_orientations> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::core::element_orientations &data) {
    using namespace naiades::core;
    std::vector<std::string> ss;
    NAIADES_ENUM_TO_STRING_APPEND(element_orientation_bits, any)
    NAIADES_ENUM_TO_STRING_APPEND(element_orientation_bits, x)
    NAIADES_ENUM_TO_STRING_APPEND(element_orientation_bits, y)
    NAIADES_ENUM_TO_STRING_APPEND(element_orientation_bits, z)
    NAIADES_ENUM_TO_STRING_APPEND(element_orientation_bits, neg_x)
    NAIADES_ENUM_TO_STRING_APPEND(element_orientation_bits, neg_y)
    NAIADES_ENUM_TO_STRING_APPEND(element_orientation_bits, neg_z)
    NAIADES_ENUM_TO_STRING_APPEND(element_orientation_bits, custom)
    NAIADES_ENUM_TO_STRING_APPEND(element_orientation_bits, xy)
    NAIADES_ENUM_TO_STRING_APPEND(element_orientation_bits, xz)
    NAIADES_ENUM_TO_STRING_APPEND(element_orientation_bits, yz)
    if (data == element_orientation_bits::none)
      ss.emplace_back("none");
    return DebugMessage().addFmt("{}", hermes::cstr::join(ss, "|"));
  }
};

template <> struct DebugTraits<naiades::core::element_primitive_bits> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage
  message(const naiades::core::element_primitive_bits &data) {
    using namespace naiades::core;
    auto m = DebugMessage();
    switch (data) {
      NAIADES_ENUM_TO_STRING_CASE(element_primitive_bits, none)
      NAIADES_ENUM_TO_STRING_CASE(element_primitive_bits, any)
      NAIADES_ENUM_TO_STRING_CASE(element_primitive_bits, vertex)
      NAIADES_ENUM_TO_STRING_CASE(element_primitive_bits, face)
      NAIADES_ENUM_TO_STRING_CASE(element_primitive_bits, cell)
      NAIADES_ENUM_TO_STRING_CASE(element_primitive_bits, particle)
      NAIADES_ENUM_TO_STRING_CASE(element_primitive_bits, point)
      NAIADES_ENUM_TO_STRING_CASE(element_primitive_bits, custom)
    }
    return m;
  }
};

template <> struct DebugTraits<naiades::core::element_alignment_bits> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage
  message(const naiades::core::element_alignment_bits &data) {
    using namespace naiades::core;
    auto m = DebugMessage();
    switch (data) {
      NAIADES_ENUM_TO_STRING_CASE(element_alignment_bits, none)
      NAIADES_ENUM_TO_STRING_CASE(element_alignment_bits, any)
      NAIADES_ENUM_TO_STRING_CASE(element_alignment_bits, x)
      NAIADES_ENUM_TO_STRING_CASE(element_alignment_bits, y)
      NAIADES_ENUM_TO_STRING_CASE(element_alignment_bits, z)
      NAIADES_ENUM_TO_STRING_CASE(element_alignment_bits, custom)
      NAIADES_ENUM_TO_STRING_CASE(element_alignment_bits, xy)
      NAIADES_ENUM_TO_STRING_CASE(element_alignment_bits, xz)
      NAIADES_ENUM_TO_STRING_CASE(element_alignment_bits, yz)
    }
    return m;
  }
};

template <> struct DebugTraits<naiades::core::element_orientation_bits> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage
  message(const naiades::core::element_orientation_bits &data) {
    using namespace naiades::core;
    auto m = DebugMessage();
    switch (data) {
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, none)
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, any)
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, any_x)
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, any_y)
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, any_z)
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, x)
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, y)
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, z)
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, neg_x)
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, neg_y)
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, neg_z)
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, custom)
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, xy)
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, xz)
      NAIADES_ENUM_TO_STRING_CASE(element_orientation_bits, yz)
    }
    return m;
  }
};

template <> struct DebugTraits<naiades::core::Element::Type> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::core::Element::Type &data) {
    using namespace naiades::core;
    auto m = DebugMessage();
    switch (data) {
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, NONE)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, ANY)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, CELL)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, FACE)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, VERTEX)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, POINT)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, CUSTOM)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, BOUNDARY_CELL)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, BOUNDARY_FACE)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, BOUNDARY_VERTEX)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, INTERIOR_CELL)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, INTERIOR_FACE)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, INTERIOR_VERTEX)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, BOUNDARY_HORIZONTAL_FACE)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, BOUNDARY_VERTICAL_FACE)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, XZ_FACE)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, YZ_FACE)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, XY_FACE)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, RIGHT_FACE)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, LEFT_FACE)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, BACK_FACE)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, FRONT_FACE)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, UP_FACE)
      NAIADES_ENUM_TO_STRING_CASE(Element::Type, DOWN_FACE)
    }
    return m;
  }
};

template <> struct DebugTraits<naiades::core::Element> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::core::Element &data) {
    return DebugMessage().addFmt("E({},{},{},{})",
                                 hermes::to_string(data.primitives()),
                                 hermes::to_string(data.alignments()),
                                 hermes::to_string(data.orientations()),
                                 hermes::to_string(data.spaces()));
  }
};

template <> struct DebugTraits<naiades::core::ElementIndex> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::core::ElementIndex &data) {
    auto m = DebugMessage();
    m.addFmt("EI({}, {})", hermes::to_string(data.index),
             hermes::to_string(data.element));
    return m;
  }
};

#undef NAIADES_ENUM_TO_STRING_CASE
#undef NAIADES_ENUM_TO_STRING_APPEND

} // namespace hermes

#endif
