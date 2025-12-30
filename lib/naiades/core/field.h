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
#include <naiades/core/spatial_discretization.h>

#include <hermes/geometry/vector.h>
#include <hermes/storage/aos.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace naiades::core {

template <typename T> class Field : public hermes::mem::AoS::FieldView<T> {
public:
  Field(const hermes::mem::AoS::FieldView<T> &field_view)
      : hermes::mem::AoS::FieldView<T>(field_view) {}

  Element element() const { return element_; }

private:
  friend class FieldGroup;

  Element element_;
};

template <typename T>
class Field_RO : public hermes::mem::AoS::ConstFieldView<T> {
public:
  Field_RO(const hermes::mem::AoS::ConstFieldView<T> &field_view)
      : hermes::mem::AoS::ConstFieldView<T>(field_view) {}
  Field_RO(const Field<T> &field)
      : hermes::mem::AoS::ConstFieldView<T>(
            static_cast<hermes::mem::AoS::ConstFieldView<T>>(field)),
        //    field.data_, field.stride_, field.offset_, field.size_),
        element_(field.element()) {}

  Element element() const { return element_; }

private:
  friend class FieldGroup;

  Element element_;
};

/// A field group holds one or more fields defined over a single type of
/// discrete location (ex: vertex and face centers).
/// \note The values are stored in an array of structs.
class FieldGroup : public hermes::mem::AoS {
public:
  void setElement(Element loc);
  Element element() const;

  template <typename T> Field<T> get(h_size field_index) {
    Field<T> acc(field<T>(field_index));
    acc.element_ = element_;
    return acc;
  }

  template <typename T> Field_RO<T> get(h_size field_index) const {
    Field_RO<T> acc(field<T>(field_index));
    acc.element_ = element_;
    return acc;
  }

private:
  friend class FieldSet;

  Element element_{Element::Type::NONE};
  NAIADES_to_string_FRIEND(FieldGroup);
};

class FieldSet {
public:
  NaResult setElementCount(Element loc, h_size count);
  NaResult setElementCountFrom(SpatialDiscretization2 *sd);

  template <typename T>
  NaResult add(Element loc, const std::vector<std::string> &field_names) {
    for (const auto &field_name : field_names) {
      FieldGroup field_group;
      field_group.setElement(loc);
      field_group.pushField<T>("value");
      NAIADES_HE_RETURN_BAD_RESULT(field_group.resize(field_sizes_[loc]));
      if (fields_.count(field_name))
        HERMES_WARN("Overwriting field {} in field set.", field_name);
      fields_[field_name] = std::move(field_group);
    }
    return NaResult::noError();
  }

  template <typename T> Result<Field<T>> get(const std::string &name) {
    auto it = fields_.find(name);
    if (it != fields_.end())
      return Result<Field<T>>(it->second.get<T>(0));
    return NaResult::notFound();
  }

  template <typename T> Result<Field_RO<T>> get(const std::string &name) const {
    auto it = fields_.find(name);
    if (it != fields_.end())
      return Result<Field_RO<T>>(it->second.get<T>(0));
    return NaResult::notFound();
  }

private:
  std::unordered_map<Element, h_size> field_sizes_;
  std::unordered_map<std::string, FieldGroup> fields_;

  NAIADES_to_string_FRIEND(FieldSet);
};

} // namespace naiades::core

namespace naiades {

HERMES_TO_STRING_TEMPLATED_METHOD_BEGIN(core::Field<T>, typename T)
HERMES_TO_STRING_METHOD_TITLE
HERMES_TO_STRING_METHOD_LINE("loc: {}\n", naiades::to_string(object.element()));
HERMES_TO_STRING_METHOD_LINE("size: {}\n", object.size());
HERMES_TO_STRING_METHOD_LINE("values: {}",
                             hermes::cstr::join(object, ", ", 10));
HERMES_TO_STRING_METHOD_END

} // namespace naiades
