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

#include <naiades/base/debug.h>

#include <hermes/base/flags.h>
#include <hermes/core/types.h>

#define NAIADES_ELEMENT_MASK(PRIMITIVE, ALIGNMENT, orientation)                \
  (static_cast<u32>(PRIMITIVE) | (static_cast<u32>(ALIGNMENT) << 8) |          \
   (static_cast<u32>(orientation) << 16))

#define NAIADES_ELEMENT_MASK_SET_PRIMTIVE(M, P) (M | (static_cast<u32>(P)))

#define NAIADES_ELEMENT_MASK_SET_ALIGNMENT(M, A)                               \
  (M | (static_cast<u32>(A) << 8))

#define NAIADES_ELEMENT_MASK_SET_ORIENTATION(M, D)                             \
  (M | (static_cast<u32>(D) << 16))

#define NAIADES_ELEMENT_MASK_GET_PRIMITIVE(M) (M & 0xff)

#define NAIADES_ELEMENT_MASK_GET_ALIGNMENT(M) ((M >> 8) & 0xff)

#define NAIADES_ELEMENT_MASK_GET_ORIENTATION(M) (M >> 16)

namespace naiades::core {

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

using element_primitives = hermes::Flags<core::element_primitive_bits>;
using element_alignments = hermes::Flags<core::element_alignment_bits>;
using element_orientations = hermes::Flags<core::element_orientation_bits>;

} // namespace naiades::core

namespace hermes {

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

enum class IndexSpace { GLOBAL, LOCAL, CUSTOM };

/// The element index holds the unique id of an element within a given space.
struct Index {
  static Index local(h_size value) { return {value, IndexSpace::LOCAL}; }
  static Index global(h_size value) { return {value, IndexSpace::GLOBAL}; }
  static Index invalid() { return {s_invalid_value_, IndexSpace::LOCAL}; }

  Index() : space_{IndexSpace::GLOBAL}, value_{s_invalid_value_} {}
  Index(h_size value, IndexSpace ctx) : space_{ctx}, value_{value} {}

  Index &operator=(h_size i) {
    value_ = i;
    return *this;
  }

  bool operator==(const Index &rhs) const {
    return space_ == rhs.space_ && value_ == rhs.value_;
  }

  operator bool() const { return isValid(); };

  h_size operator*() const { return value_; };
  bool isValid() const { return value_ != s_invalid_value_; }
  bool isLocal() const { return space_ == IndexSpace::LOCAL; }
  bool isGlobal() const { return space_ == IndexSpace::GLOBAL; }

  IndexSpace space() const { return space_; }

private:
  IndexSpace space_{IndexSpace::GLOBAL};
  h_size value_;
  static h_size s_invalid_value_;

  NAIADES_to_string_FRIEND(Index);
};

class Element {
public:
  ///      v --- V ---- v    v - VERTEX
  ///      |            |    C - CELL
  ///      U     C      U    V - [V|X|HORIZONTAL]_FACE
  ///      |            |    U - [U|Y|VERTICAL]_FACE
  ///      v --- V ---- v
  enum Type : u32 {
    NONE = 0,
    CELL = NAIADES_ELEMENT_MASK(element_primitive_bits::cell,
                                element_alignment_bits::none,
                                element_orientation_bits::none),
    FACE = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                element_alignment_bits::any,
                                element_orientation_bits::any),
    VERTEX = NAIADES_ELEMENT_MASK(element_primitive_bits::vertex,
                                  element_alignment_bits::none,
                                  element_orientation_bits::none),
    POINT = NAIADES_ELEMENT_MASK(element_primitive_bits::point,
                                 element_alignment_bits::none,
                                 element_orientation_bits::none),
    CUSTOM = NAIADES_ELEMENT_MASK(element_primitive_bits::custom,
                                  element_alignment_bits::custom,
                                  element_orientation_bits::custom),

