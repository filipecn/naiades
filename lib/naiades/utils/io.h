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
#include <naiades/geo/point_set.h>
#include <naiades/numeric/spatial_discretization.h>
#include <naiades/numeric/stencil.h>
#include <naiades/spatial/morton_tree.h>

#include <hermes/colors/color.h>
#include <hermes/geometry/transform.h>
#include <hermes/numeric/interpolation.h>

#include <filesystem>
#include <source_location>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtemplate-id-cdtor"
#include <simple_svg_1.0.0.hpp>

#pragma GCC diagnostic pop

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

enum class draw_option_bits : u32 {
  none = 0,
  vertices = 1 << 0,
  faces = 1 << 1,
  cells = 1 << 2,
  indices = 1 << 3,
  normals = 1 << 4,
  values = 1 << 5,
  all = 0xff
};

using draw_options = hermes::Flags<utils::io::draw_option_bits>;
} // namespace naiades::utils::io

namespace hermes {

template <> struct FlagTraits<naiades::utils::io::draw_option_bits> {
  static HERMES_CONST_OR_CONSTEXPR bool is_bitmask = true;
  static HERMES_CONST_OR_CONSTEXPR naiades::utils::io::draw_options all_flags =
      naiades::utils::io::draw_option_bits::none |
      naiades::utils::io::draw_option_bits::vertices |
      naiades::utils::io::draw_option_bits::faces |
      naiades::utils::io::draw_option_bits::cells |
      naiades::utils::io::draw_option_bits::normals |
      naiades::utils::io::draw_option_bits::values |
      naiades::utils::io::draw_option_bits::indices;
};

#ifdef NAIADES_INCLUDE_DEBUG_TRAITS

template <> struct DebugTraits<naiades::utils::io::draw_options> {
  static HERMES_CONST_OR_CONSTEXPR bool is_string_serializable = true;
  static DebugMessage message(const naiades::utils::io::draw_options &data) {
    DebugMessage m;
    std::vector<std::string> values;
#define CHECK_BIT(B)                                                           \
  if (naiades::utils::io::draw_option_bits::B & data)                          \
  values.push_back(#B)
    CHECK_BIT(vertices);
    CHECK_BIT(faces);
    CHECK_BIT(cells);
    CHECK_BIT(indices);
    CHECK_BIT(normals);
    CHECK_BIT(values);
    return DebugMessage("{}", hermes::cstr::join(values, " | "));
#undef CHECK_BIT
  }
};

#endif

} // namespace hermes

namespace naiades::utils::io {

class SVG {
  static svg::Color toSVG(const hermes::colors::RGB_Color &color) {
    return svg::Color(color.r * 255, color.g * 255, color.b * 255);
  }
  static svg::Point toSVG(const hermes::geo::point2 &p,
                          const hermes::geo::vec2 &svg_offset = {}) {
    HERMES_UNUSED_VARIABLE(svg_offset);
    return svg::Point(p.x, p.y);
  }

public:
  struct Element {
    virtual hermes::geo::bounds::bbox2 bounds() const = 0;
    virtual std::shared_ptr<svg::Shape>
    shape(const hermes::geo::Transform2 &t,
          const hermes::geo::Transform2 &svg_transform) const = 0;
    svg::Fill fill() const {
      return fill_color ? svg::Fill(toSVG(fill_color.value())) : svg::Fill();
    }
    svg::Stroke stroke() const { return svg::Stroke(1.0, toSVG(stroke_color)); }

    std::optional<hermes::colors::RGB_Color> fill_color;
    hermes::colors::RGB_Color stroke_color;
  };

  struct Point : public Element {
    real_t radius;
    hermes::geo::point2 center;
    hermes::geo::bounds::bbox2 bounds() const override {
      return {center - hermes::geo::vec2(radius),
              center + hermes::geo::vec2(radius)};
    }
    std::shared_ptr<svg::Shape>
    shape(const hermes::geo::Transform2 &t,
          const hermes::geo::Transform2 &svg_transform) const override {
      HERMES_UNUSED_VARIABLE(svg_transform);
      return std::shared_ptr<svg::Shape>(
          new svg::Circle(toSVG(t(center)), radius * 2, fill(), stroke()));
    }
  };

