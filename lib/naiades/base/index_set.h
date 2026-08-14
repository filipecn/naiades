/* Copyright (c) 2026, FilipeCN.
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

/// \file   index_set.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-08-08
/// \brief

#pragma once

#include <naiades/base/debug.h>

#include <variant>

namespace naiades {

enum class IndexSpace { GLOBAL, LOCAL, CUSTOM };

/// The element index holds the unique id of an element within a given space.
struct Index {
  static Index local(h_size value) { return {value, IndexSpace::LOCAL}; }
  static Index global(h_size value) { return {value, IndexSpace::GLOBAL}; }

  static Index invalid() { return {s_invalid_value_, IndexSpace::LOCAL}; }

  Index() : space_{IndexSpace::GLOBAL}, value_{s_invalid_value_} {}
  Index(h_size value, IndexSpace ctx) : space_{ctx}, value_{value} {}
  Index &operator=(h_size i) {
    value_ = i;
    return *this;
  }

  operator h_index() const { return value_; }

  bool operator==(const Index &rhs) const {
    return space_ == rhs.space_ && value_ == rhs.value_;
  }

  h_size operator*() const { return value_; };
  bool isValid() const { return value_ != s_invalid_value_; }
  bool isLocal() const { return space_ == IndexSpace::LOCAL; }
  bool isGlobal() const { return space_ == IndexSpace::GLOBAL; }

  IndexSpace space() const { return space_; }

  Index &operator++() {
    if (value_ != s_invalid_value_)
      value_++;
    return *this;
  }

private:
  friend struct std::hash<Index>;
  IndexSpace space_{IndexSpace::GLOBAL};
  h_size value_;
  static h_size s_invalid_value_;
};

/// Represents a contiguous interval of indices [start,end)
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
  IndexSet(const IndexInterval &interval);

  h_size size() const;
  void set(const std::vector<h_size> &set_indices);
  h_size operator[](h_size seq_index) const;
  Index seqIndex(h_size set_index) const;
  bool contains(const Index &index) const;

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

} // namespace naiades

namespace std {
template <> struct hash<naiades::Index> {
  inline size_t operator()(const naiades::Index &x) const {
    return static_cast<h_index>(x);
  }
};

} // namespace std

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS
namespace hermes {
#define NAIADES_ENUM_TO_STRING_CASE(TYPE, NAME)                                \
  case TYPE::NAME: {                                                           \
    m.addFmt(#NAME);                                                           \
    break;                                                                     \
  }

template <> struct DebugTraits<naiades::IndexSpace> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::IndexSpace &data) {
    auto m = DebugMessage();
    switch (data) {
      NAIADES_ENUM_TO_STRING_CASE(naiades::IndexSpace, GLOBAL)
      NAIADES_ENUM_TO_STRING_CASE(naiades::IndexSpace, LOCAL)
      NAIADES_ENUM_TO_STRING_CASE(naiades::IndexSpace, CUSTOM)
    }
    return m;
  }
};

template <> struct DebugTraits<naiades::Index> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::Index &data) {
    auto m = DebugMessage();
    if (data.isValid()) {
      m.addFmt("{}", *data);
    } else {
      m.addFmt("[invalid - ]", *data, to_string(data.space()));
    }
    return m;
  }
};
template <> struct DebugTraits<naiades::IndexInterval> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::IndexInterval &data) {
    auto m = DebugMessage();
    m.addFmt("[{}, {})", data.start, data.end);
    return m;
  }
};

template <> struct DebugTraits<naiades::IndexSet> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::IndexSet &data) {
    auto m = DebugMessage();
    m.add("index count", data.index_count_);
    m.add(
        "set indices",
        std::visit(
            naiades::IndexSetOverloaded{
                [](std::monostate s) {
                  HERMES_UNUSED_VARIABLE(s);
                  return std::string();
                },
                [](const std::vector<h_size> &indices) -> std::string {
                  return hermes::cstr::join(indices, ", ", 10);
                },
                [](const std::vector<naiades::IndexInterval> &indices)
                    -> std::string {
                  auto f = [](const naiades::IndexInterval &interval)
                      -> std::string { return hermes::to_string(interval); };
                  return hermes::cstr::join<std::vector<naiades::IndexInterval>,
                                            naiades::IndexInterval>(indices, f,
                                                                    ", ", 10);
                }},
            data.data_));
    m.add("index offsets:", hermes::cstr::join(data.index_offset_, ", ", 10));
    return m;
  }
};

#undef NAIADES_ENUM_TO_STRING_CASE

} // namespace hermes

#endif
