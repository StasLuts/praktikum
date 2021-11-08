#pragma once

#include "transport_catalogue.h"
#include <string>
#include <vector>
#include <iostream>
#include <string_view>

namespace transport_catalogue
{
	namespace input
	{
		struct Stop
		{
			std::string_view name;
			double lat;
			double lng;
		};

		struct StopsDictance
		{
			std::string_view stop_to;
			std::string_view stop_from;
			int dictance;
		};

		struct Bus
		{
			std::string_view name;
			bool cicle_type;
			std::vector<std::string_view> stops;
		};

		std::string ReadLine(std::istream&);

		int ReadLineWithNumber();

		Stop SplitForStop(std::string_view);

		Bus SplitForBus(std::string_view);

		std::vector<StopsDictance> SplitForDistance(std::string_view);

		void FillData(TransportCatalogue&, std::istream&);

	} // 	namespace input
} // namespace transport_catalogue