  struct Arrow : public Element {
    hermes::geo::point2 p;
    hermes::geo::vec2 v;
    hermes::geo::bounds::bbox2 bounds() const override {
      hermes::geo::bounds::bbox2 bbox;
      bbox = hermes::geo::bounds::make_union(bbox, p);
      bbox = hermes::geo::bounds::make_union(bbox, p + v);
      return bbox;
    }
    std::shared_ptr<svg::Shape>
    shape(const hermes::geo::Transform2 &t,
          const hermes::geo::Transform2 &svg_transform) const override {
      //             _____
      //          / \      h
      //         -   -____
      //          | |
      //          | |     |v| - h
      //          | |______
      //           | |
      //            w

      std::vector<hermes::geo::point2> vertices;
      // control w and h in pixel space and transform to object space
      auto wh = svg_transform(hermes::geo::vec2(5, 7.5));
      f32 w = wh.x;
      f32 h = wh.y;
      f32 half_w = w * 0.5f;
      f32 rod = v.length() - h;
      auto d = hermes::geo::normalize(v);
      // right side
      auto right = d.right();
      vertices.emplace_back(p + half_w * right);
      vertices.emplace_back(p + half_w * right + d * rod);
      vertices.emplace_back(p + d * rod + w * right);
      vertices.emplace_back(p + v);
      // make the inverse on the left side
      auto left = d.left();
      vertices.emplace_back(p + d * rod + w * left);
      vertices.emplace_back(p + d * rod + half_w * left);
      vertices.emplace_back(p + half_w * left);

      std::shared_ptr<svg::Shape> shape(new svg::Polygon(fill(), stroke()));
      svg::Polygon *border = reinterpret_cast<svg::Polygon *>(shape.get());
      for (const auto &position : vertices) {
        *border << toSVG(t(position));
      }
      return shape;
    }
  };

  struct Polygon : public Element {
    hermes::geo::bounds::bbox2 bounds() const override {
      hermes::geo::bounds::bbox2 bbox;
      for (const auto &p : vertices)
        bbox += p;
      return bbox;
    }
    std::shared_ptr<svg::Shape>
    shape(const hermes::geo::Transform2 &t,
          const hermes::geo::Transform2 &svg_transform) const override {
      HERMES_UNUSED_VARIABLE(svg_transform);
      std::shared_ptr<svg::Shape> shape(new svg::Polygon(fill(), stroke()));
      svg::Polygon *border = reinterpret_cast<svg::Polygon *>(shape.get());
      for (const auto &position : vertices) {
        *border << toSVG(t(position));
      }
      return shape;
    }
    std::vector<hermes::geo::point2> vertices;
  };

  struct Polyline : public Element {
    hermes::geo::bounds::bbox2 bounds() const override {
      hermes::geo::bounds::bbox2 bbox;
      for (const auto &p : vertices)
        bbox += p;
      return bbox;
    }
    std::shared_ptr<svg::Shape>
    shape(const hermes::geo::Transform2 &t,
          const hermes::geo::Transform2 &svg_transform) const override {
      HERMES_UNUSED_VARIABLE(svg_transform);
      std::shared_ptr<svg::Shape> shape(new svg::Polyline(fill(), stroke()));
      svg::Polygon *border = reinterpret_cast<svg::Polygon *>(shape.get());
      for (const auto &position : vertices) {
        *border << toSVG(t(position));
      }
      return shape;
    }
    std::vector<hermes::geo::point2> vertices;
  };

