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

/// \file   field.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07

#include <naiades/core/field.h>

#include <naiades/base/debug.h>

namespace naiades::core {

void FieldGroup::setElement(Element loc) { element_ = loc; }

void FieldGroup::setIndexOffset(h_size o) { index_offset_ = o; }

Element FieldGroup::element() const { return element_; }

h_size FieldGroup::indexOffset() const { return index_offset_; }

NaResult FieldSet::setElementCount(Element loc, h_size count) {
  for (auto &item : fields_) {
    if (item.second.element() == loc)
      NAIADES_HE_RETURN_BAD_RESULT(item.second.resize(count));
  }
  return NaResult::noError();
}

NaResult FieldSet::setElementCountFrom(Topology *sd) {
  for (auto &item : fields_) {
    auto count = sd->elementCount(item.second.element());
    HERMES_ASSERT(count);
    NAIADES_HE_RETURN_BAD_RESULT(item.second.resize(count));
  }
  return NaResult::noError();
}

} // namespace naiades::core
