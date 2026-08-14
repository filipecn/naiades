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

/// \file   neighbourhood.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-07-24
/// \brief  .

#pragma once

#include <naiades/core/geometry.h>
#include <naiades/core/topology.h>

#include <queue>
#include <unordered_set>

namespace naiades::core {

///

template <typename T>
  requires HasPtr<T> && HasGeometry<T> && HasTopology<T>
class Neighbourhood {
public:
  Neighbourhood(T::Ptr structure) noexcept : structure_{structure} {}
  std::vector<Neighbour> knn(h_size k, const ElementIndex &iloc,
                             Element b_loc) const {
    struct MaxHeapElement {
      ElementIndex eloc;
      real_t distance;

      bool operator<(const MaxHeapElement &rhs) const {
        return distance < rhs.distance;
      }
    };

    std::unordered_set<ElementIndex> listed;
    std::priority_queue<MaxHeapElement> mh;

    auto center = structure_->center(iloc);
    std::queue<ElementIndex> q;

    q.push(iloc);
    listed.insert(iloc);

    bool found_closer = true;

    // Level-by-level BFS execution
    while (!q.empty() && found_closer) {
      int current_level_size = q.size();

      // If an entire level expands and finds nothing better, we can safely
      // stop.
      found_closer = false;

      for (int i = 0; i < current_level_size; ++i) {
        auto cur_iloc = q.front();
        q.pop();

        if (b_loc != iloc.element && cur_iloc.element == b_loc) {
          continue;
        }

        auto cur_center = structure_->center(cur_iloc);
        real_t dist = hermes::geo::distance(center, cur_center);

        if (mh.size() < k) {
          mh.push({cur_iloc, dist});
          found_closer = true;
        } else if (dist < mh.top().distance) {
          mh.pop();
          mh.push({cur_iloc, dist});
          found_closer = true;
        }

        // Queue unvisited neighbors
        for (auto nid : structure_->neighbours(cur_iloc, b_loc)) {
          // std::unordered_set::insert returns a pair. .second is true
          // if the element was actually inserted (meaning it wasn't listed
          // yet).
          if (listed.insert(nid).second) {
            q.push(nid);
          }
        }
      }
    }

    // Build the result
    std::vector<Neighbour> nbh;
    nbh.reserve(mh.size());
    while (!mh.empty()) {
      nbh.push_back(
          {.element_index = mh.top().eloc, .distance = mh.top().distance});
      mh.pop();
    }

    // Optional: Since it was built from a max-heap, the vector is currently
    // ordered from furthest to closest. You might want to reverse it.
    std::reverse(nbh.begin(), nbh.end());

    return nbh;
  }

  std::vector<Neighbour> star(const ElementIndex &iloc, Element b_loc) const {
    std::vector<Neighbour> nbh(1, {.element_index = iloc, .distance = 0.f});
    auto r = ring(1, iloc, b_loc);
    nbh.insert(nbh.end(), r.begin(), r.end());
    return nbh;
  }

  std::vector<Neighbour> ring(h_size k, const ElementIndex &iloc,
                              Element b_loc) const {
    std::unordered_set<ElementIndex> listed;
    auto bfs = [&](const ElementIndex &center_iloc) {
      // (iloc, level)
      std::queue<std::pair<ElementIndex, h_index>> q;
      q.push(std::make_pair(center_iloc, 0));
      while (!q.empty()) {
        auto cur_iloc = q.front().first;
        auto cur_level = q.front().second;
        q.pop();
        listed.insert(cur_iloc);
        if (cur_level == k)
          continue;
        if (b_loc != iloc.element && cur_iloc.element == b_loc)
          continue;
        for (auto nid : structure_->neighbours(cur_iloc, b_loc)) {
          if (!listed.count(nid)) {
            q.push(std::make_pair(nid, cur_level + 1));
          }
        }
      }
    };
    bfs(iloc);
    std::vector<Neighbour> nbh;
    auto center = structure_->center(iloc);
    for (const auto &niloc : listed) {
      if (niloc != iloc) {
        auto n_center = structure_->center(niloc);
        nbh.push_back({.element_index = niloc,
                       .distance = hermes::geo::distance(center, n_center)});
      }
    }
    return nbh;
  }

private:
  T::Ptr structure_;
};

} // namespace naiades::core
