#pragma once
#include "svg.h"
#include "geo.h"
#include "transport_catalogue.h"
#include <vector>

namespace map_renderer
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

    bool IsZero(double value);

    //----------------SphereProjector----------------------------

    //класс, который проецирует точки на карту
    class SphereProjector
    {
    public:

        template <typename PointInputIt>
        SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height, double padding);

        svg::Point operator()(geo::Coordinates coords) const;

    private:

        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };

    template<typename PointInputIt>
    inline SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height, double padding)
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

    //------------------RouteRenderer--------------------------

    class RouteRenderer : svg::Drawable
    {
    public:

        RouteRenderer(const svg::Color& stroke_color_, const double stroke_width_, const std::vector<svg::Point>& stops_points_);

        void Draw(svg::ObjectContainer& container) const override;

    private:

        svg::Color stroke_color_;
        double stroke_width_;
        std::vector<svg::Point> stops_points_;
        --------------
    };

    //------------------MapRenderer--------------------------

    /*рисовалка, хранит настройки визуализации
    обрашается к методам классов визуализаторов отдельных частей карты*/
    class MapRenderer
    {
    public:

        void SetRenderSettings(const RenderSettings&);

        void CreateRender(transport_catalogue::TransportCatalogue&);

        void SetRouts(const std::vector<RouteRenderer> routs);

        svg::Document Render(const transport_catalogue::TransportCatalogue&);

    private:

        RenderSettings render_settings_;
        std::vector<RouteRenderer> routs_;

        const std::vector<RouteRenderer> RoursLineRender(const transport_catalogue::TransportCatalogue&, const map_renderer::SphereProjector&);
    };
} // namespace map_renderer