  struct Text : public Element {
    Text(float text_size, hermes::geo::vec2 offset = {8, 8}) : offset(offset) {
      font = svg::Font(text_size, "Verdana");
    }
    hermes::geo::bounds::bbox2 bounds() const override {
      hermes::geo::bounds::bbox2 bbox;
      bbox += position;
      return bbox;
    }
    std::shared_ptr<svg::Shape>
    shape(const hermes::geo::Transform2 &t,
          const hermes::geo::Transform2 &svg_transform) const override {
      HERMES_UNUSED_VARIABLE(svg_transform);
      return std::shared_ptr<svg::Shape>(new svg::Text(
          toSVG(t(position) + offset), text.c_str(), fill(), font));
    }
    hermes::geo::point2 position;
    std::string text;
    svg::Font font;
    hermes::geo::vec2 offset;
  };

  using Command =
      std::variant<std::shared_ptr<Element>, hermes::geo::Transform2>;

  SVG &setOptions(draw_options options) {
    draw_options_ = options;
    return *this;
  }
  SVG &disable(draw_options options) {
    draw_options_ &= ~options;
    return *this;
  }
  SVG &enable(draw_options options) {
    draw_options_ |= options;
    return *this;
  }
  SVG &setTextSize(h_index size) {
    text_size_ = size;
    return *this;
  }
  SVG &setPointSize(f32 size) {
    point_size_ = size;
    return *this;
  }
  template <typename T>
    requires core::HasGeometry<T> && core::HasTopology<T>
  SVG &draw(const T *mesh) {
    // edges
    for (const auto &face : mesh->elements(core::Element::face())) {
      auto vertices =
          mesh->elementIndices(face.globalIndex(), core::Element::vertex());
      link(mesh->center(core::ElementIndex::global(core::Element::vertex(),
                                                   vertices[0])),
           mesh->center(core::ElementIndex::global(core::Element::vertex(),
                                                   vertices[1])),
           bg_color);
    }
    // for (const auto &cell_vertices :
    //      mesh->indices(core::Element::cell(), core::Element::vertex())) {
    //   std::vector<hermes::geo::point2> positions;
    //   for (auto vertex_index : cell_vertices)
    //     positions.emplace_back(
    //         mesh->center(core::Element::vertex(), vertex_index));
    //   doc_ << cell(positions);
    // }
    // vertices
    if (draw_options_.contain(draw_option_bits::vertices))
      for (const auto &vertex : mesh->elements(core::Element::vertex())) {
        if (draw_options_.contain(draw_option_bits::indices)) {
          text(hermes::cstr::format("{}", vertex.local_index), vertex.center,
               bg_color);
        }
        point(vertex.center, point_size_, bg_color);
      }
    // cells
    if (draw_options_.contain(draw_option_bits::cells))
      for (const auto &cell : mesh->elements(core::Element::cell())) {
        if (draw_options_.contain(draw_option_bits::indices))
          text(hermes::cstr::format("{}", cell.local_index), cell.center,
               z_color);
        point(cell.center, point_size_, z_color);
      }
    // faces
    if (draw_options_.contain(draw_option_bits::faces))
      for (const auto &face : mesh->elements(core::Element::face())) {
        if (draw_options_.contain(draw_option_bits::indices))
          text(hermes::cstr::format("{}", face.local_index), face.center,
               x_color);
        point(face.center, point_size_, x_color);
        // normal
        if (draw_options_.contain(draw_option_bits::normals))
          arrow(face.center,
                vector_scale_ *
                    hermes::geo::vec2(mesh->normal(face.globalIndex())),
                x_color);
      }

    // // u faces
    // for (const auto &face : mesh->elements(core::Element::uFace())) {
    //   doc_ << text(
    //       hermes::cstr::format("{}({})", face.global_index,
    //       face.local_index), face.center, x_color);
    //   doc_ << svg::Circle(pos(face.center), point_size_, svg::Fill(x_color),
    //                       svg::Stroke(1, x_color));
    //   // normal
    //   doc_ << arrow(face.center,
    //                 vector_scale_ *
    //                     hermes::geo::vec2(mesh->normal(core::Element::uFace(),
    //                                                   face.global_index)),
    //                 x_color);
    // }
    // // v faces
    // for (const auto &face : mesh->elements(core::Element::vFace())) {
    //   doc_ << text(
    //       hermes::cstr::format("{}({})", face.global_index,
    //       face.local_index), face.center, y_color);
    //   doc_ << svg::Circle(pos(face.center), point_size_, svg::Fill(y_color),
    //                       svg::Stroke(1, y_color));
    //   // normal
    //   doc_ << arrow(face.center,
    //                 vector_scale_ *
    //                     hermes::geo::vec2(mesh->normal(core::Element::vFace(),
    //                                                   face.global_index)),
    //                 y_color);
    // }

    return *this;
  }
  SVG &draw(const core::Mesh2 &mesh, const core::FieldCRef<f32> &values) {
    auto palette = colors::palettes::batlow();
    // get min and max values
    auto value_range = values.valueRange();
    if (values.element().is(core::element_primitive_bits::cell)) {
      // cells
      h_index cell_index = 0;
      for (const auto &cell_vertices :
           mesh.subElements(core::Element::cell(), core::Element::vertex())) {
        std::vector<hermes::geo::point2> positions;
        for (auto vertex_index : cell_vertices)
          positions.emplace_back(mesh.center(core::ElementIndex::global(
              core::Element::vertex(), vertex_index)));
        // compute color
        polygon(positions,
                palette(hermes::numeric::smoothStep(
                    value_range.low, value_range.high, values[cell_index++])));
      }
    } else if (values.element().is(core::element_primitive_bits::vertex)) {
      for (const auto &vertex : mesh.elements(core::Element::vertex())) {

        auto color = palette(hermes::numeric::smoothStep(
            value_range.low, value_range.high, values[vertex.global_index]));
        point(vertex.center, point_size_, color);
      }
    }
    return *this;
  }
  SVG &draw(const core::Mesh2 *mesh, const core::Element &loc,
            const numeric::Scalar &values) {
    auto palette = colors::palettes::batlow();
    // get min and max values
    auto value_range = values.valueRange();
    h_index cell_index = 0;
    if (loc.is(core::element_primitive_bits::cell)) {
      // cells
      for (const auto &cell_vertices :
           mesh->subElements(core::Element::cell(), core::Element::vertex())) {
        std::vector<hermes::geo::point2> positions;
        for (auto vertex_index : cell_vertices)
          positions.emplace_back(mesh->center(core::ElementIndex::global(
              core::Element::vertex(), vertex_index)));
        // compute color
        polygon(positions,
                palette(hermes::numeric::smoothStep(
                    value_range.low, value_range.high, values[cell_index++])));
      }
    }
    return *this;
  }
  SVG &drawText(const core::Mesh2 &mesh, const core::Element &loc,
                const numeric::Scalar &values) {
    for (auto e : mesh.elements(loc)) {
      text(hermes::cstr::format("{}", values[e.global_index]), e.center,
           bg_color);
    }
    return *this;
  }
  SVG &draw(const core::Mesh2 &mesh, const numeric::DiscreteOperator &dop,
            const core::DiscreteSymbol &sym) {
    h_index center_index = dop.centerIndex();
    auto center =
        mesh.center(core::ElementIndex::global(sym.symbol.loc, center_index));
    for (const auto &item : dop.nodes()) {
      if (item.first == center_index) {
      } else {
        auto node_center =
            mesh.center(core::ElementIndex::global(sym.symbol.loc, item.first));
        link(center, node_center, bg_color);
        if (draw_options_.contain(draw_option_bits::values))
          text(hermes::cstr::format("{}", item.second),
               0.5f * (center + hermes::geo::vec2(node_center)), bg_color);
      }
    }
    for (const auto &item : dop.boundaryNodes()) {
      if (item.first == center_index) {
      } else {
        auto node_center = mesh.center(
            core::ElementIndex::global(sym.boundary_symbol.loc, item.first));
        link(center, node_center, z_color);
        if (draw_options_.contain(draw_option_bits::values))
          text(hermes::cstr::format("{}", item.second),
               0.5f * (center + hermes::geo::vec2(node_center)), bg_color);
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
        auto p = mesh.center(core::ElementIndex::global(
            boundary.boundaryElement(), item.global_index));
        point(p, point_size_ * 1.5, {});
        const auto &stencil =
            region.stencil(Index::local(item.local_set_index));
        draw(mesh, stencil,
             core::DiscreteSymbol("", boundary.interiorElement(),
                                  boundary.boundaryElement()));
      }
    }
    return *this;
  }
  SVG &draw(const core::Mesh2 &mesh, const std::vector<core::Neighbour> &star) {
    if (star.empty())
      return *this;
    auto center = mesh.center(star[0].element_index);
    point(center, point_size_, bg_color);
    for (h_index i = 1; i < star.size(); ++i) {
      auto n_center = mesh.center(star[i].element_index);
      link(center, n_center, neighbor_color);
    }
    return *this;
  }
  SVG &draw(const geo::HE2 &mesh) {

    auto drawHE = [&](const auto &face, h_index he,
                      const hermes::colors::RGB_Color &color) {
      HERMES_UNUSED_VARIABLE(face);
      auto v = mesh.heVector(he);
      auto v_size = v.length();
      auto l = hermes::geo::normalize(v.left()) * v_size * 0.02f;
      auto o = mesh.heOriginPosition(he) + v * 0.4f;
      if (draw_options_.contain(draw_option_bits::indices))
        text(hermes::cstr::format("{}[{}]{}", mesh.hePrev(he), he,
                                  mesh.heNext(he)),
             o + l * 2.5f, color);
      // arrow(o + l, v * 0.3f, color);
    };
    // faces
    for (const auto &face : mesh.elements(core::Element::face())) {
      // first half-edge
      drawHE(face, face.global_index * 2, y_color);
      drawHE(face, face.global_index * 2 + 1, z_color);
      link(mesh.heOriginPosition(face.global_index * 2),
           mesh.heOriginPosition(face.global_index * 2 + 1), x_color);
    }
    // vertices
    for (const auto &vertex : mesh.elements(core::Element::vertex())) {
      if (draw_options_.contain(draw_option_bits::indices))
        text(hermes::cstr::format("{}", vertex.local_index), vertex.center,
             bg_color);
      point(vertex.center, point_size_, bg_color);
    }

    return *this;
  }
  SVG &draw(const spatial::MortonTree2 &mt) {
    for (auto ij : mt.indexBounds()) {
      point(mt.position(ij), point_size_, x_color);
    }
    for (auto leaf : mt) {
      auto vertices = leaf.bounds.corners();
      polygon(vertices);
      text(hermes::cstr::format("L{}[{}]", leaf.level, leaf.z_index),
           leaf.bounds.corner(0), y_color);
    }

    // draw bitset below
    auto bitset_size = mt.maxResolution() * mt.maxResolution();
    auto step = mt.maxResolution() / static_cast<f32>(bitset_size);
    auto origin = hermes::geo::point2(0.0, -1.0);
    for (h_index i = 0; i < bitset_size; ++i) {
      auto vertices = hermes::geo::bounds::bbox2(
                          origin + hermes::geo::vec2(i * step, 0.0),
                          origin + hermes::geo::vec2((i + 1) * step, step))
                          .corners();
      if (mt.isActive(i))
        polygon(vertices, x_color);
      else
        polygon(vertices);
      text(hermes::cstr::format("[{}]", i), vertices[3], y_color);
    }
    return *this;
  }
  SVG &draw(const geo::PointSet2 &ps) {
    HERMES_UNUSED_VARIABLE(ps);
    HERMES_NOT_IMPLEMENTED;
    // for (const auto &p : ps)
    //   point(p, point_size_, bg_color);
    return *this;
  }
  SVG &draw(const numeric::Stencil2 &stencil) {
    for (h_index i = 1; i < stencil.size(); ++i) {
      link(stencil[0], stencil[i], x_color);
    }
    for (h_index i = 0; i < stencil.size(); ++i) {
      point(stencil[i], point_size_, y_color);
      if (i == 0)
        text(hermes::cstr::format("({},{:.2})", stencil[i].x, stencil[i].y),
             stencil[i], z_color);
      else
        text(hermes::cstr::format("({})", i), stencil[i], z_color);
    }
    return *this;
  }
  void write(const std::filesystem::path &path,
             hermes::size2 resolution = {1500, 1500}) {
    // compute bounds
    hermes::geo::bounds::bbox2 bounds;
    for (auto command : commands_)
      if (std::holds_alternative<std::shared_ptr<Element>>(command))
        bounds += std::get<std::shared_ptr<Element>>(command)->bounds();

    auto aspect_ratio = bounds.size(0) / bounds.size(1);
    resolution.width = resolution.height * aspect_ratio;

    svg::Dimensions dimensions(resolution.width, resolution.height);
    svg::Document doc(path.string(),
                      svg::Layout(dimensions, svg::Layout::Origin::BottomLeft));

    HERMES_LOG_VARIABLE(aspect_ratio);
    HERMES_LOG_VARIABLE(resolution);
    // compute world transform
    auto extended_bounds = bounds.extends() * (1.0f + margin_percent_);
    HERMES_LOG_VARIABLE(bounds.extends());
    HERMES_LOG_VARIABLE(extended_bounds);
    auto origin_translate = hermes::geo::Transform2::translate(
        -1.f * hermes::geo::vec2(bounds.center()));
    auto unit_scale = hermes::geo::Transform2::scale(1.0f / extended_bounds);
    auto center_translate = hermes::geo::Transform2::translate(
        hermes::geo::vec2(resolution.width, resolution.height) / 2.0);
    auto scale = hermes::geo::Transform2::scale(
        hermes::geo::vec2(resolution.width, resolution.height));
    auto world_transform =
        center_translate * scale * unit_scale * origin_translate;
    auto svg_transform = hermes::geo::Transform2::scale(
        hermes::geo::vec2(1. / resolution.width, 1. / resolution.height));

    // draw into doc
    for (auto command : commands_) {
      if (std::holds_alternative<std::shared_ptr<Element>>(command)) {
        doc << *std::get<std::shared_ptr<Element>>(command)->shape(
            world_transform, svg_transform);
      }
    }

    if (doc.save()) {
      HERMES_INFO("SVG {} saved.", path.string());
    }
  }

