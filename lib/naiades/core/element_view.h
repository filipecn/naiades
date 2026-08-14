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

/// \file   discrete_element_view.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-08-07
/// \brief  Element view interface

#pragma once

#include <naiades/core/element.h>
#include <naiades/core/geometry.h>
#include <naiades/core/topology.h>
#include <naiades/utils/utils.h>

namespace naiades::core {

template <typename MeshType>
  requires HasGeometry<MeshType> && HasTopology<MeshType> && HasPtr<MeshType>
class ElementView {
public:
  class iterator {
  public:
    struct ElementItem {
      ElementItem(const ElementItem &) = delete;
      ElementItem &operator=(const ElementItem &) = delete;
      ElementItem(ElementItem &&) = default;
      ElementItem &operator=(ElementItem &&) = default;

      ElementIndex operator*() const {
        return ElementIndex(
            iterator_.element_view_.element_,
            Index((*iterator_.it_).global_index, IndexSpace::GLOBAL));
      }

      h_index sequentialIndex() const {
        return (*iterator_.it_).local_set_index;
      }

    private:
      friend class ElementView::iterator;
      ElementItem(const iterator &it) : iterator_(it) {}
      const iterator &iterator_;
    };

    const ElementItem &operator*() const { return item_; }
    iterator &operator++() {
      ++it_;
      return *this;
    }
    bool operator==(const iterator &rhs) const { return it_ == rhs.it_; }
    bool operator!=(const iterator &rhs) const { return it_ != rhs.it_; }

  private:
    friend class ElementView;
    iterator(const ElementView &element_view, IndexSet::iterator it)
        : element_view_(element_view), it_(it) {}

    const ElementView &element_view_;
    IndexSet::iterator it_;
    ElementItem item_;
  };

  ElementView(MeshType::Ptr mesh, const core::Element &element)
      : mesh_(mesh), element_(element) {}

  iterator begin() const { return iterator(*this, indices_.begin()); }
  iterator end() const { return iterator(*this, indices_.end()); }

private:
  friend class iterator;

  MeshType::Ptr mesh_;
  core::Element element_;
  IndexSet indices_;
};

} // namespace naiades::core