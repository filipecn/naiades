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

#include <naiades/base/result.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace naiades::core {

///      v --- V ---- v    v - VERTEX_CENTER
///      |            |    C - CELL_CENTER
///      U     C      U    V - [V|X|HORIZONTAL]_FACE_CENTER
///      |            |    U - [U|Y|VERTICAL]_FACE_CENTER
///      v --- V ---- v
enum FieldLocation : int {
  CELL_CENTER = 0,
  FACE_CENTER = 1,

  HORIZONTAL_FACE_CENTER = 2,
  V_FACE_CENTER = 3,
  X_FACE_CENTER = 4,

  VERTICAL_FACE_CENTER = 5,
  U_FACE_CENTER = 6,
  Y_FACE_CENTER = 7,

  DEPTH_FACE_CENTER = 8,
  W_FACE_CENTER = 9,
  Z_FACE_CENTER = 10,

  VERTEX_CENTER = 11,
  POINT = 12,
  CUSTOM = 13,
  SIZE = 14
};

template <typename DataType> class Field : public std::vector<DataType> {
public:
  void setLocation(FieldLocation loc) { location_ = loc; }
  FieldLocation location() const { return location_; }

private:
  FieldLocation location_{FieldLocation::CUSTOM};
};

class FieldSet {
public:
  NaResult addScalarField(const std::string &name, FieldLocation loc);
  NaResult setLocationCount(FieldLocation loc, h_size count);

  Field<f32> *scalarField(const std::string &name);

private:
  h_size field_sizes_[FieldLocation::SIZE]{};
  std::unordered_map<std::string, Field<float>> scalar_fields_;
};

} // namespace naiades::core
