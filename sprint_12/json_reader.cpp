#include "json_reader.h"

#include <sstream>
#include <fstream>
#include <map>

namespace json_reader
{
	void JsonRead(std::istream& input)
	{
		transport_catalogue::TransportCatalogue trans_cat;
		const json::Dict dict = json::Load(input).GetRoot().AsDict();
		const auto base_requests = dict.find("base_requests");
		if (base_requests != dict.end())
		{
			MakeBase(trans_cat, base_requests->second.AsArray());
		}
		const auto render_settings = dict.find("render_settings");
		renderer::MapRenderer map_renderer;
		if (render_settings != dict.end())
		{
			SetMapRenderer(trans_cat, map_renderer, render_settings->second.AsDict());
		}
		const auto routing_settings = dict.find("routing_settings");
		transport_router::TransportRouter trans_roter(trans_cat);
		if (routing_settings != dict.end())
		{
			SetRoutingSettings(trans_roter, routing_settings->second.AsDict());
		}
		const auto stat_requests = dict.find("stat_requests");
		if (stat_requests != dict.end())
		{
			MakeResponse(request_handler::RequestHandler(trans_cat, map_renderer), trans_roter, stat_requests->second.AsArray());
		}
	}

	//------------------input-------------------------

	void MakeBase(transport_catalogue::TransportCatalogue& trans_cat, const json::Array& arr)
	{
		for (const auto& request : arr)
		{
			const auto request_type= request.AsDict().find("type");
			if (request_type!= request.AsDict().end())
			{
				if (request_type->second.AsString() == "Stop")
				{
					ReadStopData(trans_cat, request.AsDict());
				}
			}
		}

		for (const auto& request : arr)
		{
			const auto request_type= request.AsDict().find("type");
			if (request_type!= request.AsDict().end())
			{
				if (request_type->second.AsString() == "Stop")
				{
					ReadStopDistance(trans_cat, request.AsDict());
				}
			}
		}

		for (const auto& request : arr)
		{
			const auto request_type= request.AsDict().find("type");
			if (request_type!= request.AsDict().end())
			{
				if (request_type->second.AsString() == "Bus")
				{
					ReadBusData(trans_cat, request.AsDict());
				}
			}
		}
	}

	void ReadStopData(transport_catalogue::TransportCatalogue& trans_cat, const json::Dict& dict)
	{
		const auto name = dict.at("name").AsString();
		const auto latitude = dict.at("latitude").AsDouble();
		const auto longitude = dict.at("longitude").AsDouble();
		trans_cat.AddStopDatabase(name, latitude, longitude);
	}

	void ReadStopDistance(transport_catalogue::TransportCatalogue& trans_cat, const json::Dict& dict)
	{
		const auto from_stop_name = dict.at("name").AsString();
		const auto stops = dict.at("road_distances").AsDict();
		for (const auto& [to_stop_name, distance] : stops)
		{
			trans_cat.SetDistanceBetweenStops(from_stop_name, to_stop_name, distance.AsInt());
		}
	}

	void ReadBusData(transport_catalogue::TransportCatalogue& trans_cat, const json::Dict& dict)
	{
		const auto bus_name = dict.at("name").AsString();
		std::vector<std::string_view> stops;
		for (const auto& stop : dict.at("stops").AsArray())
		{
			stops.emplace_back(stop.AsString());
		}
		dict.at("stops").AsArray();
		const auto is_circular = dict.at("is_roundtrip").AsBool();
		trans_cat.AddBusDatabase(bus_name, stops, is_circular);
	}

	void SetRoutingSettings(transport_router::TransportRouter& trans_roter, const json::Dict& dict)
	{
		trans_roter.SetRoutingSettings(dict.at("bus_wait_time").AsInt(), dict.at("bus_velocity").AsDouble());
	}

	//------------------render-------------------------

	const svg::Color GetColor(const json::Node& color)
	{
		if (color.IsString())
		{
			return svg::Color{ color.AsString() };
		}
		else if (color.IsArray())
		{
			if (color.AsArray().size() == 3)
			{
				return svg::Rgb
				{
					static_cast<uint8_t>(color.AsArray()[0].AsInt()),
					static_cast<uint8_t>(color.AsArray()[1].AsInt()),
					static_cast<uint8_t>(color.AsArray()[2].AsInt())
				};
			}
			else if (color.AsArray().size() == 4)
			{
				return svg::Rgba
				{
					static_cast<uint8_t>(color.AsArray()[0].AsInt()),
					static_cast<uint8_t>(color.AsArray()[1].AsInt()),
					static_cast<uint8_t>(color.AsArray()[2].AsInt()),
					color.AsArray()[3].AsDouble()
				};
			}
		}
		return svg::Color();
	}

