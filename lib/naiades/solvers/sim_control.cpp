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

/// \file   sim_time.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07

#include <naiades/solvers/sim_control.h>

#include <naiades/utils/utils.h>

namespace naiades::solvers {

SimControl &SimControl::setCFL(f32 value) {
  cfl_ = value;
  return *this;
}

SimControl &SimControl::setTimestep(f32 timestep) {
  dt_ = timestep;
  return *this;
}

SimControl &SimControl::setWriteTimestep(f32 write_timestep) {
  wdt_ = write_timestep;
  return *this;
}

SimControl &SimControl::setStartTime(f32 start_time) {
  start_time_ = start_time;
  return *this;
}

SimControl &SimControl::setEndTime(f32 end_time) {
  end_time_ = end_time;
  return *this;
}

NaResult SimControl::run(Solver::Ptr solver) {
  h_size step_count = (end_time_ - start_time_) / dt_;

  for (auto iteration : utils::StepLoop().setDurationInSteps(step_count)) {
  }
  return NaResult::noError();
}

} // namespace naiades::solvers
