#include "serialization.h"

namespace serialize
{
	// Serialize

	void Serializer::Serialize(const std::string& filename)
	{
		std::ofstream out(filename, std::ios::binary);
		SerializeStop(trans_cat_ser_);
		SerializeDistance(trans_cat_ser_);
		SerializeBus(trans_cat_ser_);
		SerealizeRenderSettings(trans_cat_ser_);
		SerealizeRoutingSettings(trans_cat_ser_);
		trans_cat_ser_.SerializeToOstream(&out);
	}

	// Serialize private

	void Serializer::SerializeStop(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser)
	{
		for (const auto& stop : trans_cat_.GetAllStops())
		{
			transport_catalogue_serialize::Stop buf_stop;
			transport_catalogue_serialize::Coordinates buf_coordinates;
			buf_coordinates.set_lat(stop->coodinates.lat);
			buf_coordinates.set_lng(stop->coodinates.lng);
			buf_stop.set_name(stop->stop_name);
			*buf_stop.mutable_coordinates() = buf_coordinates;
			*trans_cat_ser.add_stops() = buf_stop;
		}
	}

	void Serializer::SerializeBus(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser)
	{
		for (const auto& bus : trans_cat_.GetBuses())
		{
			transport_catalogue_serialize::Bus buf_bus;
			buf_bus.set_bus_num(bus->bus_num);
			buf_bus.set_is_circular(bus->is_circular);
			for (auto stop : bus->stops)
			{
				transport_catalogue_serialize::Stop buf_stop;
				transport_catalogue_serialize::Coordinates buf_coordinates;
				buf_coordinates.set_lat(stop->coodinates.lat);
				buf_coordinates.set_lng(stop->coodinates.lng);
				buf_stop.set_name(stop->stop_name);
				*buf_stop.mutable_coordinates() = buf_coordinates;
				*buf_bus.add_stops() = buf_stop;
			}
			*trans_cat_ser.add_buses() = buf_bus;
		}
	}

	void Serializer::SerializeDistance(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser)
	{
		for (const auto& distance : trans_cat_.GetAllDistances())
		{
			transport_catalogue_serialize::Distance buf_distance;
			buf_distance.set_from(distance.first.first->stop_name);
			buf_distance.set_to(distance.first.second->stop_name);
			buf_distance.set_distance(distance.second);
			*trans_cat_ser.add_distances() = buf_distance;
		}
	}

	void Serializer::SerealizeRenderSettings(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser)
	{
		auto render_settings = map_renderer_.GetRenderSettings();
		transport_catalogue_serialize::RenderSettings buf_render_settings;
		transport_catalogue_serialize::Point buf_point;
		buf_render_settings.set_width(render_settings.width);
		buf_render_settings.set_height(render_settings.height);
		buf_render_settings.set_padding(render_settings.padding);
		buf_render_settings.set_line_width(render_settings.line_width);
		buf_render_settings.set_stop_radius(render_settings.stop_radius);
		buf_render_settings.set_bus_label_font_size(render_settings.bus_label_font_size);
		buf_render_settings.set_stop_label_font_size(render_settings.stop_label_font_size);
		buf_render_settings.set_underlayer_width(render_settings.underlayer_width);

		buf_point.set_x(render_settings.bus_label_offset.x);
		buf_point.set_y(render_settings.bus_label_offset.y);
		*buf_render_settings.mutable_bus_label_offset() = buf_point;

		buf_point.set_x(render_settings.stop_label_offset.x);
		buf_point.set_y(render_settings.stop_label_offset.y);
		*buf_render_settings.mutable_stop_label_offset() = buf_point;

		*buf_render_settings.mutable_underlayer_color() = SerealizeColor(render_settings.underlayer_color);

		for (const auto& color : render_settings.color_palette)
		{
			*buf_render_settings.add_color_palette() = SerealizeColor(color);
		}

		*trans_cat_ser.mutable_render_settings() = buf_render_settings;
	}