	void SetMapRenderer(const transport_catalogue::TransportCatalogue& trans_cat, renderer::MapRenderer& map_renderer, const json::Dict& dict)//инициализируем рисовалку
	{
		renderer::RenderSettings settings;
		settings.width = dict.at("width").AsDouble();
		settings.height = dict.at("height").AsDouble();
		settings.padding = dict.at("padding").AsDouble();
		settings.line_width = dict.at("line_width").AsDouble();
		settings.stop_radius = dict.at("stop_radius").AsDouble();
		settings.bus_label_font_size = dict.at("bus_label_font_size").AsInt();
		settings.bus_label_offset = { dict.at("bus_label_offset").AsArray()[0].AsDouble(), dict.at("bus_label_offset").AsArray()[1].AsDouble() };//массив из двух double
		settings.stop_label_font_size = dict.at("stop_label_font_size").AsInt();
		settings.stop_label_offset = { dict.at("stop_label_offset").AsArray()[0].AsDouble(), dict.at("stop_label_offset").AsArray()[1].AsDouble() };//массив из двух double
		settings.underlayer_color = GetColor(dict.at("underlayer_color"));
		settings.underlayer_width = dict.at("underlayer_width").AsDouble();
		for (const auto& color : dict.at("color_palette").AsArray())
		{
			settings.color_palette.emplace_back(GetColor(color));
		}
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

	//------------------outnput-------------------------

	void MakeResponse(const request_handler::RequestHandler& request_handler, transport_router::TransportRouter& trans_roter, const json::Array& arr)
	{
		json::Array response;
		for (const auto& request : arr)
		{
			const auto request_type= request.AsDict().find("type");
			if (request_type!= request.AsDict().end())
			{
				if (request_type->second.AsString() == "Stop")
				{
					response.emplace_back(GetStopInfo(request_handler, request.AsDict()));
				}
				else if (request_type->second.AsString() == "Bus")
				{
					response.emplace_back(GetBusInfo(request_handler, request.AsDict()));
				}
				else if (request_type->second.AsString() == "Map")
				{
					response.emplace_back(GetMapRender(request_handler, request.AsDict()));
				}
				else if (request_type->second.AsString() == "Route")
				{
					response.emplace_back(GetRouteInfo(trans_roter, request.AsDict()));
				}
			}
		}
		json::Print(json::Document(json::Builder{}.Value(response).Build()), std::cout);
	}

	const json::Node GetStopInfo(const request_handler::RequestHandler& request_handler, const json::Dict& dict)
	{
		const std::string stop_name = dict.at("name").AsString();
		const auto stop_datd = request_handler.GetStopStat(stop_name);
		if (stop_datd == nullptr)
		{
			return json::Builder{}.StartDict()
				.Key("request_id").Value(dict.at("id").AsInt())
				.Key("error_message").Value("not found")
				.EndDict().Build().AsDict();
		}
		json::Array buses;
		for (auto& bus : stop_datd.value()->buses)
		{
			buses.push_back(static_cast<std::string>(bus));
		}
		return json::Builder{}.StartDict()
			.Key("buses").Value(buses)
			.Key("request_id").Value(dict.at("id").AsInt())
			.EndDict().Build();
	}

	const json::Node GetBusInfo(const request_handler::RequestHandler& request_handler, const json::Dict& dict)
	{
		const std::string bus_name = dict.at("name").AsString();
		const auto bus_data = request_handler.GetBusStat(bus_name);
		return (bus_data == nullptr) ? json::Builder{}.StartDict()
			.Key("request_id").Value(dict.at("id").AsInt())
			.Key("error_message").Value("not found")
			.EndDict().Build().AsDict()
			:
			json::Builder{}.StartDict()
			.Key("curvature").Value(bus_data.value()->curvature)
			.Key("request_id").Value(dict.at("id").AsInt())
			.Key("route_length").Value(static_cast<int>(bus_data.value()->route_length))
			.Key("stop_count").Value(bus_data.value()->stops_on_route)
			.Key("unique_stop_count").Value(bus_data.value()->unique_stops)
			.EndDict().Build();
	}

	const json::Node GetMapRender(const request_handler::RequestHandler& request_handler, const json::Dict& dict)
	{
		svg::Document render = request_handler.RenderMap();
		std::ostringstream strm;
		render.Render(strm);
		return json::Builder{}.StartDict()
			.Key("map").Value(strm.str())
			.Key("request_id").Value(dict.at("id").AsInt())
			.EndDict().Build();
	}

	const json::Node GetRouteInfo(transport_router::TransportRouter& trans_roter, const json::Dict& dict)
	{
		auto route_data = trans_roter.GetRoute(dict.at("from").AsString(), dict.at("to").AsString());
		if (!route_data.find)
		{
			return json::Builder{}.StartDict()
				.Key("request_id").Value(dict.at("id").AsInt())
				.Key("error_message").Value("not found")
				.EndDict().Build().AsDict();
		}
		json::Array items;
		for (const auto& item : route_data.items)
		{
			json::Dict items_map;
			if (item.type == graph::EdgeType::BUS)
			{
				items_map["type"] = "Bus";
				items_map["bus"] = item.bus_or_stop_name;
				items_map["span_count"] = item.span_count;
			}
			else if(item.type == graph::EdgeType::WAIT)
			{
				items_map["type"] = "Wait";
				items_map["stop_name"] = item.bus_or_stop_name;
			}
			items_map["time"] = item.time;
			items.push_back(items_map);
		}
		return json::Builder{}.StartDict()
			.Key("request_id").Value(dict.at("id").AsInt())
			.Key("total_time").Value(route_data.total_time)
			.Key("items").Value(items)
			.EndDict().Build().AsDict();
	}
} // namespace json_reader
