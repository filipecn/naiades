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

/// \file   element.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07

#include <naiades/core/element.h>

namespace naiades::core {

h_size Index::s_invalid_value_ = 1 << 30;

ElementIndex ElementIndex::global(Element loc, h_size i) {
  return {.index = Index::global(i), .element = loc};
}

Element Element::cell() { return Element(Element::Type::CELL); }

Element Element::face() { return Element(Element::Type::FACE); }

Element Element::uFace() { return Element(Element::Type::U_FACE); }

Element Element::vFace() { return Element(Element::Type::V_FACE); }

Element Element::wFace() { return Element(Element::Type::W_FACE); }

Element Element::vertex() { return Element(Element::Type::VERTEX); }

} // namespace naiades::core
