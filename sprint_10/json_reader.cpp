#include "json_reader.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
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
		const auto stat_requests = dict.find("stat_requests");
		if (stat_requests != dict.end())
		{
			MakeResponse(trans_cat, stat_requests->second.AsArray());
		}
	}

	//------------------input-------------------------

	void MakeBase(transport_catalogue::TransportCatalogue trans_cat, const json::Array& arr)
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

	void ReadStopData(transport_catalogue::TransportCatalogue trans_cat, const json::Dict& dict)
	{
		const auto name = dict.at("name").AsString();
		const auto latitude = dict.at("latitude").AsDouble();
		const auto longitude = dict.at("longitude").AsDouble();
		trans_cat.AddingStopDatabase(name, latitude, longitude);
	}

	void ReadStopDistance(transport_catalogue::TransportCatalogue trans_cat, const json::Dict& dict)
	{
		const auto from_stop_name = dict.at("name").AsString();
		const auto stops = dict.at("road_distances").AsMap();
		for (const auto& [to_stop_name, distance] : stops)
		{
			trans_cat.SetDistanceBetweenStops(from_stop_name, to_stop_name, distance.AsInt());
		}
	}

	void ReadBusData(transport_catalogue::TransportCatalogue trans_cat, const json::Dict& dict)
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

	//------------------outnput-------------------------

	void MakeResponse(transport_catalogue::TransportCatalogue trans_cat, const json::Array& arr)
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
			}
		}
		json::Print(json::Document(response), std::cout);
	}

	json::Dict GetStopInfo(transport_catalogue::TransportCatalogue trans_cat, const json::Dict& dict)
	{
		const auto stop_name = dict.at("name").AsString();
		const auto stop_datd = trans_cat.GetStopInfo(stop_name);
		json::Dict stop_info;
		stop_info.emplace("buses", stop_datd->buses_);
		stop_info.emplace("request_id", dict.at("id").IsInt());
	}

	json::Dict GetBusInfo(transport_catalogue::TransportCatalogue, const json::Dict&)
	{
		///////
	}

} // json_reader
