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

#include <naiades/utils/fields.h>

#include <pybind11/chrono.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <igl/cotmatrix.h>

pybind11::array_t<float> get_scalar_field(pybind11::array_t<float> &arr,
                                          int resolution) {
  (void)resolution;
  // access array data and dimensions
  auto buf = arr.request();
  // const float *ptr = static_cast<const float *>(buf.ptr);
  if (buf.ndim != 1)
    throw std::runtime_error("ONly 1D arrays are supported.");
  h_size positions_count = buf.shape[0] / 2;

  std::vector<float> data(positions_count);
  std::vector<h_size> shape = {positions_count};
  std::vector<h_size> strides = {sizeof(float)};
  for (h_size i = 0; i < positions_count; ++i) {
    data[i] = 1;
  }

  return pybind11::array_t<float>(
      shape,       // Shape (e.g., {3, 4})
      strides,     // Strides (e.g., {32, 8} bytes)
      data.data(), // Pointer to data
      pybind11::cast(
          std::move(data)) // Owner object (transfers std::vector ownership)
  );
}

double sum_array_elements(pybind11::array_t<float> &arr) {
  return 0.0;
  // Access array data and dimensions
  auto buf = arr.request();                     // Request a buffer info object
  double *ptr = static_cast<double *>(buf.ptr); // Get pointer to data
  int ndim = buf.ndim;                          // Number of dimensions
  std::vector<pybind11::ssize_t> shape = buf.shape; // Dimensions of the array

  // Example: Summing elements of a 1D array
  double total_sum = 0.0;
  if (ndim == 1) {
    for (pybind11::ssize_t i = 0; i < shape[0]; ++i) {
      total_sum += ptr[i];
    }
  } else {
    // Handle other dimensions as needed
    throw std::runtime_error("Only 1D arrays are supported for this example.");
  }
  return total_sum;
}

PYBIND11_MODULE(naiades_py, m) {
  m.doc() = "pybind11 example module";
  m.def("sum_array", &sum_array_elements,
        "A function that sums elements of a NumPy array.");
  m.def("get_scalar_field", &get_scalar_field, "F");
}