	transport_catalogue_serialize::Color Serializer::SerealizeColor(const svg::Color& color)
	{
		transport_catalogue_serialize::Color buf_color;
		transport_catalogue_serialize::Rgba buf_rgba;
		 
		if (std::holds_alternative<svg::Rgb>(color))
		{
		svg::Rgb rgb = std::get<svg::Rgb>(color);
		buf_rgba.set_blue(rgb.blue);
		buf_rgba.set_green(rgb.green);
		buf_rgba.set_red(rgb.red);
		*buf_color.mutable_rgba() = buf_rgba;
		}
		else if (std::holds_alternative<svg::Rgba>(color))
		{
			svg::Rgba rgba = std::get<svg::Rgba>(color);
			buf_rgba.set_blue(rgba.blue);
			buf_rgba.set_green(rgba.green);
			buf_rgba.set_red(rgba.red);
			buf_rgba.set_opacity(rgba.opacity);
			buf_color.set_is_rgba(true);
			*buf_color.mutable_rgba() = buf_rgba;
		}
		else if(std::holds_alternative<std::string>(color))
		{
			buf_color.set_name(std::get<std::string>(color));
		}
		return buf_color;
	}

	void Serializer::SerealizeRoutingSettings(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser)
	{
		transport_catalogue_serialize::RoutingSettings buf_routing_settings;
		auto routing_settings = trans_rote_.GetRoutingSettings();
		buf_routing_settings.set_bus_velocity(routing_settings.bus_velocity);
		buf_routing_settings.set_bus_wait_time(routing_settings.bus_wait_time);
		*trans_cat_ser.mutable_routing_settings() = buf_routing_settings;
	}

	//Deserializer

	void Deserializer::DeserializeCatalogAndRenderer(transport_catalogue::TransportCatalogue& trans_cat, renderer::MapRenderer& map_renderer, const std::string& filename)
	{
		std::ifstream in(filename, std::ios::binary);
		transport_catalogue_serialize::TransportCatalogue trans_cat_ser;
		trans_cat_ser.ParseFromIstream(&in);
		// trans_cat
		for (int i = 0; i < trans_cat_ser.stops().size(); ++i)
		{
			auto stop_ser = trans_cat_ser.stops(i);
			trans_cat.AddStopDatabase(stop_ser.name(), stop_ser.coordinates().lat(), stop_ser.coordinates().lng());
		}
		for (int i = 0; i < trans_cat_ser.buses().size(); ++i)
		{
			auto bus_ser = trans_cat_ser.buses(i);
			std::vector<std::string_view> stops;
			for (const auto& stop_ser : bus_ser.stops())
			{
				stops.push_back(stop_ser.name());
			}
			trans_cat.AddBusDatabase(bus_ser.bus_num(), stops, bus_ser.is_circular());
		}
		for (int i = 0; i < trans_cat_ser.distances_size(); ++i)
		{
			const auto deserialized_distance = DeserializeDistance(trans_cat_ser.distances(i), trans_cat);
			trans_cat.SetDistanceBetweenStops(deserialized_distance.first.first->stop_name, deserialized_distance.first.second->stop_name, deserialized_distance.second);
		}
		// map_renderer
		auto settings  = DeserealizeRenderSettings(trans_cat_ser.render_settings());
		map_renderer.SetRenderSettings(settings);
		const auto all_stops_coordinates = trans_cat.GetAllStopsCoordinates();
		renderer::SphereProjector projector(all_stops_coordinates.begin(), all_stops_coordinates.end(), settings.width, settings.height, settings.padding);
		const std::vector<svg::Color> color_pallete = map_renderer.GetColorPallete();
		size_t color_num = 0;
		std::map<std::string, svg::Point> stops;
		for (const auto& it : trans_cat.GetBuses())
		{
			std::vector<svg::Point> stops_points;
			for (const auto& stop : trans_cat.GetStops(it->bus_num))
			{
				stops_points.emplace_back(projector(stop->coodinates));
				stops[stop->stop_name] = stops_points.back();
			}
			if (it->is_circular == true)
			{
				map_renderer.AddTextRender(*stops_points.begin(), it->bus_num, color_pallete[color_num], false);
				map_renderer.AddRoutRender(stops_points, color_pallete[color_num]);
			}
			else if (it->is_circular == false)
			{
				map_renderer.AddTextRender(*stops_points.begin(), it->bus_num, color_pallete[color_num], false);
				if (*it->stops.begin() != it->stops.back())map_renderer.AddTextRender(stops_points.back(), it->bus_num, color_pallete[color_num], false);
				stops_points.insert(stops_points.end(), stops_points.rbegin() + 1, stops_points.rend());
				map_renderer.AddRoutRender(stops_points, color_pallete[color_num]);
			}
			(color_num == color_pallete.size() - 1) ? color_num = 0 : ++color_num;
		}
		for (const auto& [name, coordinate] : stops)
		{
			map_renderer.AddStopPointRender(coordinate);
			map_renderer.AddTextRender(coordinate, name, color_pallete[color_num], true);
		}
	}

