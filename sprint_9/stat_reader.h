#pragma once

#include "transport_catalogue.h"
#include <string_view>

namespace transport_catalogue
{
	namespace output
	{
		std::string_view SplitForOutput(const std::string_view&);

		void PrintRouteInfo(const TransportCatalogue&, const std::string_view&, std::ostream&);

		void PrintStopInfo(const TransportCatalogue&, const std::string_view&, std::ostream&);

		void OutputData(const TransportCatalogue&, std::ostream&);

	} // namespace output
} // namespace transport_catalogue
