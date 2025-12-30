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

#pragma once

#include <naiades/utils/utils.h>

namespace naiades {

HERMES_TO_STRING_METHOD_BEGIN(naiades::utils::StepLoop::Iteration)
HERMES_TO_STRING_METHOD_FIELD(step_.iteration_index);
HERMES_TO_STRING_METHOD_FIELD(step_.last_step_duration.count());
HERMES_TO_STRING_METHOD_FIELD(step_.current_fps_period.count());
HERMES_TO_STRING_METHOD_END

} // namespace naiades

namespace naiades::utils {

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
