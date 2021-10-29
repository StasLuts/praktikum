#include "json_reader.h"

/*
 * «десь можно разместить код наполнени€ транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

namespace json_reader
{
	void JsonRead(std::istream& input)
	{
		transport_catalogue::TransportCatalogue trans_cat;
 		const auto dict = json::Load(input).GetRoot().AsMap();
		const auto base_requests = dict.find("base_requests");
		if (base_requests != dict.end())
		{
			MakeBase(trans_cat, base_requests->second.AsArray());
		}
		const auto render_settings = dict.find("render_settings");// если render_settings, передаем каталог и словарь в метод инициализируюший рисовалку
		//renderer::MapRenderer map_renderer;
		if (render_settings != dict.end())
		{
			SetMapRenderer(trans_cat, map_renderer, render_settings->second.AsMap());
		}
		const auto stat_requests = dict.find("stat_requests");
		if (stat_requests != dict.end())
		{
			MakeResponse(trans_cat, map_renderer, stat_requests->second.AsArray());
		}
	}

	//------------------input-------------------------

	void MakeBase(transport_catalogue::TransportCatalogue& trans_cat, const json::Array& arr)
	{
		for (const auto& recuest : arr)
		{
			const auto rec_type = recuest.AsMap().find("type");
			if (rec_type != recuest.AsMap().end())
			{
				if (rec_type->second.AsString() == "Stop")
				{
					ReadStopData(trans_cat, recuest.AsMap());
				}
			}
		}

		for (const auto& recuest : arr)
		{
			const auto rec_type = recuest.AsMap().find("type");
			if (rec_type != recuest.AsMap().end())
			{
				if (rec_type->second.AsString() == "Stop")
				{
					ReadStopDistance(trans_cat, recuest.AsMap());
				}
			}
		}

		for (const auto& recuest : arr)
		{
			const auto rec_type = recuest.AsMap().find("type");
			if (rec_type != recuest.AsMap().end())
			{
				if (rec_type->second.AsString() == "Bus")
				{
					ReadBusData(trans_cat, recuest.AsMap());
				}
			}
		}
	}

	void ReadStopData(transport_catalogue::TransportCatalogue& trans_cat, const json::Dict& dict)
	{
		const auto name = dict.at("name").AsString();
		const auto latitude = dict.at("latitude").AsDouble();
		const auto longitude = dict.at("longitude").AsDouble();
		trans_cat.AddingStopDatabase(name, latitude, longitude);
	}

	void ReadStopDistance(transport_catalogue::TransportCatalogue& trans_cat, const json::Dict& dict)
	{
		const auto from_stop_name = dict.at("name").AsString();
		const auto stops = dict.at("road_distances").AsMap();
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
		const auto cicle_type = dict.at("is_roundtrip").AsBool();
		trans_cat.AddingBusDatabase(bus_name, stops, cicle_type);
	}

	//------------------render-------------------------

	const svg::Color GetColor(const json::Node& color)//вытаскиваем тип цвета из документа
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

	//читает и наполн€ет настройки визуализации
	renderer::RenderSettings ReadRenderSettings(const json::Dict& dict)//заполн€ем настройки визуализации
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
	}

	void SetMapRenderer(const transport_catalogue::TransportCatalogue& trans_cat, renderer::MapRenderer& map_renderer, const json::Dict& dict)//инициализируем рисовалку
	{
		map_renderer.SetRenderSettings(ReadRenderSettings(dict));
		map_renderer.CreateRender(trans_cat);//сдесь метод заполн€юший на основе каталога инфу по рисованию обьектов
	}

	//------------------outnput-------------------------

	void MakeResponse(transport_catalogue::TransportCatalogue& trans_cat, const renderer::MapRenderer& map_renderer, const json::Array& arr)
	{
		json::Array response;
		for (const auto& recuest : arr)
		{
			const auto rec_type = recuest.AsMap().find("type");
			if (rec_type != recuest.AsMap().end())
			{
				if (rec_type->second.AsString() == "Stop")
				{
					response.emplace_back(GetStopInfo(trans_cat, recuest.AsMap()));
				}
				else if (rec_type->second.AsString() == "Bus")
				{
					response.emplace_back(GetBusInfo(trans_cat, recuest.AsMap()));
				}
				else if (rec_type->second.AsString() == "Map")
				{
					//response.emplace_back(map_renderer.Render());
				}
			}
		}
		json::Print(json::Document(response), std::cout);
	}

	const json::Dict GetStopInfo(const transport_catalogue::TransportCatalogue& trans_cat, const json::Dict& dict)
	{
		const auto stop_name = dict.at("name").AsString();
		const auto stop_datd = trans_cat.GetStopInfo(stop_name);
		json::Dict stop_info;
		if(stop_datd == nullptr)
		{
			stop_info.emplace("request_id", dict.at("id").AsInt());
			stop_info.emplace("error_message", "not found");
		}
		else
		{
			json::Array buses;
			for (const auto& bus : stop_datd->buses_)
			{
				buses.push_back(static_cast<std::string>(bus));
			}
			stop_info.emplace("buses", buses);
			stop_info.emplace("request_id", dict.at("id").AsInt());
		}
		return stop_info;
	}

	const json::Dict GetBusInfo(transport_catalogue::TransportCatalogue& trans_cat, const json::Dict& dict)
	{
		const auto bus_name = dict.at("name").AsString();
		const auto bus_data = trans_cat.GetRoute(bus_name);
		json::Dict bus_info;
		if (bus_data == nullptr)
		{
			bus_info.emplace("request_id", dict.at("id").AsInt());
			bus_info.emplace("error_message", "not found");
		}
		else
		{
			bus_info.emplace("curvature", bus_data->curvature_);
			bus_info.emplace("request_id", dict.at("id").AsInt());
			bus_info.emplace("route_length", static_cast<int>(bus_data->route_length_));
			bus_info.emplace("stop_count", bus_data->stops_on_route_);
			bus_info.emplace("unique_stop_count", bus_data->unique_stops_);
		}
		return bus_info;
	}

} // namespace json_reader