  SVG &arrow(const hermes::geo::point2 &p, const hermes::geo::vec2 &v,
             const hermes::colors::RGB_Color &color) {
    std::shared_ptr<Element> shape(new Arrow());
    shape->fill_color = color;
    Arrow *border = reinterpret_cast<Arrow *>(shape.get());
    border->p = p;
    border->v = v;
    commands_.emplace_back(shape);
    return *this;
  }
  SVG &polygon(const std::vector<hermes::geo::point2> &positions) {
    std::shared_ptr<Element> shape(new Polygon());
    shape->stroke_color = bg_color;
    Polygon *border = reinterpret_cast<Polygon *>(shape.get());
    for (const auto &position : positions) {
      border->vertices.emplace_back(position);
    }
    commands_.emplace_back(shape);
    return *this;
  }
  SVG &polygon(const std::vector<hermes::geo::point2> &positions,
               const hermes::colors::RGB_Color &color) {
    std::shared_ptr<Element> shape(new Polygon());
    shape->fill_color = color;
    shape->stroke_color = bg_color;
    Polygon *border = reinterpret_cast<Polygon *>(shape.get());
    for (const auto &position : positions) {
      border->vertices.emplace_back(position);
    }
    commands_.emplace_back(shape);
    return *this;
  }
  SVG &point(const hermes::geo::point2 &center, f32 radius) {
    std::shared_ptr<Element> shape(new Point());
    shape->stroke_color = bg_color;
    Point *border = reinterpret_cast<Point *>(shape.get());
    border->center = center;
    border->radius = radius;
    commands_.emplace_back(shape);
    return *this;
  }
  SVG &point(const hermes::geo::point2 &center, f32 radius,
             const hermes::colors::RGB_Color &color) {
    std::shared_ptr<Element> shape(new Point());
    shape->fill_color = color;
    shape->stroke_color = color;
    Point *border = reinterpret_cast<Point *>(shape.get());
    border->center = center;
    border->radius = radius;
    commands_.emplace_back(shape);
    return *this;
  }
  SVG &text(const std::string &s, const hermes::geo::point2 &position,
            const hermes::colors::RGB_Color &color) {
    std::shared_ptr<Element> shape(new Text(text_size_));
    shape->fill_color = color;
    Text *border = reinterpret_cast<Text *>(shape.get());
    border->position = position;
    border->text = s;
    commands_.emplace_back(shape);
    return *this;
  }
  SVG &link(const hermes::geo::point2 &a, const hermes::geo::point2 &b,
            const hermes::colors::RGB_Color &color) {
    std::shared_ptr<Element> shape(new Polyline());
    shape->fill_color = color;
    shape->stroke_color = color;
    auto polyline = reinterpret_cast<Polyline *>(shape.get());
    polyline->vertices.emplace_back(a);
    polyline->vertices.emplace_back(b);
    commands_.emplace_back(shape);
    return *this;
  }

