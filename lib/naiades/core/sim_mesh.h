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

/// \file   sim_mesh.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Simulation Mesh.

#pragma once

#include <naiades/core/field.h>
#include <naiades/geo/grid.h>

namespace naiades::core {

struct Boundary {
  std::vector<h_size> primitives;
  /// section name -> [start, end)
  std::unordered_map<std::string, std::pair<h_size, h_size>> sections;
};

/// A simulation mesh holds the topology of the discretization and other
/// information required by simulation algorithms.
class SimMesh {
public:
  struct Config {
    static Config from(const geo::RegularGrid2 &grid);
    Result<SimMesh> build() const;

  private:
    Boundary cell_boundary_;
    Boundary face_boundary_;
  };

  const Boundary &faceBoundary() const;
  const Boundary &cellBoundary() const;

private:
  Boundary cell_boundary_;
  Boundary face_boundary_;
};

} // namespace naiades::core
