#pragma once
#include "json.h"
#include "transport_catalogue.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

namespace json_reader
{
	void JsonRead(std::istream&);

	//------------------input-------------------------

	void MakeBase(transport_catalogue::TransportCatalogue&, const json::Array&);

	void ReadStopData(transport_catalogue::TransportCatalogue&, const json::Dict&);

	void ReadStopDistance(transport_catalogue::TransportCatalogue&, const json::Dict&);

	void ReadBusData(transport_catalogue::TransportCatalogue&, const json::Dict&);

	//------------------outnput-------------------------

	void MakeResponse(transport_catalogue::TransportCatalogue&, const json::Array&);

	const json::Dict GetStopInfo(const transport_catalogue::TransportCatalogue&, const json::Dict&);

	const json::Dict GetBusInfo(transport_catalogue::TransportCatalogue&, const json::Dict&);
} // json_reader 















/*
#pragma once

#include "transport_catalogue.h"
#include <string>
#include <vector>
#include <tuple>
#include <string_view>

namespace transport_catalogue
{
	namespace input
	{
		std::string ReadLine();

		int ReadLineWithNumber();

		std::tuple<std::string_view, double, double> SplitForStop(std::string_view text);

		std::tuple<std::string_view, bool, std::vector<std::string_view>> SplitForBus(std::string_view text);

		std::vector<std::tuple<std::string_view, std::string_view, int>> SplitForDistance(std::string_view text);

		void FillData(TransportCatalogue& catalog);
	}
}
*/