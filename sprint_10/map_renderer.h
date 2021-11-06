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

    //----------------StopPointRender------------------------

    class StopPointRender : svg::Drawable
    {
    public:

        StopPointRender(const svg::Point&, const RenderSettings&);

        void Draw(svg::ObjectContainer&) const override;

    private:

        svg::Point stop_coordinate_;
        const RenderSettings& render_settings_;
    };

    //-----------------TextRender------------------------

    class TextRender : svg::Drawable
    {
    public:

        TextRender(const svg::Point&, const std::string&, const svg::Color&, bool, const RenderSettings&);

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

        RouteRender(const std::vector<svg::Point>&, const svg::Color&, const RenderSettings&);

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

        void SetRenderSettings(const RenderSettings&);

        void AddRoutRender(const std::vector<svg::Point>&, const svg::Color&);

        void AddStopPointRender(const svg::Point&);

        void AddTextRender(const svg::Point&, const std::string&,  const svg::Color&, bool);
        
        svg::Document GetRender() const;

        const std::vector<svg::Color> GetColorPallete() const;

    private:

        std::vector<RouteRender> routs_renders_;
        std::vector<TextRender> routs_names_renders_;
        std::vector<StopPointRender> stops_points_renders_;
        std::vector<TextRender> stops_names_renders_;
        RenderSettings render_settings_;
    };

} // namespace renderer

