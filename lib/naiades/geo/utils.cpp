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

/// \file   utils.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-04-20

#include <naiades/geo/utils.h>

#include <igl/triangle/triangulate.h>

namespace naiades::geo {

Result<HE2> triangulate(std::vector<hermes::geo::point2> &points) {

  // Input polygon
  Eigen::MatrixXd V;
  Eigen::MatrixXi E;
  Eigen::MatrixXd H;

  // Triangulated interior
  Eigen::MatrixXd V2;
  Eigen::MatrixXi F2;

  V.resize(points.size(), 2);
  E.resize(points.size(), 2);

  for (h_size i = 0; i < points.size(); ++i) {
    V(i, 0) = points[i].x;
    V(i, 1) = points[i].y;
    E(i, 0) = i;
    E(i, 1) = (i + 1) % points.size();
  }
  igl::triangle::triangulate(V, E, H, "a0.005q", V2, F2);

  HE2 he;
  // add vertices
  for (i32 v = 0; v < V2.rows(); ++v) {
    he.addVertex(hermes::geo::point2(V2(v, 0), V2(v, 1)));
  }
  for (i32 t = 0; t < F2.rows(); ++t)
    he.addCell({
        static_cast<h_index>(F2(t, 0)),
        static_cast<h_index>(F2(t, 1)),
        static_cast<h_index>(F2(t, 2)),
    });

  return Result<HE2>(std::move(he));
}

} // namespace naiades::geo