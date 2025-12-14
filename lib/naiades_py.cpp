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

/// \file   naiades_py.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2025-06-07
/// \brief  Python interface for Naiades lib.

#include <hermes/base/str.h>
#include <hermes/random/rng.h>
#include <naiades/sampling/sampler.h>
#include <naiades/utils/fields.h>
#include <naiades/utils/math.h>

#include <pybind11/chrono.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/native_enum.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <igl/cotmatrix.h>

namespace py = pybind11;

using py_Element = py::native_enum<naiades::core::Element::Type>;

/*
void printCallback(const hermes::cstr &m) {
  py::print(m.str());
  std::cout << std::flush;
}

void initHermes(bool verbose = false) {
  std::cout << std::unitbuf;
  hermes::io::Logger::addOptions(hermes::io ::logger_option_bits::abbreviate);
  hermes::io::Logger::setLogCallback(hermes::io::Logger::Level::info,
                                     printCallback);
  if (verbose) {
    hermes::io::Logger::setLogCallback([](const hermes::cstr &m,
                                          hermes::io::Logger::Level level,
                                          hermes::io::logger_options o) {
      HERMES_UNUSED_VARIABLE(level);
      HERMES_UNUSED_VARIABLE(o);
      py::print(m.str());
      std::cout << std::flush;
    });
  } else {
    hermes::io::Logger::setLogCallback([](const hermes::cstr &m,
                                          hermes::io::Logger::Level level,
                                          hermes::io::logger_options o) {
      HERMES_UNUSED_VARIABLE(m);
      HERMES_UNUSED_VARIABLE(level);
      HERMES_UNUSED_VARIABLE(o);
    });
  }
}*/

struct py_Mesh {

  static py_Mesh from(const naiades::geo::Grid2 &grid,
                      naiades::core::Element element) {
    py_Mesh mesh;
    auto g_vertices = grid.positions(naiades::core::Element::VERTEX_CENTER);
    auto g_positions = grid.positions(element);
    auto g_indices =
        grid.indices(element, naiades::core::Element::VERTEX_CENTER);
    {
      std::vector<f32> data(g_positions.size() * 3);
      std::vector<h_size> shape = {g_positions.size(), 3};
      std::vector<h_size> strides = {3 * sizeof(f32), sizeof(f32)};
      for (h_size i = 0; i < g_positions.size(); ++i) {
        data[i * 3 + 0] = g_positions[i].x;
        data[i * 3 + 1] = g_positions[i].y;
        data[i * 3 + 2] = 0;
      }
      mesh.positions =
          py::array_t<f32, py::array::c_style>(shape, strides, data.data());
    }
    {
      std::vector<f32> data(g_vertices.size() * 3);
      std::vector<h_size> shape = {g_vertices.size(), 3};
      std::vector<h_size> strides = {3 * sizeof(f32), sizeof(f32)};
      for (h_size i = 0; i < g_vertices.size(); ++i) {
        data[i * 3 + 0] = g_vertices[i].x;
        data[i * 3 + 1] = g_vertices[i].y;
        data[i * 3 + 2] = 0;
      }
      mesh.vertices =
          py::array_t<f32, py::array::c_style>(shape, strides, data.data());
    }
    {
      std::vector<u32> data(g_indices.size() * 4);
      std::vector<h_size> shape = {g_positions.size(), 4};
      std::vector<h_size> strides = {4 * sizeof(u32), sizeof(u32)};
      for (h_size i = 0; i < g_indices.size(); ++i)
        for (h_size j = 0; j < 4; ++j)
          data[i * 4 + j] = g_indices[i][j];
      mesh.faces =
          py::array_t<u32, py::array::c_style>(shape, strides, data.data());
    }
    return mesh;
  }

  py::array_t<f32, py::array::c_style> positions;
  py::array_t<f32, py::array::c_style> vertices;
  py::array_t<u32, py::array::c_style> faces;
};

struct StableFluids2_py {
  StableFluids2_py(bool verbose = false) {
    grid_.setSize({50, 50});
    grid_.setCellSize(1.f / 50);
    HERMES_UNUSED_VARIABLE(verbose);
    fields_.addVectorFields(naiades::core::Element::CELL_CENTER,
                            {"cell_velocity"});
    fields_.addScalarFields(naiades::core::Element::CELL_CENTER,
                            {"cell_R", "cell_G", "cell_B"});
    fields_.addScalarFields(naiades::core::Element::VERTEX_CENTER,
                            {"gaussian"});

    fields_.setElementCount(
        naiades::core::Element::CELL_CENTER,
        grid_.locationCount(naiades::core::Element::CELL_CENTER));

    fields_.setElementCount(
        naiades::core::Element::VERTEX_CENTER,
        grid_.locationCount(naiades::core::Element::VERTEX_CENTER));
  }

  void step(f32 timestep) {
    HERMES_UNUSED_VARIABLE(timestep);
    if (auto *cell_velocity = fields_.vectorField("cell_velocity")) {
      naiades::utils::zalesakVelocityField(grid_, *cell_velocity, {0.5f, 0.5f},
                                           hermes::math::constants::two_pi);
    }
    if (auto *cell_velocity = fields_.scalarField("cell_R")) {
      naiades::utils::setField<f32>(
          grid_, *cell_velocity, [&](const hermes::geo::point2 &p) -> f32 {
            return naiades::utils::gaussian(hermes::geo::vec2(0.02),
                                            hermes::geo::point2(0.5), p);
          });
    }
    if (auto *cell_velocity = fields_.scalarField("gaussian")) {
      naiades::utils::setField<f32>(
          grid_, *cell_velocity, [&](const hermes::geo::point2 &p) -> f32 {
            return naiades::utils::gaussian(hermes::geo::vec2(0.02),
                                            hermes::geo::point2(0.5), p);
          });
    }
  }

