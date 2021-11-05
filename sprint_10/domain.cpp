#include "domain.h"

namespace domain
{
	StopStat::StopStat(std::string_view stop_name, const std::set<std::string_view>& buses)
		: stop_name_(stop_name), buses_(buses) {}

	BusStat::BusStat(std::string_view bus_num, int stops_on_route, int unique_stops, int64_t route_length, const double curvature)
		: bus_num_(bus_num), stops_on_route_(stops_on_route), unique_stops_(unique_stops), route_length_(route_length), curvature_(curvature) {}

	Stop::Stop(std::string_view stop_name, const double lat, const double lng)
		: stop_name_(stop_name)
	{
		coodinates_.lat = lat;
		coodinates_.lng = lng;
	}

	Bus::Bus(std::string_view bus_num, const std::vector<const Stop*>& stops, const std::unordered_set<const Stop*>& unicue_stops, bool cicle_type)
		: bus_num_(bus_num), stops_(stops), unicue_stops_(unicue_stops), cicle_type_(cicle_type) {}

} // namespace domain

