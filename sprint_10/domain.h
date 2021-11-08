#pragma once

#include "geo.h"

#include <string>
#include <set>
#include <unordered_set>
#include <vector>

namespace domain
{

	using BusPtr = const Bus*;
	using StopPtr = const Stop*;

	struct StopStat
	{
		StopStat(const std::string_view& stop_name, const std::set<std::string_view>& buses);
		std::string stop_name_;
		std::set<std::string_view> buses_;
	};

	struct BusStat
	{
		BusStat(const std::string_view& bus_num, int stops_on_route, int unique_stops, int64_t route_length, const double curvature);
		std::string bus_num_;
		int stops_on_route_;
		int unique_stops_;
		int64_t route_length_;
		double curvature_;
	};

	struct Stop
	{
		Stop(const std::string_view& stop_name, const double lat, const double lng);
		std::string stop_name_;
		geo::Coordinates coodinates_;
	};

	struct Bus
	{
		Bus(const std::string_view& bus_num, const std::vector<const Stop*>& stops, const std::unordered_set<StopPtr>& unicue_stops, bool cicle_type);
		std::string bus_num_;
		std::vector<const Stop*> stops_;
		std::unordered_set<const Stop*> unicue_stops_;
		bool cicle_type_;
	};

	struct PairStopsHasher
	{
		size_t operator() (std::pair<StopPtr, StopPtr> stops) const
		{
			return pair_hasher_(stops.first) + pair_hasher_(stops.second);
		}

	private:

		std::hash<const void*> pair_hasher_;
	};

} // namespace domain