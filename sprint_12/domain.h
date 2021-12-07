#pragma once

#include "geo.h"

#include <string>
#include <set>
#include <unordered_set>
#include <vector>

namespace domain
{
	struct RoutingSettings
	{
		int bus_wait_time = 0;
		double bus_velocity = 0.0;
	};

	struct Stop
	{
		Stop(const std::string_view stop_name, const double lat, const double lng);
		geo::Coordinates coodinates;
		std::string stop_name;
	};

	using StopPtr = const Stop*;

	struct Bus
	{
		Bus(const std::string_view bus_num, const std::vector<StopPtr>& stops, const std::unordered_set<StopPtr>& unique_stops, const bool is_circular);
		std::string bus_num;
		std::vector<StopPtr> stops;
		std::unordered_set<const Stop*> unique_stops;
		bool is_circular;
	};

	struct StopStat
	{
		StopStat(const std::string_view stop_name, const std::set<std::string_view>& buses);
		std::string stop_name;
		std::set<std::string_view> buses;
	};

	struct BusStat
	{
		BusStat(const std::string_view bus_num, const int stops_on_route, const int unique_stops, const int64_t route_length, const double curvature);
		std::string bus_num;
		int stops_on_route;
		int unique_stops;
		int64_t route_length;
		double curvature;
	};

	using BusPtr = const Bus*;	

	struct PairStopsHasher
	{
		size_t operator() (const std::pair<StopPtr, StopPtr>& stops) const
		{
			return pair_hasher_(stops.first) + pair_hasher_(stops.second);
		}

	private:

		std::hash<const void*> pair_hasher_;
	};

} // namespace domain