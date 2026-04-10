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

/// \file   io.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date   2026-03-27

#pragma once

#include <naiades/core/mesh.h>
#include <naiades/geo/he.h>
#include <naiades/numeric/spatial_discretization.h>

#include <hermes/colors/color.h>
#include <hermes/geometry/transform.h>
#include <hermes/numeric/interpolation.h>

#include <simple_svg_1.0.0.hpp>

#include <filesystem>

namespace naiades::utils::colors {

/// \brief Linearly interpolates between two colors
/// \param t
/// \param a
/// \param b
/// \return
inline hermes::colors::RGB_Color mix(float t,
                                     const hermes::colors::RGB_Color &a,
                                     const hermes::colors::RGB_Color &b) {
  return {hermes::numeric::lerp(t, a.r, b.r),
          hermes::numeric::lerp(t, a.g, b.g),
          hermes::numeric::lerp(t, a.b, b.b)};
}

class ColorPalette {
public:
  /// \brief Empty color palette constructor
  ColorPalette();
  /// \brief u32 data constructor
  /// \param c
  /// \param n
  explicit ColorPalette(const u32 *c, size_t n);
  /// \brief f32 data constructor
  /// \param c
  /// \param rgb_count
  explicit ColorPalette(const f32 *c, size_t rgb_count);
  /// \brief integer list data constructor
  /// \param c
  // ColorPalette(std::initializer_list<int> c);
  /// \brief float list data constructor
  /// \param c
  // ColorPalette(std::initializer_list<double> c);
  /// \brief Get color from parametric coordinate
  /// \param t
  /// \param alpha
  /// \return
  inline hermes::colors::RGB_Color operator()(float t) const {
    t = hermes::numbers::clamp(t, 0.f, 1.f);
    float ind =
        hermes::numeric::lerp(t, 0.f, static_cast<float>(colors.size() - 1));
    float r = std::abs(hermes::numbers::fract(ind));
    hermes::colors::RGB_Color c;

    auto upper = hermes::numbers::ceil2Int(ind);
    auto lower = hermes::numbers::floor2Int(ind);

    if (upper >= static_cast<int>(colors.size()))
      c = colors[colors.size() - 1];
    else if (lower < 0)
      c = colors[0];
    else if (lower == upper)
      c = colors[lower];
    else
      c = mix(r, colors[lower], colors[upper]);
    return c;
  }
  std::vector<hermes::colors::RGB_Color> colors; //!< raw color data
};

/// \brief Set of Color Palettes
struct palettes {
  // *******************************************************************************************************************
  //                                                                                                   STATIC METHODS
  // *******************************************************************************************************************
  /// \brief Matlab Heat Map color map
  /// \return
  static ColorPalette matlabHeatMap();
  /// \brief Batlow color map
  /// \return
  static ColorPalette batlow();
};

} // namespace naiades::utils::colors