    HORIZONTAL_FACE = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                           element_alignment_bits::xz,
                                           element_orientation_bits::any_y),
    V_FACE = HORIZONTAL_FACE,
    X_FACE = HORIZONTAL_FACE,
    XZ_FACE = HORIZONTAL_FACE,
    UP_FACE = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                   element_alignment_bits::xz,
                                   element_orientation_bits::y),
    DOWN_FACE = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                     element_alignment_bits::xz,
                                     element_orientation_bits::neg_y),

    VERTICAL_FACE = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                         element_alignment_bits::yz,
                                         element_orientation_bits::any_x),
    U_FACE = VERTICAL_FACE,
    Y_FACE = VERTICAL_FACE,
    YZ_FACE = VERTICAL_FACE,
    LEFT_FACE = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                     element_alignment_bits::yz,
                                     element_orientation_bits::neg_x),
    RIGHT_FACE = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                      element_alignment_bits::yz,
                                      element_orientation_bits::x),

    DEPTH_FACE = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                      element_alignment_bits::xy,
                                      element_orientation_bits::any_z),
    W_FACE = DEPTH_FACE,
    Z_FACE = DEPTH_FACE,
    XY_FACE = DEPTH_FACE,
    FRONT_FACE = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                      element_alignment_bits::xy,
                                      element_orientation_bits::z),
    BACK_FACE = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                     element_alignment_bits::xy,
                                     element_orientation_bits::neg_z),

    ANY = NAIADES_ELEMENT_MASK(element_primitive_bits::any,
                               element_alignment_bits::any,
                               element_orientation_bits::any),
  };

  Element() noexcept = default;
  Element(element_primitives prim,
          element_alignments a = element_alignment_bits::none,
          element_orientations d = element_orientation_bits::none) noexcept
      : mask_(NAIADES_ELEMENT_MASK(prim, a, d)) {}
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
  inline Element &addorientations(element_orientations d) {
    auto mask_d = orientations();
    mask_ = NAIADES_ELEMENT_MASK_SET_ORIENTATION(mask_, mask_d | d);
    return *this;
  }
  inline Element &setPrimitives(element_primitives prim) {
    mask_ &= ~0xff;
    mask_ = NAIADES_ELEMENT_MASK_SET_PRIMTIVE(mask_, prim);
    return *this;
  }
  inline Element &setAlignments(element_alignments a) {
    mask_ &= 0xff00ff;
    mask_ = NAIADES_ELEMENT_MASK_SET_ALIGNMENT(mask_, a);
    return *this;
  }
  inline Element &setOrientations(element_orientations d) {
    mask_ &= 0x00ffff;
    mask_ = NAIADES_ELEMENT_MASK_SET_ORIENTATION(mask_, d);
    return *this;
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
  inline bool is(element_primitives prim) const {
    auto p = NAIADES_ELEMENT_MASK_GET_PRIMITIVE(mask_);
    return (p & static_cast<u32>(prim)) == static_cast<u32>(prim);
  }
  inline bool has(element_alignments a) const {
    auto _a = NAIADES_ELEMENT_MASK_GET_ALIGNMENT(mask_);
    return (_a & static_cast<u32>(a)) == static_cast<u32>(a);
  }
  inline bool has(element_orientations d) const {
    auto _d = NAIADES_ELEMENT_MASK_GET_ORIENTATION(mask_);
    return (_d & static_cast<u32>(d)) == static_cast<u32>(d);
  }

private:
  u32 mask_{};

  NAIADES_to_string_FRIEND(Element);
  friend class std::hash<naiades::core::Element>;
};

struct ElementIndex {
  static ElementIndex global(Element loc, h_size i);
  Index index{Index::invalid()};
  Element element{Element::Type::ANY};

  NAIADES_to_string_FRIEND(ElementIndex);
};

#undef NAIADES_ELEMENT_MASK
#undef NAIADES_ELEMENT_MASK_SET_PRIMTIVE
#undef NAIADES_ELEMENT_MASK_SET_ALIGNMENT
#undef NAIADES_ELEMENT_MASK_SET_orientation
#undef NAIADES_ELEMENT_MASK_GET_PRIMITIVE
#undef NAIADES_ELEMENT_MASK_GET_ALIGNMENT
#undef NAIADES_ELEMENT_MASK_GET_orientation

} // namespace naiades::core

namespace naiades {

HERMES_DECLARE_TO_STRING_DEBUG_METHOD(core::IndexSpace)
HERMES_DECLARE_TO_STRING_DEBUG_METHOD(core::element_primitive_bits)
HERMES_DECLARE_TO_STRING_DEBUG_METHOD(core::element_alignment_bits)
HERMES_DECLARE_TO_STRING_DEBUG_METHOD(core::element_orientation_bits)
HERMES_DECLARE_TO_STRING_DEBUG_METHOD(core::element_primitives)
HERMES_DECLARE_TO_STRING_DEBUG_METHOD(core::element_alignments)
HERMES_DECLARE_TO_STRING_DEBUG_METHOD(core::element_orientations)
HERMES_DECLARE_TO_STRING_DEBUG_METHOD(core::Element::Type)
HERMES_DECLARE_TO_STRING_DEBUG_METHOD(core::Element)

} // namespace naiades

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

} // namespace std
