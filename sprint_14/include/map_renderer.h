#pragma once

#include "svg.h"
#include "geo.h"
#include "domain.h"

#include <vector>
#include <algorithm>

namespace renderer
{
    //----------------RenderSettings----------------------------

    struct RenderSettings
    {
        double width = 0.0;
        double height = 0.0;
        double padding = 0.0;
        double line_width = 0.0;
        double stop_radius = 0.0;
        int bus_label_font_size = 0;
        svg::Point bus_label_offset{ 0.0, 0.0 };
        int stop_label_font_size = 0;
        svg::Point stop_label_offset{ 0.0, 0.0 };
        svg::Color underlayer_color;
        double underlayer_width = 0.0;
        std::vector<svg::Color> color_palette;
    };

    inline const double EPSILON = 1e-6;

    bool IsZero(const double& value);

    //----------------SphereProjector----------------------------

    //класс, который проецирует точки на карту
    class SphereProjector
    {
    public:

        template <typename PointInputIt>
        SphereProjector(const PointInputIt& points_begin, const PointInputIt& points_end, const double& max_width, const double& max_height, const double& padding);

        svg::Point operator()(const geo::Coordinates& coords) const;

    private:

        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };

    template<typename PointInputIt>
    inline SphereProjector::SphereProjector(const PointInputIt& points_begin, const PointInputIt& points_end, const double& max_width, const double& max_height, const double& padding)
        : padding_(padding)
    {
        if (points_begin == points_end)
        {
            return;
        }

        const auto [left_it, right_it] = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs)
            {
                return lhs.lng < rhs.lng;
            });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        const auto [bottom_it, top_it] = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs)
            {
                return lhs.lat < rhs.lat;
            });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_))
        {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat))
        {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom)
        {
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        }

        else if (width_zoom)
        {
            zoom_coeff_ = *width_zoom;
        }


        else if (height_zoom)
        {
            zoom_coeff_ = *height_zoom;
        }
    }

    //----------------StopPointRender------------------------

    class StopPointRender : svg::Drawable
    {
    public:

        StopPointRender(const svg::Point& stop_coordinate, const RenderSettings& render_settings);

        void Draw(svg::ObjectContainer& container) const override;

    private:

        svg::Point stop_coordinate_;
        const RenderSettings& render_settings_;
    };

    //-----------------TextRender------------------------

    class TextRender : svg::Drawable
    {
    public:

        TextRender(const svg::Point& coordinate, const std::string_view& data, const svg::Color& fill, const bool& this_stop, const RenderSettings& render_settings);

        void Draw(svg::ObjectContainer&) const override;

    private:

        svg::Point coordinate_;
        std::string data_;
        svg::Color fill_;
        bool this_stop_;
        const RenderSettings& render_settings_;

        svg::Text CreateText() const;

        svg::Text CreateBeckgraund() const;
    };

    //----------------RouteRender------------------------

    class RouteRender : svg::Drawable
    {
    public:

        RouteRender(const std::vector<svg::Point>& stops_coordinates, const svg::Color& stroke_color, const RenderSettings& render_settings);

        void Draw(svg::ObjectContainer&) const override;

    private:

        std::vector<svg::Point> stops_coordinates_;
        svg::Color stroke_color_;
        const RenderSettings& render_settings_;
    };

    //---------------MapRenderer-------------------------

    class MapRenderer
    {
    public:

        void SetRenderSettings(const RenderSettings& render_settings);

        const RenderSettings& GetRenderSettings() const;

        void AddRoutRender(const std::vector<svg::Point>& stops_coordinates, const svg::Color& stroke_color);

        void AddStopPointRender(const svg::Point& stop_coordinate);

        void AddTextRender(const svg::Point& stop_coordinate, const std::string& data , const svg::Color& text_color, bool this_stop);

        svg::Document GetRender() const;

        const std::vector<svg::Color>& GetColorPallete() const;

    private:

        std::vector<RouteRender> routs_renders_;
        std::vector<TextRender> routs_names_renders_;
        std::vector<StopPointRender> stops_points_renders_;
        std::vector<TextRender> stops_names_renders_;
        RenderSettings render_settings_;
    };

} // namespace renderer

//program.exe < input.json