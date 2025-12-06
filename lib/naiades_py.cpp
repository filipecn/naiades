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
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <igl/cotmatrix.h>

namespace py = pybind11;

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
}

struct StableFluids2_py {
  StableFluids2_py(bool verbose = false) {
    HERMES_UNUSED_VARIABLE(verbose);
    fields_.addScalarField("density",
                           naiades::core::FieldLocation::CELL_CENTER);

    fields_.setLocationCount(
        naiades::core::FieldLocation::CELL_CENTER,
        grid_.locationCount(naiades::core::FieldLocation::CELL_CENTER));
  }

  void step(f32 timestep) {
    HERMES_UNUSED_VARIABLE(timestep);
    auto *density = fields_.scalarField("density");
    HERMES_ASSERT(density);

    naiades::utils::setField<f32>(grid_, *density,
                                  [&](const hermes::geo::point2 &p) -> f32 {
                                    HERMES_UNUSED_VARIABLE(p);
                                    return rng.uniformFloat();
                                  });
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
        shape,       // Shape (e.g., {3, 4})
        strides,     // Strides (e.g., {32, 8} bytes)
        data.data(), // Pointer to data
        py::cast(
            std::move(data)) // Owner object (transfers std::vector ownership)
    );
  }

  py::array_t<float> sampleFloatField(const std::string &name,
                                      py::array_t<float> &arr) {
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

    std::vector<float> data(positions_count);
    std::vector<h_size> shape = {positions_count};
    std::vector<h_size> strides = {sizeof(float)};
    for (h_size i = 0; i < positions_count; ++i) {
      // sample
      data[i] = samples_field[i];
    }

    return py::array_t<float>(
        shape,       // Shape (e.g., {3, 4})
        strides,     // Strides (e.g., {32, 8} bytes)
        data.data(), // Pointer to data
        py::cast(
            std::move(data)) // Owner object (transfers std::vector ownership)
    );
  }

  naiades::geo::RegularGrid2 grid_;
  naiades::core::FieldSet fields_;
  hermes::random::PCGRNG rng;
};

PYBIND11_MODULE(naiades_py, m) {
  m.doc() = "Naiades module";

  py::class_<StableFluids2_py>(m, "StableFluids2")
      .def(py::init([](bool verbose) { return new StableFluids2_py(verbose); }))
      .def("get_float_field", &StableFluids2_py::getFloatField, "")
      .def("step", &StableFluids2_py::step, "")
      .def("sample_float_field", &StableFluids2_py::sampleFloatField, "");
}
