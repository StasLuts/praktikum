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

	//-----------------TextRender------------------------

	
	TextRender::TextRender(const svg::Point& coordinate, const std::string& data, const svg::Color& fill, bool this_stop, const RenderSettings& render_settings)
		: coordinate_(coordinate), data_(data), fill_(fill), this_stop_(this_stop), render_settings_(render_settings) {}

	void TextRender::Draw(svg::ObjectContainer& container) const
	{
		svg::Text text;
		if (this_stop_)
		{

		}
		else
		{

		}
		container.Add(text);
	}

	//----------------RouteRender------------------------

	RouteRender::RouteRender(const std::vector<svg::Point>& stops_coordinates, const svg::Color& stroke_color, const double, const RenderSettings& render_settings)
		: stops_coordinates_(stops_coordinates), stroke_color_(stroke_color), render_settings_(render_settings) {}

	void RouteRender::Draw(svg::ObjectContainer& container) const
	{
		svg::Polyline render;
		for (const auto& stop_coordinate : stops_coordinates_)
		{
			render.AddPoint(stop_coordinate);
		}
		render.SetFillColor("none");
		render.SetStrokeColor(stroke_color_);
		render.SetStrokeWidth(render_settings_.line_width);
		render.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
		render.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
		container.Add(render);
	}

	//---------------MapRenderer-------------------------

	void MapRenderer::SetRenderSettings(const RenderSettings& render_settings)
	{
		render_settings_ = render_settings;
	}

	void MapRenderer::AddRoutRender(const std::vector<svg::Point>& stops_coordinates, const svg::Color& stroke_color)
	{
		routs_renders_.emplace_back(stops_coordinates, stroke_color, render_settings_);
	}

	void MapRenderer::AddTextRender(const svg::Point& stop_coordinate, const std::string& data, const svg::Color& text_color, bool this_stop)
	{
		routs_names_renders_.emplace_back(stop_coordinate, data, text_color, this_stop, render_settings_);
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