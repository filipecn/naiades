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

/// \file   utils.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07

#include <naiades/utils/utils.h>

#include <algorithm>

namespace naiades {

HERMES_TO_STRING_METHOD_BEGIN(naiades::utils::StepLoop::Iteration)
HERMES_TO_STRING_METHOD_FIELD(step_.iteration_index);
HERMES_TO_STRING_METHOD_FIELD(step_.last_step_duration.count());
HERMES_TO_STRING_METHOD_FIELD(step_.current_fps_period.count());
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(utils::IndexInterval)
HERMES_TO_STRING_METHOD_LINE("[{}, {})", object.start, object.end);
HERMES_TO_STRING_METHOD_END

HERMES_TO_STRING_METHOD_BEGIN(utils::IndexSet)
HERMES_TO_STRING_METHOD_LINE(
    "set indices: {}\n",
    std::visit(
        utils::IndexSetOverloaded{
            [](std::monostate s) {
              HERMES_UNUSED_VARIABLE(s);
              return std::string();
            },
            [](const std::vector<h_size> &indices) -> std::string {
              return hermes::cstr::join(indices, ", ", 10);
            },
            [](const std::vector<utils::IndexInterval> &indices)
                -> std::string {
              auto f = [](const utils::IndexInterval &interval) -> std::string {
                return naiades::to_string(interval);
              };
              return hermes::cstr::join<std::vector<utils::IndexInterval>,
                                        utils::IndexInterval>(indices, f, ", ",
                                                              10);
            }},
        object.data_));
HERMES_TO_STRING_METHOD_FIELD(index_count_);
HERMES_TO_STRING_METHOD_LINE("index offsets: {}\n",
                             hermes::cstr::join(object.index_offset_, ", ",
                                                10));
HERMES_TO_STRING_METHOD_END

} // namespace naiades