  py::array_t<float> getFloatField(const std::string &name) {
    auto *field = fields_.scalarField(name);
    if (!field)
      throw std::runtime_error("Field not found!");

    std::vector<float> data(field->size());
    std::vector<h_size> shape = {field->size()};
    std::vector<h_size> strides = {sizeof(float)};
    for (h_size i = 0; i < field->size(); ++i)
      data[i] = (*field)[i];

    return py::array_t<float>(
        shape, strides, data.data(),
        py::cast(
            std::move(data)) // Owner object (transfers std::vector ownership)
    );
  }

  py::array_t<float> sampleFloatField(const std::string &name,
                                      py::array_t<f32> &arr) {
    auto *field = fields_.scalarField(name);
    if (!field)
      throw std::runtime_error("Field not found!");

    // access array data and dimensions
    auto buf = arr.request();
    const float *ptr = static_cast<const float *>(buf.ptr);
    if (buf.ndim != 1)
      throw std::runtime_error("Only 1D arrays are supported.");

    h_size positions_count = buf.shape[0] / 2;

    std::vector<hermes::geo::point2> samples;
    for (h_size i = 0; i < positions_count; ++i)
      samples.emplace_back(ptr[i * 2], ptr[i * 2 + 1]);

    // sample field
    auto samples_field = naiades::sampling::sample(grid_, *field, samples);

    std::vector<f32> data(positions_count);
    std::vector<h_size> shape = {positions_count};
    std::vector<h_size> strides = {sizeof(f32)};
    for (h_size i = 0; i < positions_count; ++i)
      data[i] = samples_field[i];

    return py::array_t<f32>(shape,        // Shape (e.g., {3, 4})
                            strides,      // Strides (e.g., {32, 8} bytes)
                            data.data()); // Pointer to data
  }

  py::array_t<float> sampleVectorField(const std::string &name,
                                       py::array_t<f32> &arr) {
    auto *field = fields_.vectorField(name);
    if (!field)
      throw std::runtime_error("Field not found!");

    // access array data and dimensions
    auto buf = arr.request();
    const float *ptr = static_cast<const float *>(buf.ptr);
    if (buf.ndim != 1)
      throw std::runtime_error("Only 1D arrays are supported.");

    h_size positions_count = buf.shape[0] / 2;

    std::vector<hermes::geo::point2> samples;
    for (h_size i = 0; i < positions_count; ++i)
      samples.emplace_back(ptr[i * 2], ptr[i * 2 + 1]);

    // sample field
    auto samples_field = naiades::sampling::sample(grid_, *field, samples);

    std::vector<f32> data(positions_count * 2);
    std::vector<h_size> shape = {positions_count, 2};
    std::vector<h_size> strides = {2 * sizeof(f32), sizeof(f32)};
    for (h_size i = 0; i < positions_count; ++i) {
      data[i * 2 + 0] = samples_field[i].x;
      data[i * 2 + 1] = samples_field[i].y;
    }

    return py::array_t<f32>(shape,        // Shape (e.g., {3, 4})
                            strides,      // Strides (e.g., {32, 8} bytes)
                            data.data()); // Pointer to data
  }

  py::array_t<f32> getPositions(naiades::core::Element loc) {
    auto positions_count = grid_.locationCount(loc);
    std::vector<f32> data(positions_count * 2);

    for (auto ij : hermes::range2(grid_.resolution(loc))) {
      auto flat_ij = grid_.flatIndex(loc, ij);
      auto wp = grid_.position(loc, ij);
      data[flat_ij * 2 + 0] = wp.x;
      data[flat_ij * 2 + 1] = wp.y;
    }

    std::vector<h_size> shape = {positions_count, 2};
    std::vector<h_size> strides = {2 * sizeof(f32), sizeof(f32)};
    return py::array_t<f32>(shape, strides, data.data());
  }

  py_Mesh mesh(naiades::core::Element::Type element_type) {
    auto m = py_Mesh::from(grid_, element_type);
    return m;
  }

  naiades::geo::Grid2 grid_;
  naiades::core::FieldSet fields_;
  hermes::random::PCGRNG rng;
};

#define PY_ENUM_VALUE(T, V) .value(#V, T::V)

PYBIND11_MODULE(naiades_py, m) {
  m.doc() = "Naiades module";

  py_Element(m, "Element", "enum.IntEnum")
      PY_ENUM_VALUE(naiades::core::Element::Type, CELL_CENTER)   //
      PY_ENUM_VALUE(naiades::core::Element::Type, U_FACE_CENTER) //
      PY_ENUM_VALUE(naiades::core::Element::Type, V_FACE_CENTER) //
          .finalize();

  py::class_<py_Mesh>(m, "Mesh")
      .def_readonly("field", &py_Mesh::positions)
      .def_readonly("verts", &py_Mesh::vertices)
      .def_readonly("faces", &py_Mesh::faces);

  py::class_<StableFluids2_py>(m, "StableFluids2")
      .def(py::init([](bool verbose) { return new StableFluids2_py(verbose); }))
      .def("get_float_field", &StableFluids2_py::getFloatField, "")
      .def("step", &StableFluids2_py::step, "")
      .def("sample_float_field", &StableFluids2_py::sampleFloatField, "")
      .def("sample_vector_field", &StableFluids2_py::sampleVectorField, "")
      .def("get_positions", &StableFluids2_py::getPositions, "")
      .def("get_mesh", &StableFluids2_py::mesh, "");
}
