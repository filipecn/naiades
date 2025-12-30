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

/// \file   boundary.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07

#include <naiades/core/boundary.h>

namespace naiades {

HERMES_TO_STRING_METHOD_BEGIN(core::IndexInterval)
HERMES_TO_STRING_METHOD_LINE("[{}, {})", object.start, object.end);
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(core::Boundary::Region)
HERMES_TO_STRING_METHOD_LINE(
    "values: {}\n",
    std::visit(
        core::IndexSetOverloaded{
            [](std::monostate s) { return std::string(); },
            [](const std::vector<h_size> &indices) -> std::string {
              return hermes::cstr::join(indices, ", ", 10);
            },
            [](const std::vector<core::IndexInterval> &indices) -> std::string {
              auto f = [](const core::IndexInterval &interval) -> std::string {
                return naiades::to_string(interval);
              };
              return hermes::cstr::join<std::vector<core::IndexInterval>,
                                        core::IndexInterval>(indices, f, ", ",
                                                             10);
            }},
        object.index_set_));
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(core::Boundary)
HERMES_TO_STRING_METHOD_TITLE
HERMES_TO_STRING_METHOD_LINE("regions [{}]", object.regions_.size());
HERMES_TO_STRING_METHOD_ARRAY_FIELD_BEGIN(regions_, region)
HERMES_TO_STRING_METHOD_LINE("{}\n", naiades::to_string(region));
HERMES_TO_STRING_METHOD_MAP_FIELD_END
HERMES_TO_STRING_METHOD_END

} // namespace naiades

namespace naiades::core {

void Boundary::Region::setCondition(const std::string &field_name,
                                    bc::Condition<f32>::Ptr condition) {}

h_size Boundary::setRegion(const std::vector<h_size> &indices) {
  if (indices.empty())
    return NaResult::inputError();
  std::vector<IndexInterval> intervals;
  h_size last_index = indices.front();
  IndexInterval interval;
  interval.start = last_index;
  interval.end = last_index + 1;
  for (auto index : indices) {
    if (index > last_index + 1) {
      interval.end = last_index + 1;
      intervals.emplace_back(interval);
      interval.start = index;
      interval.end = index + 1;
    }
    last_index = index;
  }
  interval.end = last_index + 1;
  intervals.emplace_back(interval);

  regions_.emplace_back();
  regions_.back().index_set_ = intervals;
  return NaResult::noError();
}

void Boundary::set(h_size region_index, const std::string &field_name,
                   bc::Condition<f32>::Ptr condition) {
  HERMES_ASSERT(region_index < regions_.size());
  regions_[region_index].setCondition(field_name, condition);
}

void Boundary::set(const std::string &field_name,
                   bc::Condition<f32>::Ptr condition) {
  for (auto &region : regions_)
    region.setCondition(field_name, condition);
}

} // namespace naiades::core
