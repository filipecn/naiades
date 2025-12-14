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

/// \file   field.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07

#include <naiades/core/field.h>

#include <naiades/base/debug.h>

namespace naiades {

HERMES_TO_STRING_DEBUG_METHOD_BEGIN(core::FieldSet)
HERMES_PUSH_DEBUG_TITLE
HERMES_PUSH_DEBUG_LINE("scalar fields [{}]", object.scalar_fields_.size());
HERMES_PUSH_DEBUG_MAP_FIELD_BEGIN(scalar_fields_, name, field)
HERMES_PUSH_DEBUG_LINE("name: {}", name);
HERMES_PUSH_DEBUG_LINE("{}", to_string(field));
HERMES_PUSH_DEBUG_MAP_FIELD_END
HERMES_TO_STRING_DEBUG_METHOD_END

} // namespace naiades

namespace naiades::core {

NaResult
FieldSet::addScalarFields(Element loc,
                          const std::vector<std::string> &field_names) {
  for (const auto &name : field_names)
    NAIADES_RETURN_BAD_RESULT(addScalarField(name, loc));
  return NaResult::noError();
}

NaResult
FieldSet::addVectorFields(Element loc,
                          const std::vector<std::string> &field_names) {
  for (const auto &name : field_names)
    NAIADES_RETURN_BAD_RESULT(addVectorField(name, loc));
  return NaResult::noError();
}

NaResult FieldSet::addScalarField(const std::string &name, Element loc) {
  auto it = scalar_fields_.find(name);
  if (it != scalar_fields_.end())
    return NaResult::checkError();
  Field<float> field;
  field.setElement(loc);
  // TODO check resize error
  field.resize(field_sizes_[loc]);
  scalar_fields_[name].setElement(loc);
  return NaResult::noError();
}

NaResult FieldSet::addVectorField(const std::string &name, Element loc) {
  auto it = vector_fields_.find(name);
  if (it != vector_fields_.end())
    return NaResult::checkError();
  Field<hermes::geo::vec2> field;
  field.setElement(loc);
  // TODO check resize error
  field.resize(field_sizes_[loc]);
  vector_fields_[name].setElement(loc);
  return NaResult::noError();
}

NaResult FieldSet::setElementCount(Element loc, h_size count) {
  for (auto &item : scalar_fields_) {
    // TODO check resize error
    if (item.second.element() == loc)
      item.second.resize(count);
  }
  for (auto &item : vector_fields_) {
    // TODO check resize error
    if (item.second.element() == loc)
      item.second.resize(count);
  }
  return NaResult::noError();
}

Field<f32> *FieldSet::scalarField(const std::string &name) {
  auto it = scalar_fields_.find(name);
  if (it != scalar_fields_.end())
    return &(it->second);
  return nullptr;
}

Field<hermes::geo::vec2> *FieldSet::vectorField(const std::string &name) {
  auto it = vector_fields_.find(name);
  if (it != vector_fields_.end())
    return &(it->second);
  return nullptr;
}

} // namespace naiades::core
