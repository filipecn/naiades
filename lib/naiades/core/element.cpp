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

#include <naiades/core/element.h>

namespace naiades {

#define NAIADES_ENUM_TO_STRING_APPEND(TYPE, NAME)                              \
  if (object.contain(TYPE::NAME))                                              \
    ss.emplace_back(#NAME);

#define NAIADES_ENUM_TO_STRING_CASE(TYPE, NAME)                                \
  case TYPE::NAME: {                                                           \
    s = #NAME;                                                                 \
    break;                                                                     \
  }

HERMES_TO_STRING_METHOD_BEGIN(core::IndexSpace)
std::string s;
switch (object) {
  NAIADES_ENUM_TO_STRING_CASE(core::IndexSpace, GLOBAL)
  NAIADES_ENUM_TO_STRING_CASE(core::IndexSpace, LOCAL)
  NAIADES_ENUM_TO_STRING_CASE(core::IndexSpace, CUSTOM)
}
HERMES_TO_STRING_METHOD_LINE(s.c_str())
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(core::Index)
if (object) {
  HERMES_TO_STRING_METHOD_LINE("{}", *object);
} else {
  HERMES_TO_STRING_METHOD_LINE("[invalid - ]", *object,
                               naiades::to_string(object.space()));
}
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(core::element_primitives)
std::vector<std::string> ss;
NAIADES_ENUM_TO_STRING_APPEND(core::element_primitive_bits, any)
NAIADES_ENUM_TO_STRING_APPEND(core::element_primitive_bits, vertex)
NAIADES_ENUM_TO_STRING_APPEND(core::element_primitive_bits, face)
NAIADES_ENUM_TO_STRING_APPEND(core::element_primitive_bits, cell)
NAIADES_ENUM_TO_STRING_APPEND(core::element_primitive_bits, particle)
NAIADES_ENUM_TO_STRING_APPEND(core::element_primitive_bits, point)
NAIADES_ENUM_TO_STRING_APPEND(core::element_primitive_bits, custom)
if (object == core::element_primitive_bits::none)
  ss.emplace_back("none");
HERMES_TO_STRING_METHOD_LINE("{}", hermes::cstr::join(ss, "|"))
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(core::element_alignments)
std::vector<std::string> ss;
NAIADES_ENUM_TO_STRING_APPEND(core::element_alignment_bits, any)
NAIADES_ENUM_TO_STRING_APPEND(core::element_alignment_bits, x)
NAIADES_ENUM_TO_STRING_APPEND(core::element_alignment_bits, y)
NAIADES_ENUM_TO_STRING_APPEND(core::element_alignment_bits, z)
NAIADES_ENUM_TO_STRING_APPEND(core::element_alignment_bits, custom)
NAIADES_ENUM_TO_STRING_APPEND(core::element_alignment_bits, xy)
NAIADES_ENUM_TO_STRING_APPEND(core::element_alignment_bits, xz)
NAIADES_ENUM_TO_STRING_APPEND(core::element_alignment_bits, yz)
if (object == core::element_alignment_bits::none)
  ss.emplace_back("none");
HERMES_TO_STRING_METHOD_LINE("{}", hermes::cstr::join(ss, "|"))
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(core::element_orientations)
std::vector<std::string> ss;
NAIADES_ENUM_TO_STRING_APPEND(core::element_orientation_bits, any)
NAIADES_ENUM_TO_STRING_APPEND(core::element_orientation_bits, x)
NAIADES_ENUM_TO_STRING_APPEND(core::element_orientation_bits, y)
NAIADES_ENUM_TO_STRING_APPEND(core::element_orientation_bits, z)
NAIADES_ENUM_TO_STRING_APPEND(core::element_orientation_bits, neg_x)
NAIADES_ENUM_TO_STRING_APPEND(core::element_orientation_bits, neg_y)
NAIADES_ENUM_TO_STRING_APPEND(core::element_orientation_bits, neg_z)
NAIADES_ENUM_TO_STRING_APPEND(core::element_orientation_bits, custom)
NAIADES_ENUM_TO_STRING_APPEND(core::element_orientation_bits, xy)
NAIADES_ENUM_TO_STRING_APPEND(core::element_orientation_bits, xz)
NAIADES_ENUM_TO_STRING_APPEND(core::element_orientation_bits, yz)
if (object == core::element_orientation_bits::none)
  ss.emplace_back("none");
HERMES_TO_STRING_METHOD_LINE("{}", hermes::cstr::join(ss, "|"))
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(core::element_primitive_bits)
std::string s;
switch (object) {
  NAIADES_ENUM_TO_STRING_CASE(core::element_primitive_bits, none)
  NAIADES_ENUM_TO_STRING_CASE(core::element_primitive_bits, any)
  NAIADES_ENUM_TO_STRING_CASE(core::element_primitive_bits, vertex)
  NAIADES_ENUM_TO_STRING_CASE(core::element_primitive_bits, face)
  NAIADES_ENUM_TO_STRING_CASE(core::element_primitive_bits, cell)
  NAIADES_ENUM_TO_STRING_CASE(core::element_primitive_bits, particle)
  NAIADES_ENUM_TO_STRING_CASE(core::element_primitive_bits, point)
  NAIADES_ENUM_TO_STRING_CASE(core::element_primitive_bits, custom)
}
HERMES_TO_STRING_METHOD_LINE(s.c_str())
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(core::element_alignment_bits)
std::string s;
switch (object) {
  NAIADES_ENUM_TO_STRING_CASE(core::element_alignment_bits, none)
  NAIADES_ENUM_TO_STRING_CASE(core::element_alignment_bits, any)
  NAIADES_ENUM_TO_STRING_CASE(core::element_alignment_bits, x)
  NAIADES_ENUM_TO_STRING_CASE(core::element_alignment_bits, y)
  NAIADES_ENUM_TO_STRING_CASE(core::element_alignment_bits, z)
  NAIADES_ENUM_TO_STRING_CASE(core::element_alignment_bits, custom)
  NAIADES_ENUM_TO_STRING_CASE(core::element_alignment_bits, xy)
  NAIADES_ENUM_TO_STRING_CASE(core::element_alignment_bits, xz)
  NAIADES_ENUM_TO_STRING_CASE(core::element_alignment_bits, yz)
}
HERMES_TO_STRING_METHOD_LINE(s.c_str())
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(core::element_orientation_bits)
std::string s;
switch (object) {
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, none)
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, any)
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, any_x)
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, any_y)
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, any_z)
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, x)
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, y)
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, z)
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, neg_x)
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, neg_y)
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, neg_z)
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, custom)
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, xy)
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, xz)
  NAIADES_ENUM_TO_STRING_CASE(core::element_orientation_bits, yz)
}
HERMES_TO_STRING_METHOD_LINE(s.c_str())
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(core::Element::Type)
std::string s;
switch (object) {
  NAIADES_ENUM_TO_STRING_CASE(core::Element::Type, NONE)
  NAIADES_ENUM_TO_STRING_CASE(core::Element::Type, ANY)
  NAIADES_ENUM_TO_STRING_CASE(core::Element::Type, CELL_CENTER)
  NAIADES_ENUM_TO_STRING_CASE(core::Element::Type, FACE_CENTER)
  NAIADES_ENUM_TO_STRING_CASE(core::Element::Type, VERTEX_CENTER)
  NAIADES_ENUM_TO_STRING_CASE(core::Element::Type, POINT)
  NAIADES_ENUM_TO_STRING_CASE(core::Element::Type, CUSTOM)
  NAIADES_ENUM_TO_STRING_CASE(core::Element::Type, XZ_FACE_CENTER)
  NAIADES_ENUM_TO_STRING_CASE(core::Element::Type, YZ_FACE_CENTER)
  NAIADES_ENUM_TO_STRING_CASE(core::Element::Type, XY_FACE_CENTER)
}
HERMES_TO_STRING_METHOD_LINE(s.c_str())
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(core::Element)
HERMES_TO_STRING_METHOD_LINE("[{},{},{}]",
                             naiades::to_string(object.primitives()),
                             naiades::to_string(object.alignments()),
                             naiades::to_string(object.orientations()));
HERMES_TO_STRING_METHOD_END

#undef NAIADES_ENUM_TO_STRING_CASE

} // namespace naiades

namespace naiades::core {

h_size Index::s_invalid_value_ = 1 << 30;

} // namespace naiades::core
