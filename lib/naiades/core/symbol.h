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

/// \file   symbol.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-03-21

#pragma once

#include <naiades/core/element.h>

#include <string>

namespace naiades::core {

/// Represents a numerical quantity/field defined at a discrete element.
struct Symbol {
  Symbol() = default;
  Symbol(const std::string &name, const Element &loc) : name{name}, loc{loc} {}
  std::string name;
  Element loc;

  bool operator==(const Symbol &rhs) const {
    return name == rhs.name && loc == rhs.loc;
  }
};

struct DiscreteSymbol {

  static DiscreteSymbol cell(const std::string &name,
                             const Element &boundary_loc = Element::face()) {
    return DiscreteSymbol(name, Element::cell(), boundary_loc);
  }
  DiscreteSymbol() = default;
  DiscreteSymbol(const std::string &name, const Element &interior_field_loc,
                 const Element &boundary_field_loc)
      : symbol{Symbol(name, interior_field_loc)},
        boundary_symbol{Symbol(name, boundary_field_loc)} {}
  DiscreteSymbol(const Symbol &sym, const Symbol &b_sym) noexcept
      : symbol{sym}, boundary_symbol{b_sym} {}

  bool operator==(const DiscreteSymbol &rhs) const {
    return symbol == rhs.symbol && boundary_symbol == rhs.boundary_symbol;
  }

  Symbol symbol;
  Symbol boundary_symbol;
};

} // namespace naiades::core

namespace std {
template <> struct hash<naiades::core::Symbol> {
  inline size_t operator()(const naiades::core::Symbol &symbol) const {
    return hash<std::string>{}(symbol.name) ^
           (hash<naiades::core::Element>{}(symbol.loc) << 1);
  }
};

} // namespace std

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS

namespace hermes {

template <> struct DebugTraits<naiades::core::Symbol> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::core::Symbol &data) {
    return DebugMessage().addFmt("({}, {})", data.name,
                                 hermes::to_string(data.loc));
  }
};

template <> struct DebugTraits<naiades::core::DiscreteSymbol> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::core::DiscreteSymbol &data) {
    return DebugMessage()
        .add("sym  ", data.symbol)
        .add("b_sym", data.boundary_symbol);
  }
};

} // namespace hermes

#endif