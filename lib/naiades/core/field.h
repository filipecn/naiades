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

/// \file   field.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Simulation field.

#pragma once

#include <naiades/base/debug.h>
#include <naiades/base/result.h>
#include <naiades/core/element.h>

#include <hermes/geometry/vector.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace naiades::core {

template <typename DataType> class Field : public std::vector<DataType> {
public:
  void setElement(Element element) { element_ = element; }
  Element element() const { return element_; }

private:
  Element element_{Element::Type::ANY};
  template <typename FriendDataType>
  NAIADES_to_string_FRIEND(Field<FriendDataType>);
};

class FieldSet {
public:
  NaResult addScalarFields(Element loc,
                           const std::vector<std::string> &field_names);
  NaResult addVectorFields(Element loc,
                           const std::vector<std::string> &field_names);
  NaResult addScalarField(const std::string &name, Element loc);
  NaResult addVectorField(const std::string &name, Element loc);
  NaResult setElementCount(Element loc, h_size count);

  Field<f32> *scalarField(const std::string &name);
  Field<hermes::geo::vec2> *vectorField(const std::string &name);

private:
  std::unordered_map<Element, h_size> field_sizes_;
  std::unordered_map<std::string, Field<f32>> scalar_fields_;
  std::unordered_map<std::string, Field<hermes::geo::vec2>> vector_fields_;
  NAIADES_to_string_FRIEND(FieldSet);
};

} // namespace naiades::core

namespace naiades {

HERMES_TO_STRING_DEBUG_TEMPLATED_METHOD_BEGIN(core::Field<T>, typename T)
HERMES_PUSH_DEBUG_TITLE
HERMES_PUSH_DEBUG_NAIADES_FIELD(element_);
HERMES_PUSH_DEBUG_LINE("size: {}\n", object.size());
HERMES_PUSH_DEBUG_LINE("values: {}", hermes::cstr::join(object, ", ", 10));
HERMES_TO_STRING_DEBUG_METHOD_END

} // namespace naiades
