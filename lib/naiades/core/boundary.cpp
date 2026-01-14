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

Boundary::Region::Region(DiscretizationTopology::Ptr d_t, Element element_type,
                         const std::vector<h_size> &indices)
    : discretization_{d_t}, element_type_{element_type} {
  index_set_.set(indices);
}

void Boundary::Region::setCondition(bc::BoundaryCondition::Ptr condition) {
  condition_ = condition;
}

NaResult Boundary::compute(FieldCRef<f32> interior_field,
                           FieldRef<f32> boundary_field) const {
  for (const auto &region : regions_)
    NAIADES_RETURN_BAD_RESULT(region.compute(interior_field, boundary_field));
  return NaResult::noError();
}

bool Boundary::Region::contains(const Index &index) const {
  return index_set_.contains(index);
}

NaResult Boundary::Region::resolve() {
  if (!(bool)discretization_ || !(bool)condition_) {
    return NaResult::checkError();
  }
  stencils_.resize(index_set_.size());
  for (auto it : index_set_)
    stencils_[it.flat_index] = condition_->resolve(discretization_, it.index);
  return NaResult::noError();
}

const DiscreteOperator &Boundary::Region::stencil(const Index &index) const {
  if (stencils_.empty()) {
    HERMES_ERROR(
        "Accessing stencil in boundary but boundary regions is not resolved.");
    static DiscreteOperator dop;
    return dop;
  }
  if (index.space() == IndexSpace::LOCAL) {
    HERMES_ASSERT(*index < stencils_.size());
    return stencils_[*index];
  }
  auto local_index = index_set_.seqIndex(*index);
  HERMES_ASSERT(local_index < stencils_.size());
  return stencils_[local_index];
}

Boundary &Boundary::set(DiscretizationTopology::Ptr d_t, Element element_type) {
  discretization_ = d_t;
  element_ = element_type;
  return *this;
}

Boundary &Boundary::addRegion(const std::vector<h_size> &indices,
                              h_size *region_index) {
  if (region_index)
    *region_index = regions_.size();
  regions_.emplace_back(discretization_, element_, indices);
  return *this;
}

Boundary &Boundary::setCondition(h_size region_index,
                                 bc::BoundaryCondition::Ptr condition) {
  HERMES_ASSERT(region_index < regions_.size());
  regions_[region_index].setCondition(condition);
  return *this;
}

Boundary &Boundary::setCondition(bc::BoundaryCondition::Ptr condition) {
  for (h_size i = 0; i < regions_.size(); ++i)
    setCondition(i, condition);
  return *this;
}

NaResult Boundary::resolve() {
  for (auto &region : regions_)
    NAIADES_RETURN_BAD_RESULT(region.resolve());
  return NaResult::noError();
}

const DiscreteOperator &Boundary::stencil(const Index &index) const {
  for (const auto &region : regions_)
    if (region.contains(index))
      return region.stencil(index);
  HERMES_WARN("Index {} not found in boundary.", naiades::to_string(index));
  static DiscreteOperator s_dop;
  return s_dop;
}

NaResult Boundary::Region::compute(FieldCRef<f32> interior_field,
                                   FieldRef<f32> field) const {
  if (stencils_.size() < index_set_.size()) {
    HERMES_ERROR("Boundary region not resolved before compute!");
    return NaResult::checkError();
  }
  for (auto i : index_set_) {
    field[i.index] = stencils_[i.flat_index](interior_field);
  }
  return NaResult::noError();
}

BoundarySet::Config &BoundarySet::Config::setElement(Element element_type) {
  element_type_ = element_type;
  return *this;
}

BoundarySet::Config &
BoundarySet::Config::setTopology(DiscretizationTopology::Ptr d_t) {
  d_t_ = d_t;
  return *this;
}

BoundarySet BoundarySet::Config::build() const {
  BoundarySet bs;
  bs.element_type_ = element_type_;
  bs.discretization_ = d_t_;
  return bs;
}

BoundarySet &BoundarySet::addRegion(const std::string &field_name,
                                    const std::vector<h_size> &indices,
                                    h_size *region_index) {
  auto it = boundaries_.find(field_name);
  if (it == boundaries_.end())
    boundaries_[field_name].set(discretization_, element_type_);
  boundaries_[field_name].addRegion(indices, region_index);
  return *this;
}

BoundarySet &BoundarySet::set(const std::string &field_name,
                              h_size region_index,
                              bc::BoundaryCondition::Ptr condition) {
  auto it = boundaries_.find(field_name);
  if (it == boundaries_.end())
    boundaries_[field_name].set(discretization_, element_type_);
  boundaries_[field_name].setCondition(region_index, condition);
  return *this;
}

BoundarySet &BoundarySet::set(const std::string &field_name,
                              bc::BoundaryCondition::Ptr condition) {
  auto it = boundaries_.find(field_name);
  if (it == boundaries_.end())
    boundaries_[field_name].set(discretization_, element_type_);
  boundaries_[field_name].setCondition(condition);
  return *this;
}

const Boundary &BoundarySet::operator[](const std::string &field_name) const {
  static Boundary s_dummy({});
  auto it = boundaries_.find(field_name);
  if (it == boundaries_.end())
    return s_dummy;
  return it->second;
}

Boundary &BoundarySet::operator[](const std::string &field_name) {
  static Boundary s_dummy({});
  auto it = boundaries_.find(field_name);
  if (it == boundaries_.end())
    return s_dummy;
  return it->second;
}

} // namespace naiades::core