  static inline hermes::colors::RGB_Color x_color{236 / 255.0f, 143 / 255.0f,
                                                  141 / 255.0f};
  static inline hermes::colors::RGB_Color y_color{83 / 255.0f, 125 / 255.0f,
                                                  150 / 255.0f};
  static inline hermes::colors::RGB_Color z_color{68 / 255.0f, 161 / 255.0f,
                                                  148 / 255.0f};
  static inline hermes::colors::RGB_Color bg_color{244 / 255.0f, 240 / 255.0f,
                                                   228 / 255.0f};
  static inline hermes::colors::RGB_Color neighbor_color{
      244 / 255.0f, 240 / 255.0f, 11 / 255.0f};

private:
  draw_options draw_options_{draw_option_bits::vertices |
                             draw_option_bits::faces | draw_option_bits::cells |
                             draw_option_bits::indices};

  std::vector<Command> commands_;

  f32 margin_percent_{0.1f};
  f32 vector_scale_{0.01f};
  f32 point_size_{0.001f};
  h_index text_size_{11};
  // palette
};

class PlotManager {
public:
  PlotManager(const PlotManager &) = delete;
  PlotManager(PlotManager &&) = delete;
  PlotManager &operator=(const PlotManager &) = delete;
  PlotManager &operator=(PlotManager &&) = delete;
  ~PlotManager() = default;

