#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

namespace map_renderer
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

	//-------------------------RouteRenderer----------------------------

	RouteRenderer::RouteRenderer(const svg::Color& stroke_color_, const double stroke_width_, const std::vector<svg::Point>& stops_points_)
	{

	}

	void RouteRenderer::Draw(svg::ObjectContainer& container) const
	{

	}

	//-------------------------MapRenderer----------------------------

	void MapRenderer::SetRenderSettings(const RenderSettings& render_settings)
	{
		render_settings_ = render_settings;
	}

	void MapRenderer::CreateRender(transport_catalogue::TransportCatalogue& trans_cat)
	{
		const auto stops_coordinates = trans_cat.GetCoordinates();
		map_renderer::SphereProjector sphere_projector(stops_coordinates.begin(), stops_coordinates.end(), render_settings_.width, render_settings_.height, render_settings_.padding);
	}

	void MapRenderer::SetRouts(const std::vector<RouteRenderer> routs)
	{
		routs_ = std::move(routs);
	}

	svg::Document MapRenderer::Render(const transport_catalogue::TransportCatalogue& trans_cat)
	{
		svg::Document render;
	}

	const std::vector<RouteRenderer> MapRenderer::RoursLineRender(const transport_catalogue::TransportCatalogue&, const map_renderer::SphereProjector&)
	{
		return std::vector<RouteRenderer>();
	}
} // namespace map_renderer