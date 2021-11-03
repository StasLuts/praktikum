#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

namespace renderer
{
	//----------------SphereProjector----------------------------

	bool IsZero(double value)
	{
		return std::abs(value) < EPSILON;
	}

	svg::Point SphereProjector::operator()(geo::Coordinates coords) const
	{
		return { (coords.lng - min_lon_) * zoom_coeff_ + padding_, (max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
	}

	//-----------------RouteNamesRender------------------------

	RouteNamesRender::RouteNamesRender(const svg::Point& stop_coordinate,
		const svg::Point& bus_label_offset,
		const int bus_label_font_size,
		const std::string& data,
		const svg::Color& fill,
		const svg::Color& background,
		const double stroke_width)
		:
		stop_coordinate_(stop_coordinate),
		bus_label_offset_(bus_label_offset),
		bus_label_font_size_(bus_label_font_size),
		data_(data),
		fill_(fill),
		background_(background, stroke_width) {}

	RouteNamesRender::Background::Background(const svg::Color& fill_and_stroke, const double stroke_width)
		: background_fill_(fill_and_stroke), stroke_(fill_and_stroke), stroke_width_(stroke_width) {}

	void RouteNamesRender::Draw(svg::ObjectContainer&) const
	{

	}

	//----------------RouteRender------------------------

	RouteRender::RouteRender(const std::vector<svg::Point>& stops_coordinates, const svg::Color& stroke_color, const double stroke_width)
		: stops_coordinates_(stops_coordinates), stroke_color_(stroke_color), stroke_width_(stroke_width) {}

	void RouteRender::Draw(svg::ObjectContainer& container) const
	{
		svg::Polyline render;
		for (const auto& stop_coordinate : stops_coordinates_)
		{
			render.AddPoint(stop_coordinate);
		}
		render.SetFillColor(fill_color_);
		render.SetStrokeColor(stroke_color_);
		render.SetStrokeWidth(stroke_width_);
		render.SetStrokeLineCap(stroke_linecap_);
		render.SetStrokeLineJoin(stroke_linejoin_);
		container.Add(render);
	}

	//---------------MapRenderer-------------------------

	void MapRenderer::SetRenderSettings(const RenderSettings& render_settings)
	{
		render_settings_ = render_settings;
	}

	void MapRenderer::AddRoutRender(const std::vector<svg::Point>& stops_coordinates, const svg::Color& stroke_color)
	{
		
		routs_renders_.emplace_back(stops_coordinates, stroke_color, render_settings_.width);
	}

	svg::Document MapRenderer::GetRender() const
	{
		svg::Document render;
		for (const auto& route : routs_renders_)
		{
			route.Draw(render);
		}
		return render;
	}

	const std::vector<svg::Color> MapRenderer::GetColorPallete() const
	{
		return render_settings_.color_palette;
	}
} // namespace renderer