namespace naiades::utils {

IndexSet::iterator::iterator(const IndexSet &index_set, h_size flat_index)
    : index_set_{index_set}, item_{flat_index, 0} {
  std::visit(
      IndexSetOverloaded{[](std::monostate s) { HERMES_UNUSED_VARIABLE(s); },
                         [&](const std::vector<h_size> &indices) {
                           if (item_.flat_index < indices.size())
                             item_.index = indices[item_.flat_index];
                         },
                         [&](const std::vector<IndexInterval> &indices) {
                           if (item_.flat_index < indices.size()) {
                             item_.index = index_set[item_.flat_index];
                           }
                         }},
      index_set_.data_);
}

const IndexSet::iterator::Item &IndexSet::iterator::operator*() const {
  return item_;
}

IndexSet::iterator &IndexSet::iterator::operator++() {
  item_.flat_index++;
  std::visit(IndexSetOverloaded{
                 [](std::monostate s) { HERMES_UNUSED_VARIABLE(s); },
                 [&](const std::vector<h_size> &indices) {
                   if (item_.flat_index < indices.size())
                     item_.index = indices[item_.flat_index];
                 },
                 [&](const std::vector<IndexInterval> &indices) {
                   if (interval_index_ < indices.size() - 1) {
                     if (item_.flat_index >=
                         index_set_.index_offset_[interval_index_ + 1]) {
                       interval_index_++;
                     }
                   }
                   item_.index = indices[interval_index_].start +
                                 (item_.flat_index -
                                  index_set_.index_offset_[interval_index_]);
                 }},
             index_set_.data_);
  return *this;
}

bool IndexSet::iterator::operator==(const IndexSet::iterator &rhs) const {
  return item_.flat_index == rhs.item_.flat_index;
}

bool IndexSet::iterator::operator!=(const IndexSet::iterator &rhs) const {
  return !(*this == rhs);
}

IndexSet::IndexSet(const std::vector<h_size> &set_indices) { set(set_indices); }

h_size IndexSet::size() const { return index_count_; }

void IndexSet::set(const std::vector<h_size> &set_indices) {
  if (set_indices.empty())
    return;
  auto sorted_indices = set_indices;
  std::sort(sorted_indices.begin(), sorted_indices.end());

  index_count_ = set_indices.size();

  std::vector<IndexInterval> intervals;
  h_size last_index = sorted_indices.front();
  IndexInterval interval;
  interval.start = last_index;
  interval.end = last_index + 1;
  for (auto index : sorted_indices) {
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

  std::sort(intervals.begin(), intervals.end(),
            [](const IndexInterval &a, const IndexInterval &b) -> bool {
              HERMES_ASSERT(a.start != b.start);
              return a.start < b.start;
            });

  h_size offset = 0;
  for (auto interval : intervals) {
    index_offset_.emplace_back(offset);
    offset += interval.end - interval.start;
  }

  data_ = std::move(intervals);
}

std::optional<h_size>
findIntervalIndex(const std::vector<IndexInterval> &indices, h_size index) {
  if (indices.empty())
    return {};
  auto it = std::lower_bound(indices.begin(), indices.end(), index,
                             [](const IndexInterval &interval, h_size value) {
                               return interval.start < value;
                             });
  auto interval_index = it - indices.begin();
  // corner case
  if (static_cast<h_size>(interval_index) < indices.size() &&
      indices[interval_index].start <= index &&
      indices[interval_index].end > index)
    return interval_index;
  if (interval_index > 0) {
    if (indices[interval_index - 1].end <= index)
      return {};
    if (indices[interval_index - 1].start <= index)
      return interval_index - 1;
  }
  return {};
}

h_size IndexSet::operator[](h_size seq_index) const {
  return std::visit(
      IndexSetOverloaded{
          [](std::monostate s) -> h_size {
            HERMES_UNUSED_VARIABLE(s);
            HERMES_ASSERT(false);
            return {};
          },
          [&](const std::vector<h_size> &indices) -> h_size {
            HERMES_ASSERT(seq_index < indices.size());
            return indices[seq_index];
          },
          [&](const std::vector<IndexInterval> &indices) -> h_size {
            auto it =
                std::lower_bound(index_offset_.begin(), index_offset_.end(),
                                 seq_index, std::less{});
            if (it == index_offset_.end()) {
              // should be in the last interval
              HERMES_ASSERT(seq_index - index_offset_.back() <
                            indices.back().end - indices.back().start);
              return indices.back().start + seq_index - index_offset_.back();
            }
            auto interval_index = it - index_offset_.begin();
            if (index_offset_[interval_index] > seq_index) {
              // we need the previous
              HERMES_ASSERT(interval_index);
              interval_index--;
            }
            return indices[interval_index].start + seq_index -
                   index_offset_[interval_index];
          }},
      data_);
}

core::Index IndexSet::seqIndex(h_size set_index) const {
  return std::visit(
      IndexSetOverloaded{
          [](std::monostate s) -> core::Index {
            HERMES_UNUSED_VARIABLE(s);
            return core::Index::invalid();
          },
          [&](const std::vector<h_size> &indices) -> core::Index {
            auto it = std::lower_bound(indices.begin(), indices.end(),
                                       set_index, std::less{});
            if (it == indices.end())
              return core::Index::invalid();
            return core::Index::local(it - indices.begin());
          },
          [&](const std::vector<IndexInterval> &indices) -> core::Index {
            auto interval_index = findIntervalIndex(indices, set_index);
            if (interval_index.has_value()) {
              return core::Index::local(index_offset_[*interval_index] +
                                        set_index -
                                        indices[*interval_index].start);
            }
            return core::Index::invalid();
          }},
      data_);
}

bool IndexSet::contains(const core::Index &index) const {
  if (index.space() == core::IndexSpace::LOCAL)
    return *index < index_count_;
  return std::visit(IndexSetOverloaded{
                        [](std::monostate s) -> bool {
                          HERMES_UNUSED_VARIABLE(s);
                          return false;
                        },
                        [&](const std::vector<h_size> &indices) -> bool {
                          if (indices.empty())
                            return false;
                          auto it =
                              std::lower_bound(indices.begin(), indices.end(),
                                               *index, std::less{});
                          if (it == indices.end())
                            return false;
                          return *index == *it;
                        },
                        [&](const std::vector<IndexInterval> &indices) -> bool {
                          auto i = findIntervalIndex(indices, *index);
                          return i.has_value();
                        }},
                    data_);
}

IndexSet::iterator IndexSet::begin() const { return {*this, 0}; }

IndexSet::iterator IndexSet::end() const { return {*this, size()}; }

StepLoop::Iteration::Iteration(StepLoop &loop, bool is_end)
    : loop_{loop}, is_end_{is_end} {}

StepLoop::Iteration &StepLoop::Iteration::operator++() {
  if (in_step_) {
    // finish step
    in_step_ = false;
    auto step_duration = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - step_.step_start);
    step_.last_step_duration = step_duration;
    step_.current_fps_period =
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - step_.step_start);
  }
  step_.iteration_index++;
  if (loop_.max_step_count_ && step_.iteration_index >= loop_.max_step_count_) {
    is_end_ = true;
    return *this;
  }
  if (is_end_)
    return *this;
  in_step_ = true;
  step_.step_start = std::chrono::steady_clock::now();
  return *this;
}

StepLoop::Iteration &StepLoop::Iteration::operator*() { return *this; }

bool StepLoop::Iteration::operator==(const StepLoop::Iteration &rhs) const {
  return is_end_ == rhs.is_end_;
}

bool StepLoop::Iteration::operator!=(const StepLoop::Iteration &rhs) const {
  return is_end_ != rhs.is_end_;
}

const StepLoop::Iteration::Step &StepLoop::Iteration::step() const {
  return step_;
}

void StepLoop::Iteration::endLoop() { is_end_ = true; }

StepLoop &StepLoop::setDurationInSteps(u32 step_count) {
  max_step_count_ = step_count;
  return *this;
}

StepLoop::Iteration StepLoop::begin() { return {*this, false}; }

StepLoop::Iteration StepLoop::end() { return {*this, true}; }

} // namespace naiades::utils
