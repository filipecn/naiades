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

#include <naiades/base/debug.h>
#include <naiades/core/element.h>

#include <variant>

namespace naiades::utils {

/// Represents a contiguous interval of boundary indices [start,end)
struct IndexInterval {
  h_size start;
  h_size end;
};

using IndexSetData = std::variant<std::monostate, std::vector<IndexInterval>,
                                  std::vector<h_size>>;
template <class... Ts> struct IndexSetOverloaded : Ts... {
  using Ts::operator()...;
};

template <class... Ts> IndexSetOverloaded(Ts...) -> IndexSetOverloaded<Ts...>;

/// The IndexSet maps an arbitrary sequence of indices into a contiguous 0-based
/// sequence.
class IndexSet {
public:
  class iterator {
  public:
    struct Item {
      h_size local_set_index;
      h_size global_index;
    };
    const Item &operator*() const;
    iterator &operator++();
    bool operator==(const iterator &rhs) const;
    bool operator!=(const iterator &rhs) const;

  private:
    iterator(const IndexSet &index_set, h_size flat_index);
    h_size interval_index_{0};
    const IndexSet &index_set_;
    Item item_;

    friend class IndexSet;
  };

  IndexSet() = default;
  IndexSet(const std::vector<h_size> &set_indices);

  h_size size() const;
  void set(const std::vector<h_size> &set_indices);
  h_size operator[](h_size seq_index) const;
  core::Index seqIndex(h_size set_index) const;
  bool contains(const core::Index &index) const;

  iterator begin() const;
  iterator end() const;

private:
  // arbitrary sequence
  IndexSetData data_;
  // offset of arbitrary elements in the contiguous 0-based sequence.
  std::vector<h_size> index_offset_;
  // total index count
  h_size index_count_;

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS
  friend struct hermes::DebugTraits<IndexSet>;
#endif
};

/// Auxiliary class for looping over step indices keeping a time statistics
///
/// This class is meant to be used in a for loop as:
/// for (auto it : StepLoop()) {
///   // iteration info can be accessed via 'it':
///   it.step();
/// }
/// The step loop can be configured as well:
/// StepLoop().setFPS(...).setDurationInSteps(...)
///
/// \note The FPS is ensured by calling the std::this_thread::sleep method
class StepLoop {
public:
  StepLoop &setDurationInSteps(u32 step_count);

  struct Iteration {
    struct Step {
      u32 iteration_index{0};
      // fps
      std::chrono::steady_clock::time_point step_start;
      std::chrono::microseconds last_step_duration{0};
      std::chrono::microseconds current_fps_period{0};
    };

    Iteration(StepLoop &loop, bool is_end);

    Iteration &operator++();
    Iteration &operator*();
    bool operator==(const Iteration &) const;
    bool operator!=(const Iteration &) const;

    const Step &step() const;
    void endLoop();

  private:
    StepLoop &loop_;
    bool in_step_{false};
    bool is_end_{false};
    Step step_;

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS
    friend struct hermes::DebugTraits<Iteration>;
#endif
  };

  Iteration begin();
  Iteration end();

private:
  friend struct Iteration;

  // default for 60 fps
  std::chrono::microseconds fps_period_{16666};
  u32 max_step_count_{0};
};

} // namespace naiades::utils

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS

namespace hermes {

template <> struct DebugTraits<naiades::utils::StepLoop::Iteration> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::utils::StepLoop::Iteration &data) {
    auto m = DebugMessage();
    m.add("iteration index", data.step_.iteration_index);
    m.add("last step duration", data.step_.last_step_duration.count());
    m.add("current fps period", data.step_.current_fps_period.count());
    return m;
  }
};

template <> struct DebugTraits<naiades::utils::IndexInterval> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::utils::IndexInterval &data) {
    auto m = DebugMessage();
    m.addFmt("[{}, {})", data.start, data.end);
    return m;
  }
};

template <> struct DebugTraits<naiades::utils::IndexSet> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::utils::IndexSet &data) {
    auto m = DebugMessage();
    m.add("index count", data.index_count_);
    m.add("set indices",
          std::visit(
              naiades::utils::IndexSetOverloaded{
                  [](std::monostate s) {
                    HERMES_UNUSED_VARIABLE(s);
                    return std::string();
                  },
                  [](const std::vector<h_size> &indices) -> std::string {
                    return hermes::cstr::join(indices, ", ", 10);
                  },
                  [](const std::vector<naiades::utils::IndexInterval> &indices)
                      -> std::string {
                    auto f = [](const naiades::utils::IndexInterval &interval)
                        -> std::string { return hermes::to_string(interval); };
                    return hermes::cstr::join<
                        std::vector<naiades::utils::IndexInterval>,
                        naiades::utils::IndexInterval>(indices, f, ", ", 10);
                  }},
              data.data_));
    m.add("index offsets:", hermes::cstr::join(data.index_offset_, ", ", 10));
    return m;
  }
};

} // namespace hermes

#endif
