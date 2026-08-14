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

/// \file   topology.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-03-28

#include <naiades/core/topology.h>

namespace naiades::core {

NeighbourhoodCriteria NeighbourhoodCriteria::knn(h_index n) {
  return {.max_topological_distance = 0,
          .max_count = n,
          .max_distance = -1.f,
          .sort_by_distance = false};
}

NeighbourhoodCriteria NeighbourhoodCriteria::k_ring(h_index k) {
  return {.max_topological_distance = k,
          .max_count = 0,
          .max_distance = -1.f,
          .sort_by_distance = false};
}

NeighbourhoodCriteria &NeighbourhoodCriteria::withMaxCount(h_index _max_count) {
  max_count = _max_count;
  return *this;
}

NeighbourhoodCriteria &
NeighbourhoodCriteria::withMaxDistance(f32 _max_distance) {
  max_distance = _max_distance;
  return *this;
}

NeighbourhoodCriteria &NeighbourhoodCriteria::withSortByDistance() {
  sort_by_distance = true;
  return *this;
}

std::vector<std::vector<h_size>> Topology::subElements(Element loc,
                                                       Element sub_loc) const {
  std::vector<std::vector<h_size>> is;
  h_size n = elementCount(loc);
  for (h_index i = 0; i < n; ++i) {
    is.emplace_back(elementIndices(
        ElementIndex::global(loc, i + elementIndexOffset(loc)), sub_loc));
  }
  return is;
}

// std::vector<Neighbour> Topology::star(const ElementIndex &iloc,
//                                       Element boundary_loc) const {
//   return star(iloc, iloc.element, {boundary_loc});
// }

} // namespace naiades::core