  static PlotManager &instance() {
    static PlotManager plot_manager;
    return plot_manager;
  }

  static void plot(const std::string &name,
                   const std::function<void(SVG &)> &func) {
    auto &plot_manager = PlotManager::instance();
    auto plot_count = plot_manager.registerPlot(name);
    SVG svg;
    func(svg);
    svg.write(name + "_" + std::to_string(plot_count) + ".svg");
  }

private:
  h_size registerPlot(const std::string &name) { return plot_counts[name]++; }

  PlotManager() = default;
  /// plot name -> plot count
  std::unordered_map<std::string, h_size> plot_counts;
};

} // namespace naiades::utils::io

#ifndef NAIADES_PLOT
#define NAIADES_PLOT(COMMAND)                                                  \
  ::naiades::utils::io::PlotManager::plot(                                     \
      std::filesystem::path{std::source_location::current().file_name()}       \
              .stem()                                                          \
              .string() +                                                      \
          std::to_string(std::source_location::current().line()),              \
      [&](::naiades::utils::io::SVG &svg) { svg.COMMAND; });
#endif

#ifndef NAIADES_PLOT_IF
#define NAIADES_PLOT_IF(A, COMMAND)                                            \
  if (A)                                                                       \
    naiades::utils::io::PlotManager::plot(                                     \
        std::filesystem::path{std::source_location::current().file_name()}     \
                .stem()                                                        \
                .string() +                                                    \
            std::to_string(std::source_location::current().line()),            \
        [&](::naiades::utils::io::SVG &svg) { svg.COMMAND; });
#endif