	void Deserializer::DeserealizeRouter(transport_router::TransportRouter& trans_rote, const std::string& filename)
	{
		std::ifstream in(filename, std::ios::binary);
		transport_catalogue_serialize::TransportCatalogue trans_cat_ser;
		trans_cat_ser.ParseFromIstream(&in);
		auto routing_settings_ser = trans_cat_ser.routing_settings();
		trans_rote.SetRoutingSettings(routing_settings_ser.bus_wait_time(), routing_settings_ser.bus_velocity());
	}

	//Deserializer private

	std::pair<std::pair<domain::StopPtr, domain::StopPtr>, int> Deserializer::DeserializeDistance(const transport_catalogue_serialize::Distance& distance_ser, const transport_catalogue::TransportCatalogue& trans_cat)
	{
		domain::StopPtr stop_from = trans_cat.FindStop(distance_ser.from());
		domain::StopPtr stop_to = trans_cat.FindStop(distance_ser.to());
		int64_t distance = distance_ser.distance();
		return std::make_pair(std::make_pair(stop_from, stop_to), distance);
	}

	renderer::RenderSettings Deserializer::DeserealizeRenderSettings(const transport_catalogue_serialize::RenderSettings& render_settings_ser)
	{
		renderer::RenderSettings render_settings;

		render_settings.width = render_settings_ser.width();
		render_settings.height = render_settings_ser.height();
		render_settings.padding = render_settings_ser.padding();
		render_settings.line_width = render_settings_ser.line_width();
		render_settings.stop_radius = render_settings_ser.stop_radius();
		render_settings.bus_label_font_size = render_settings_ser.bus_label_font_size();
		render_settings.stop_label_font_size = render_settings_ser.stop_label_font_size();
		render_settings.underlayer_width = render_settings_ser.underlayer_width();

		render_settings.bus_label_offset = { render_settings_ser.bus_label_offset().x(), render_settings_ser.bus_label_offset().y() };
		render_settings.stop_label_offset = { render_settings_ser.stop_label_offset().x(), render_settings_ser.stop_label_offset().y() };

		render_settings.underlayer_color = DserealizeColor(render_settings_ser.underlayer_color());

		for (const auto& color_ser : render_settings_ser.color_palette())
		{
			render_settings.color_palette.push_back(DserealizeColor(color_ser));
		}

		return render_settings;
	}

	svg::Color Deserializer::DserealizeColor(const transport_catalogue_serialize::Color& color_ser)
	{
		if (!color_ser.name().empty())
		{
			return color_ser.name();
		}
		else if (color_ser.is_rgba())
		{
			return svg::Rgba(color_ser.rgba().red(), color_ser.rgba().green(), color_ser.rgba().blue(), color_ser.rgba().opacity());
		}
		return svg::Rgb(color_ser.rgba().red(), color_ser.rgba().green(), color_ser.rgba().blue());
	}
} // namespace serialize