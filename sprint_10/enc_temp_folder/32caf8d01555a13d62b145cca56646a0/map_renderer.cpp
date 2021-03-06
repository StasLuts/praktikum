#include "map_renderer.h"

/*
 * ? ???? ????? ?? ?????? ?????????? ???, ?????????? ?? ???????????? ????? ????????? ? ??????? SVG.
 * ???????????? ?????????? ??? ??????????? ?? ?????? ????? ????????? ???????.
 * ???? ?????? ???????? ???? ??????.
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