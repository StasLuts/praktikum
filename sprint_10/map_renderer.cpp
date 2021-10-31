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

	//----------------RouteRender------------------------

	RouteRender::RouteRender(const std::vector<svg::Point>& stops_coordinates, const svg::Color& stroke_color, const double stroke_width)
		: stops_coordinates_(std::move(stops_coordinates)), stroke_color_(stroke_color), stroke_width_(stroke_width) {}

	void RouteRender::Draw(svg::ObjectContainer& container) const
	{
	}

	//---------------MapRenderer-------------------------

	void MapRenderer::SetRenderSettings(const RenderSettings& render_settings)
	{
		render_settings_ = std::move(render_settings);
	}

	void MapRenderer::AddRoutRender(const std::vector<geo::Coordinates>& stops_coordinates, const svg::Color& stroke_color)
	{
		routs_renders_.emplace_back(( stops_coordinates, stroke_color, render_settings_.line_width ));
	}

	svg::Document MapRenderer::GetRender() const
	{
		return svg::Document();
	}

	const std::vector<svg::Color> MapRenderer::GetColorPallete() const
	{
		return render_settings_.color_palette;
	}

} // namespace renderer