/*
* "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n  <polyline points=\"25924.4,35685.7 22325.6,22325.6 25924.4,35685.7\" fill=\"none\" stroke=\"rgba(195,60,81,0.624413)\" stroke-width=\"38727.6\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <polyline points=\"22325.6,22325.6 25924.4,35685.7 22325.6,22325.6\" fill=\"none\" stroke=\"rgb(2,81,213)\" stroke-width=\"38727.6\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <polyline points=\"22325.6,22325.6 25924.4,35685.7 22325.6,22325.6\" fill=\"none\" stroke=\"rgba(81,152,19,0.683438)\" stroke-width=\"38727.6\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"25924.4\" y=\"35685.7\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">RkYZl</text>\n  <text fill=\"rgba(195,60,81,0.624413)\" x=\"25924.4\" y=\"35685.7\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">RkYZl</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"22325.6\" y=\"22325.6\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">RkYZl</text>\n  <text fill=\"rgba(195,60,81,0.624413)\" x=\"22325.6\" y=\"22325.6\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">RkYZl</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"22325.6\" y=\"22325.6\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">i</text>\n  <text fill=\"rgb(2,81,213)\" x=\"22325.6\" y=\"22325.6\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">i</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"25924.4\" y=\"35685.7\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">i</text>\n  <text fill=\"rgb(2,81,213)\" x=\"25924.4\" y=\"35685.7\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">i</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"22325.6\" y=\"22325.6\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">jUYheoA3Mcm2 kqTN</text>\n  <text fill=\"rgba(81,152,19,0.683438)\" x=\"22325.6\" y=\"22325.6\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">jUYheoA3Mcm2 kqTN</text>\n  <circle cx=\"22325.6\" cy=\"22325.6\" r=\"21462.7\" fill=\"white\"/>\n  <circle cx=\"25924.4\" cy=\"35685.7\" r=\"21462.7\" fill=\"white\"/>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"22325.6\" y=\"22325.6\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">gtKSYiTpuO3KjmLenbqOj7iO</text>\n  <text fill=\"black\" x=\"22325.6\" y=\"22325.6\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">gtKSYiTpuO3KjmLenbqOj7iO</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"25924.4\" y=\"35685.7\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">vtkKOKMLWRQv</text>\n  <text fill=\"black\" x=\"25924.4\" y=\"35685.7\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">vtkKOKMLWRQv</text>\n</svg>",
* "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n  <polyline points=\"25924.4,35685.7 22325.6,22325.6 25924.4,35685.7\" fill=\"none\" stroke=\"rgba(195,60,81,0.624413)\" stroke-width=\"38727.6\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <polyline points=\"22325.6,22325.6 25924.4,35685.7 22325.6,22325.6\" fill=\"none\" stroke=\"rgb(2,81,213)\" stroke-width=\"38727.6\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <polyline points=\"22325.6,22325.6 25924.4,35685.7 22325.6,22325.6\" fill=\"none\" stroke=\"rgba(81,152,19,0.683438)\" stroke-width=\"38727.6\" stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"25924.4\" y=\"35685.7\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">RkYZl</text>\n  <text fill=\"rgba(195,60,81,0.624413)\" x=\"25924.4\" y=\"35685.7\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">RkYZl</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"22325.6\" y=\"22325.6\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">RkYZl</text>\n  <text fill=\"rgba(195,60,81,0.624413)\" x=\"22325.6\" y=\"22325.6\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">RkYZl</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"22325.6\" y=\"22325.6\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">jUYheoA3Mcm2 kqTN</text>\n  <text fill=\"rgb(2,81,213)\" x=\"22325.6\" y=\"22325.6\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">jUYheoA3Mcm2 kqTN</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"22325.6\" y=\"22325.6\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">i</text>\n  <text fill=\"rgba(81,152,19,0.683438)\" x=\"22325.6\" y=\"22325.6\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">i</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"25924.4\" y=\"35685.7\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">i</text>\n  <text fill=\"rgba(81,152,19,0.683438)\" x=\"25924.4\" y=\"35685.7\" dx=\"59718.9\" dy=\"15913.5\" font-size=\"78497\" font-family=\"Verdana\" font-weight=\"bold\">i</text>\n  <circle cx=\"25924.4\" cy=\"35685.7\" r=\"21462.7\" fill=\"white\"/>\n  <circle cx=\"22325.6\" cy=\"22325.6\" r=\"21462.7\" fill=\"white\"/>\n  <circle cx=\"22325.6\" cy=\"22325.6\" r=\"21462.7\" fill=\"white\"/>\n  <circle cx=\"25924.4\" cy=\"35685.7\" r=\"21462.7\" fill=\"white\"/>\n  <circle cx=\"22325.6\" cy=\"22325.6\" r=\"21462.7\" fill=\"white\"/>\n  <circle cx=\"22325.6\" cy=\"22325.6\" r=\"21462.7\" fill=\"white\"/>\n  <circle cx=\"25924.4\" cy=\"35685.7\" r=\"21462.7\" fill=\"white\"/>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"25924.4\" y=\"35685.7\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">vtkKOKMLWRQv</text>\n  <text fill=\"black\" x=\"25924.4\" y=\"35685.7\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">vtkKOKMLWRQv</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"22325.6\" y=\"22325.6\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">gtKSYiTpuO3KjmLenbqOj7iO</text>\n  <text fill=\"black\" x=\"22325.6\" y=\"22325.6\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">gtKSYiTpuO3KjmLenbqOj7iO</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"22325.6\" y=\"22325.6\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">gtKSYiTpuO3KjmLenbqOj7iO</text>\n  <text fill=\"black\" x=\"22325.6\" y=\"22325.6\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">gtKSYiTpuO3KjmLenbqOj7iO</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"25924.4\" y=\"35685.7\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">vtkKOKMLWRQv</text>\n  <text fill=\"black\" x=\"25924.4\" y=\"35685.7\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">vtkKOKMLWRQv</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"22325.6\" y=\"22325.6\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">gtKSYiTpuO3KjmLenbqOj7iO</text>\n  <text fill=\"black\" x=\"22325.6\" y=\"22325.6\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">gtKSYiTpuO3KjmLenbqOj7iO</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"22325.6\" y=\"22325.6\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">gtKSYiTpuO3KjmLenbqOj7iO</text>\n  <text fill=\"black\" x=\"22325.6\" y=\"22325.6\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">gtKSYiTpuO3KjmLenbqOj7iO</text>\n  <text fill=\"coral\" stroke=\"coral\" stroke-width=\"34006.7\" stroke-linecap=\"round\" stroke-linejoin=\"round\" x=\"25924.4\" y=\"35685.7\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">vtkKOKMLWRQv</text>\n  <text fill=\"black\" x=\"25924.4\" y=\"35685.7\" dx=\"-23192\" dy=\"92100.2\" font-size=\"86988\" font-family=\"Verdana\">vtkKOKMLWRQv</text>\n</svg>",
*/