namespace naiades::utils::io {

class SVG {
public:
  SVG(const std::filesystem::path &path) : path_{path} {}
  SVG &setDimensions(const hermes::geo::bounds::bbox2 &bounds) {
    hermes::geo::vec2 size{1500, 1500};
    auto scale = hermes::geo::Transform2::scale(
        size / (bounds.extends() * (1.0f + margin_percent_)));
    transform_ = scale * hermes::geo::Transform2::translate(hermes::geo::vec2(
                             bounds.extends() * (margin_percent_ / 2.f)));
    inv_scale_ = hermes::geo::inverse(scale);
    dimensions_ = svg::Dimensions(size.x, size.y);
    doc_ = svg::Document(
        path_.string(),
        svg::Layout(dimensions_, svg::Layout::Origin::BottomLeft));
    return *this;
  }
  SVG &draw(const core::Mesh2 &mesh) {
    // edges
    for (const auto &cell_vertices :
         mesh.indices(core::Element::cell(), core::Element::vertex())) {
      std::vector<hermes::geo::point2> positions;
      for (auto vertex_index : cell_vertices)
        positions.emplace_back(
            mesh.center(core::Element::vertex(), vertex_index));
      doc_ << cell(positions);
    }
    // vertices
    for (const auto &vertex : mesh.elements(core::Element::vertex())) {
      doc_ << text(hermes::cstr::format("{}", vertex.local_index),
                   vertex.center, bg_color);
      doc_ << svg::Circle(pos(vertex.center), point_size_, svg::Fill(bg_color),
                          svg::Stroke(1, bg_color));
    }
    // cells
    for (const auto &cell : mesh.elements(core::Element::cell())) {
      doc_ << text(hermes::cstr::format("{}", cell.local_index), cell.center,
                   z_color);
      doc_ << svg::Circle(pos(cell.center), point_size_, svg::Fill(z_color),
                          svg::Stroke(1, z_color));
    }
    // u faces
    for (const auto &face : mesh.elements(core::Element::uFace())) {
      doc_ << text(
          hermes::cstr::format("{}({})", face.global_index, face.local_index),
          face.center, x_color);
      doc_ << svg::Circle(pos(face.center), point_size_, svg::Fill(x_color),
                          svg::Stroke(1, x_color));
      // normal
      doc_ << arrow(face.center,
                    vector_scale_ *
                        hermes::geo::vec2(mesh.normal(core::Element::uFace(),
                                                      face.global_index)),
                    x_color);
    }
    // v faces
    for (const auto &face : mesh.elements(core::Element::vFace())) {
      doc_ << text(
          hermes::cstr::format("{}({})", face.global_index, face.local_index),
          face.center, y_color);
      doc_ << svg::Circle(pos(face.center), point_size_, svg::Fill(y_color),
                          svg::Stroke(1, y_color));
      // normal
      doc_ << arrow(face.center,
                    vector_scale_ *
                        hermes::geo::vec2(mesh.normal(core::Element::vFace(),
                                                      face.global_index)),
                    y_color);
    }

    return *this;
  }
  SVG &draw(const core::Mesh2 &mesh, const core::FieldCRef<f32> &values) {
    auto palette = colors::palettes::batlow();
    // get min and max values
    auto value_range = values.valueRange();
    h_index cell_index = 0;
    if (values.element().is(core::element_primitive_bits::cell)) {
      // cells
      for (const auto &cell_vertices :
           mesh.indices(core::Element::cell(), core::Element::vertex())) {
        std::vector<hermes::geo::point2> positions;
        for (auto vertex_index : cell_vertices)
          positions.emplace_back(
              mesh.center(core::Element::vertex(), vertex_index));
        // compute color
        doc_ << cell(positions, palette(hermes::numeric::smoothStep(
                                    value_range.low, value_range.high,
                                    values[cell_index++])));
      }
    }
    return *this;
  }
  SVG &draw(const core::Mesh2 &mesh, const core::Element &loc,
            const numeric::Scalar &values) {
    auto palette = colors::palettes::batlow();
    // get min and max values
    auto value_range = values.valueRange();
    h_index cell_index = 0;
    if (loc.is(core::element_primitive_bits::cell)) {
      // cells
      for (const auto &cell_vertices :
           mesh.indices(core::Element::cell(), core::Element::vertex())) {
        std::vector<hermes::geo::point2> positions;
        for (auto vertex_index : cell_vertices)
          positions.emplace_back(
              mesh.center(core::Element::vertex(), vertex_index));
        // compute color
        doc_ << cell(positions, palette(hermes::numeric::smoothStep(
                                    value_range.low, value_range.high,
                                    values[cell_index++])));
      }
    }
    return *this;
  }
  SVG &drawText(const core::Mesh2 &mesh, const core::Element &loc,
                const numeric::Scalar &values) {
    for (auto e : mesh.elements(loc)) {
      doc_ << text(hermes::cstr::format("{}", values[e.global_index]), e.center,
                   bg_color);
    }
    return *this;
  }
  SVG &draw(const core::Mesh2 &mesh, const numeric::DiscreteOperator &dop,
            const core::DiscreteSymbol &sym) {
    h_index center_index = dop.centerIndex();
    auto center = mesh.center(sym.symbol.loc, center_index);
    for (const auto &item : dop.nodes()) {
      if (item.first == center_index) {
      } else {
        auto node_center = mesh.center(sym.symbol.loc, item.first);
        doc_ << link(center, node_center, bg_color);
        doc_ << text(hermes::cstr::format("{}", item.second),
                     0.5f * (center + hermes::geo::vec2(node_center)),
                     bg_color);
      }
    }
    for (const auto &item : dop.boundaryNodes()) {
      if (item.first == center_index) {
      } else {
        auto node_center = mesh.center(sym.boundary_symbol.loc, item.first);
        doc_ << link(center, node_center, z_color);
        doc_ << text(hermes::cstr::format("{}", item.second),
                     0.5f * (center + hermes::geo::vec2(node_center)),
                     bg_color);
      }
    }
    return *this;
  }
  SVG &
  draw(const core::Mesh2 &mesh,
       const std::unordered_map<core::Symbol, numeric::Boundary> boundaries) {
    for (const auto &item : boundaries) {
      draw(mesh, item.second);
    }
    return *this;
  }
  SVG &draw(const core::Mesh2 &mesh, const numeric::Boundary &boundary) {
    for (const auto &region : boundary.regions()) {
      for (auto item : region.indices()) {
        auto p = mesh.center(boundary.boundaryElement(), item.global_index);
        doc_ << svg::Circle(pos(p), point_size_ * 1.5, {},
                            svg::Stroke(1, bg_color));
        const auto &stencil =
            region.stencil(core::Index::local(item.local_set_index));
        draw(mesh, stencil,
             core::DiscreteSymbol("", boundary.interiorElement(),
                                  boundary.boundaryElement()));
      }
    }
    return *this;
  }
  SVG &draw(const geo::HE2 &he_mesh) { return *this; }
  void write() {
    if (doc_.save()) {
      HERMES_INFO("SVG {} saved.", path_.string());
    }
  }

private:
  svg::Color toSVG(const hermes::colors::RGB_Color &color) const {
    return svg::Color(color.r * 255, color.g * 255, color.b * 255);
  }
  svg::Polygon arrow(const hermes::geo::point2 &p, const hermes::geo::vec2 &v,
                     const svg::Color &color) const {
    //             _____
    //          / \      h
    //         -   -____
    //          | |
    //          | |     |v| - h
    //          | |______
    //           | |
    //            w

    svg::Polygon border(svg::Fill(color), svg::Stroke(1, color));
    // control w and h in pixel space and transform to object space
    auto wh = inv_scale_(hermes::geo::vec2(5, 7.5));
    f32 w = wh.x;
    f32 h = wh.y;
    f32 half_w = w * 0.5f;
    f32 rod = v.length() - h;
    auto d = hermes::geo::normalize(v);
    // right side
    auto right = d.right();
    border << pos(p + half_w * right);
    border << pos(p + half_w * right + d * rod);
    border << pos(p + d * rod + w * right);
    border << pos(p + v);
    // make the inverse on the left side
    auto left = d.left();
    border << pos(p + d * rod + w * left);
    border << pos(p + d * rod + half_w * left);
    border << pos(p + half_w * left);
    return border;
  }
  svg::Point pos(const hermes::geo::point2 &p,
                 const hermes::geo::vec2 &svg_offset = {}) const {
    auto tp = transform_(p);
    return svg::Point(tp.x + svg_offset.x, tp.y + svg_offset.y);
  }
  svg::Polygon cell(const std::vector<hermes::geo::point2> &positions) const {
    svg::Polygon border(svg::Stroke(1, bg_color));
    for (const auto &position : positions)
      border << pos(position);
    return border;
  }
  svg::Polygon cell(const std::vector<hermes::geo::point2> &positions,
                    const hermes::colors::RGB_Color &color) const {
    svg::Polygon border(svg::Fill(toSVG(color)), svg::Stroke(1, bg_color));
    for (const auto &position : positions)
      border << pos(position);
    return border;
  }
  svg::Text text(const std::string &s, const hermes::geo::point2 &position,
                 const svg::Color &color) const {
    return svg::Text(pos(position, {8.0f, 8.f}), s.c_str(), svg::Fill(color),
                     svg::Font(11, "Verdana"));
  }
  svg::Polyline link(const hermes::geo::point2 &a, const hermes::geo::point2 &b,
                     const svg::Color &color) const {
    return (svg::Polyline(svg::Stroke(1, color)) << pos(a) << pos(b));
  }

  hermes::geo::Transform2 transform_;
  hermes::geo::Transform2 inv_scale_;
  std::filesystem::path path_;
  svg::Document doc_;
  svg::Dimensions dimensions_;
  f32 margin_percent_{0.3f};
  f32 vector_scale_{0.2f};
  f32 point_size_{15.f};
  // palette
  svg::Color x_color{236, 143, 141};
  svg::Color y_color{83, 125, 150};
  svg::Color z_color{68, 161, 148};
  svg::Color bg_color{244, 240, 228};
};

} // namespace naiades::utils::io