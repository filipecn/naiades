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

HERMES_TO_STRING_METHOD_BEGIN(core::Boundary::Region)
HERMES_TO_STRING_METHOD_NAIADES_FIELD(index_set_);
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(core::Boundary)
HERMES_TO_STRING_METHOD_TITLE
HERMES_TO_STRING_METHOD_LINE("regions [{}]", object.regions_.size());
HERMES_TO_STRING_METHOD_ARRAY_FIELD_BEGIN(regions_, region)
HERMES_TO_STRING_METHOD_LINE("{}\n", naiades::to_string(region));
HERMES_TO_STRING_METHOD_MAP_FIELD_END
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(core::BoundarySet)
HERMES_TO_STRING_METHOD_TITLE
HERMES_TO_STRING_METHOD_MAP_FIELD_BEGIN(boundaries_, name, boundary)
HERMES_TO_STRING_METHOD_LINE("{}", naiades::to_string(boundary));
HERMES_TO_STRING_METHOD_MAP_FIELD_END
HERMES_TO_STRING_METHOD_END

} // namespace naiades

namespace naiades::core {

void Boundary::Region::setIndices(const std::vector<h_size> &indices) {
  index_set_.set(indices);
}

void Boundary::Region::setCondition(bc::BoundaryCondition::Ptr condition) {
  condition_ = condition;
}

Boundary::Region::Region(const std::vector<h_size> &indices) {
  setIndices(indices);
}

f32 Boundary::compute(h_size boundary_index, h_size interior_index,
                      FieldRef<f32> field) const {
  auto op = resolve(boundary_index, interior_index);
  return op(field);
}

bool Boundary::Region::contains(const Index &index) const {
  return index_set_.contains(index);
}

DiscreteOperator Boundary::Region::resolve(h_size boundary_index,
                                           h_size interior_index) const {
  return condition_->resolve(boundary_index, interior_index);
}

h_size Boundary::addRegion(const std::vector<h_size> &indices) {
  h_size new_region_index = regions_.size();
  regions_.emplace_back();
  regions_.back().setIndices(indices);
  return new_region_index;
}

void Boundary::setCondition(h_size region_index,
                            bc::BoundaryCondition::Ptr condition) {
  HERMES_ASSERT(region_index < regions_.size());
  regions_[region_index].setCondition(condition);
}

void Boundary::setCondition(bc::BoundaryCondition::Ptr condition) {
  for (h_size i = 0; i < regions_.size(); ++i)
    setCondition(i, condition);
}

DiscreteOperator Boundary::resolve(h_size boundary_index,
                                   h_size interior_index) const {
  for (const auto &region : regions_)
    if (region.contains(Index::global(boundary_index)))
      return region.resolve(boundary_index, interior_index);
  // HERMES_ASSERT(false);
  return {};
}

void Boundary::Region::compute(FieldCRef<f32> interior_field,
                               FieldRef<f32> field) {
  if (stencils_.size() != index_set_.size())
    stencils_.resize(index_set_.size());
  // compute the stencil for each boundary index
}

h_size BoundarySet::addRegion(const std::string &field_name,
                              const std::vector<h_size> &indices) {
  return boundaries_[field_name].addRegion(indices);
}

void BoundarySet::set(const std::string &field_name, h_size region_index,
                      bc::BoundaryCondition::Ptr condition) {
  boundaries_[field_name].setCondition(region_index, condition);
}

void BoundarySet::set(const std::string &field_name,
                      bc::BoundaryCondition::Ptr condition) {
  boundaries_[field_name].setCondition(condition);
}

const Boundary &BoundarySet::operator[](const std::string &field_name) const {
  static Boundary s_dummy;
  auto it = boundaries_.find(field_name);
  if (it == boundaries_.end())
    return s_dummy;
  return it->second;
}

} // namespace naiades::core
