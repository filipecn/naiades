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

#define NAIADES_ELEMENT_MASK(PRIMITIVE, ALIGNMENT)                             \
  (static_cast<u32>(PRIMITIVE) | (static_cast<u32>(ALIGNMENT) << 16))

#define NAIADES_ELEMENT_MASK_SET_PRIMTIVE(M, P) (M | (static_cast<u32>(P)))

#define NAIADES_ELEMENT_MASK_SET_ALIGNMENT(M, A)                               \
  (M | (static_cast<u32>(A) << 16))

#define NAIADES_ELEMENT_MASK_GET_PRIMITIVE(M) (M & 0xff)

#define NAIADES_ELEMENT_MASK_GET_ALIGNMENT(M) (M >> 16)

namespace naiades::core {

enum class element_primitive_bits : u32 {
  any = 0,
  vertex = 1 << 0,
  face = 1 << 1,
  cell = 1 << 2,
  particle = 1 << 3,
  point = 1 << 4,
  custom = 1 << 5
};

enum class element_alignment_bits : u32 {
  any = 0,
  x = 1 << 0,
  y = 1 << 1,
  z = 1 << 2,
  custom = 1 << 4,
  xy = x | y,
  xz = x | z,
  yz = y | z,
};

using element_primitives = hermes::Flags<core::element_primitive_bits>;
using element_alignments = hermes::Flags<core::element_alignment_bits>;

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
      naiades::core::element_alignment_bits::custom |
      naiades::core::element_alignment_bits::xy |
      naiades::core::element_alignment_bits::xz |
      naiades::core::element_alignment_bits::yz;
};

} // namespace hermes

namespace naiades::core {

class Element {
public:
  ///      v --- V ---- v    v - VERTEX_CENTER
  ///      |            |    C - CELL_CENTER
  ///      U     C      U    V - [V|X|HORIZONTAL]_FACE_CENTER
  ///      |            |    U - [U|Y|VERTICAL]_FACE_CENTER
  ///      v --- V ---- v
  enum Type : u32 {
    ANY = 0,
    CELL_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::cell,
                                       element_alignment_bits::any),
    FACE_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                       element_alignment_bits::any),
    VERTEX_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::vertex,
                                         element_alignment_bits::any),
    POINT = NAIADES_ELEMENT_MASK(element_primitive_bits::point,
                                 element_alignment_bits::any),
    CUSTOM = NAIADES_ELEMENT_MASK(element_primitive_bits::custom,
                                  element_alignment_bits::custom),

    HORIZONTAL_FACE_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                                  element_alignment_bits::xz),
    V_FACE_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                         element_alignment_bits::xz),
    X_FACE_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                         element_alignment_bits::xz),
    XZ_FACE_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                          element_alignment_bits::xz),

    VERTICAL_FACE_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                                element_alignment_bits::yz),
    U_FACE_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                         element_alignment_bits::yz),
    Y_FACE_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                         element_alignment_bits::yz),
    YZ_FACE_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                          element_alignment_bits::yz),

    DEPTH_FACE_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                             element_alignment_bits::xy),
    W_FACE_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                         element_alignment_bits::xy),
    Z_FACE_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                         element_alignment_bits::xy),
    XY_FACE_CENTER = NAIADES_ELEMENT_MASK(element_primitive_bits::face,
                                          element_alignment_bits::xy)
  };

  Element() noexcept = default;
  Element(element_primitives prim,
          element_alignments a = element_alignment_bits::any) noexcept
      : mask_(NAIADES_ELEMENT_MASK(prim, a)) {}
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
  inline Element &setPrimitives(element_primitives prim) {
    mask_ &= ~0xff;
    mask_ = NAIADES_ELEMENT_MASK_SET_PRIMTIVE(mask_, prim);
    return *this;
  }
  inline Element &setAlignments(element_alignments a) {
    mask_ &= 0xff;
    mask_ = NAIADES_ELEMENT_MASK_SET_ALIGNMENT(mask_, a);
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
  inline bool is(element_primitives prim) const {
    auto p = NAIADES_ELEMENT_MASK_GET_PRIMITIVE(mask_);
    return (p & static_cast<u32>(prim)) == static_cast<u32>(prim);
  }
  inline bool has(element_alignments a) const {
    auto _a = NAIADES_ELEMENT_MASK_GET_ALIGNMENT(mask_);
    return (_a & static_cast<u32>(a)) == static_cast<u32>(a);
  }

private:
  u32 mask_{};

  NAIADES_to_string_FRIEND(Element);
  friend class std::hash<naiades::core::Element>;
};

#undef NAIADES_ELEMENT_MASK
#undef NAIADES_ELEMENT_MASK_SET_PRIMTIVE
#undef NAIADES_ELEMENT_MASK_SET_ALIGNMENT
#undef NAIADES_ELEMENT_MASK_GET_PRIMITIVE
#undef NAIADES_ELEMENT_MASK_GET_ALIGNMENT

} // namespace naiades::core

namespace naiades {

HERMES_DECLARE_TO_STRING_DEBUG_METHOD(core::element_primitive_bits)
HERMES_DECLARE_TO_STRING_DEBUG_METHOD(core::element_alignment_bits)
HERMES_DECLARE_TO_STRING_DEBUG_METHOD(core::element_primitives)
HERMES_DECLARE_TO_STRING_DEBUG_METHOD(core::element_alignments)
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

} // namespace std
