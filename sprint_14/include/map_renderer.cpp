#include "map_renderer.h"

namespace renderer
{
	//----------------SphereProjector----------------------------

	bool IsZero(const double& value)
	{
		return std::abs(value) < EPSILON;
	}

	svg::Point SphereProjector::operator()(const geo::Coordinates& coords) const
	{
		return { (coords.lng - min_lon_) * zoom_coeff_ + padding_, (max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
	}

	//----------------StopPointRender------------------------

	StopPointRender::StopPointRender(const svg::Point& stop_coordinate, const RenderSettings& render_settings)
		: stop_coordinate_(stop_coordinate), render_settings_(render_settings) {}

	void StopPointRender::Draw(svg::ObjectContainer& container) const
	{
		svg::Circle circle;
		circle.SetCenter(stop_coordinate_);
		circle.SetRadius(render_settings_.stop_radius);
		circle.SetFillColor("white");
		container.Add(circle);
	}

	//-----------------TextRender------------------------


	TextRender::TextRender(const svg::Point& coordinate, const std::string_view& data, const svg::Color& fill, const bool& this_stop, const RenderSettings& render_settings)
		: coordinate_(coordinate), data_(data), fill_(fill), this_stop_(this_stop), render_settings_(render_settings) {}

	svg::Text TextRender::CreateText() const
	{
		svg::Text text;
		if (this_stop_ == true)
		{
			text.SetOffset(render_settings_.stop_label_offset);
			text.SetFontSize(render_settings_.stop_label_font_size);
			text.SetFillColor("black");
		}
		else
		{
			text.SetOffset(render_settings_.bus_label_offset);
			text.SetFontSize(render_settings_.bus_label_font_size);
			text.SetFontWeight("bold");
			text.SetFillColor(fill_);
		}
		text.SetPosition(coordinate_);
		text.SetFontFamily("Verdana");
		text.SetData(data_);
		return text;
	}

	svg::Text TextRender::CreateBeckgraund() const
	{
		svg::Text text{ CreateText() };
		text.SetFillColor(render_settings_.underlayer_color).SetStrokeColor(render_settings_.underlayer_color);
		text.SetStrokeWidth(render_settings_.underlayer_width);
		text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
		text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
		return text;
	}

	void TextRender::Draw(svg::ObjectContainer& container) const
	{
		container.Add(CreateBeckgraund());
		container.Add(CreateText());
	}

	//----------------RouteRender------------------------

	RouteRender::RouteRender(const std::vector<svg::Point>& stops_coordinates, const svg::Color& stroke_color, const RenderSettings& render_settings)
		: stops_coordinates_(stops_coordinates), stroke_color_(stroke_color), render_settings_(render_settings) {}

	void RouteRender::Draw(svg::ObjectContainer& container) const
	{
		svg::Polyline render;
		for (const auto& stop_coordinate : stops_coordinates_)
		{
			render.AddPoint(stop_coordinate);
		}
		render.SetFillColor(svg::NoneColor);
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

	const RenderSettings& MapRenderer::GetRenderSettings() const
	{
		return render_settings_;
	}

	void MapRenderer::AddRoutRender(const std::vector<svg::Point>& stops_coordinates, const svg::Color& stroke_color)
	{
		routs_renders_.emplace_back(stops_coordinates, stroke_color, render_settings_);
	}

	void MapRenderer::AddStopPointRender(const svg::Point& stop_coordinate)
	{
		stops_points_renders_.emplace_back(stop_coordinate, render_settings_);
	}

	void MapRenderer::AddTextRender(const svg::Point& stop_coordinate, const std::string& data, const svg::Color& text_color, bool this_stop)
	{
		(this_stop) ? stops_names_renders_.emplace_back(stop_coordinate, data, text_color, this_stop, render_settings_)
			: routs_names_renders_.emplace_back(stop_coordinate, data, text_color, this_stop, render_settings_);
	}

	svg::Document MapRenderer::GetRender() const
	{
		svg::Document render;
		for (const auto& route_line : routs_renders_)
		{
			route_line.Draw(render);
		}
		for (const auto& route_name : routs_names_renders_)
		{
			route_name.Draw(render);
		}
		for (const auto& stop_point : stops_points_renders_)
		{
			stop_point.Draw(render);
		}
		for (const auto& stop_name : stops_names_renders_)
		{
			stop_name.Draw(render);
		}
		return render;
	}

	const std::vector<svg::Color>& MapRenderer::GetColorPallete() const
	{
		return render_settings_.color_palette;
	}

} // namespace renderer