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

#include <naiades/numeric/boundary.h>

namespace naiades {

HERMES_TO_STRING_METHOD_BEGIN(numeric::Boundary::Region)
HERMES_TO_STRING_METHOD_NAIADES_FIELD(index_set_);
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(numeric::Boundary)
HERMES_TO_STRING_METHOD_TITLE
HERMES_TO_STRING_METHOD_LINE("regions [{}]", object.regions_.size());
HERMES_TO_STRING_METHOD_ARRAY_FIELD_BEGIN(regions_, region)
HERMES_TO_STRING_METHOD_LINE("{}\n", naiades::to_string(region));
HERMES_TO_STRING_METHOD_MAP_FIELD_END
HERMES_TO_STRING_METHOD_END

} // namespace naiades

namespace naiades::numeric {

Boundary::Region::Region(core::Element element_type,
                         const std::vector<h_size> &indices)
    : boundary_element_type_{element_type},
      interior_element_type_{element_type} {
  index_set_.set(indices);
}

void Boundary::Region::setCondition(bc::BoundaryCondition::Ptr condition,
                                    core::Element interior_field_loc) {
  condition_ = condition;
  interior_element_type_ = interior_field_loc;
}

bool Boundary::Region::contains(const core::Index &index) const {
  return index_set_.contains(index);
}

NaResult Boundary::Region::resolve(core::Topology::Ptr topology) {
  if (!(bool)condition_)
    return NaResult::checkError();
  stencils_.resize(index_set_.size());
  for (auto it : index_set_) {
    auto boundary_element =
        core::ElementIndex::global(boundary_element_type_, it.index);
    auto interior_index =
        topology->interiorNeighbour(boundary_element, interior_element_type_);
    auto interior_element =
        core::ElementIndex::global(interior_element_type_, interior_index);
    stencils_[it.flat_index] =
        condition_->resolve(boundary_element, interior_element);
  }
  return NaResult::noError();
}

NaResult Boundary::compute(core::FieldCRef<f32> interior_field,
                           core::FieldRef<f32> boundary_field) const {
  for (const auto &region : regions_)
    NAIADES_RETURN_BAD_RESULT(region.compute(interior_field, boundary_field));
  return NaResult::noError();
}

const DiscreteOperator &
Boundary::Region::stencil(const core::Index &index) const {
  if (stencils_.empty()) {
    HERMES_ERROR(
        "Accessing stencil in boundary but boundary regions is not resolved.");
    static DiscreteOperator dop;
    return dop;
  }
  if (index.isLocal()) {
    HERMES_ASSERT(*index < stencils_.size());
    return stencils_[*index];
  }
  auto local_index = index_set_.seqIndex(*index);
  HERMES_ASSERT(local_index < stencils_.size());
  return stencils_[*local_index];
}

Boundary &Boundary::addRegion(core::Element loc,
                              const std::vector<h_size> &indices,
                              h_size *region_index) {
  if (region_index)
    *region_index = regions_.size();
  regions_.emplace_back(loc, indices);
  return *this;
}

Boundary &Boundary::setCondition(h_size region_index,
                                 bc::BoundaryCondition::Ptr condition,
                                 core::Element interior_field_loc) {
  HERMES_ASSERT(region_index < regions_.size());
  regions_[region_index].setCondition(condition, interior_field_loc);
  return *this;
}

Boundary &Boundary::setCondition(bc::BoundaryCondition::Ptr condition,
                                 core::Element interior_field_loc) {
  for (h_size i = 0; i < regions_.size(); ++i)
    setCondition(i, condition, interior_field_loc);
  return *this;
}

NaResult Boundary::resolve(core::Topology::Ptr topology) {
  for (auto &region : regions_)
    NAIADES_RETURN_BAD_RESULT(region.resolve(topology));
  return NaResult::noError();
}

const DiscreteOperator &Boundary::stencil(const core::Index &index) const {
  for (const auto &region : regions_)
    if (region.contains(index))
      return region.stencil(index);
  HERMES_WARN("Index {} not found in boundary.", naiades::to_string(index));
  static DiscreteOperator s_dop;
  return s_dop;
}

NaResult Boundary::Region::compute(core::FieldCRef<f32> interior_field,
                                   core::FieldRef<f32> field) const {
  if (stencils_.size() < index_set_.size()) {
    HERMES_ERROR("Boundary region not resolved before compute!");
    return NaResult::checkError();
  }
  for (auto i : index_set_)
    field.at(core::Index::global(i.index)) =
        stencils_[i.flat_index](interior_field);
  return NaResult::noError();
}

const utils::IndexSet &Boundary::Region::indices() const { return index_set_; }

} // namespace naiades::numeric
