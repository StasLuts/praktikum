#pragma once

#include "geo.h"

#include <string>
#include <set>
#include <unordered_set>
#include <vector>

namespace domain
{

	struct Stop
	{
		Stop(const std::string_view&, const double&, const double&);
		geo::Coordinates coodinates_;
		std::string stop_name_;
	};

	using StopPtr = const Stop*;

	struct Bus
	{
		Bus(const std::string_view&, const std::vector<StopPtr>&, const std::unordered_set<StopPtr>&, const bool&);
		std::string bus_num_;
		std::vector<StopPtr> stops_;
		std::unordered_set<const Stop*> unicue_stops_;
		bool cicle_type_;
	};

	struct StopStat
	{
		StopStat(const std::string_view&, const std::set<std::string_view>&);
		std::string stop_name_;
		std::set<std::string_view> buses_;
	};

	struct BusStat
	{
		BusStat(const std::string_view&, const int&, const int&, const int64_t&, const double&);
		std::string bus_num_;
		int stops_on_route_;
		int unique_stops_;
		int64_t route_length_;
		double curvature_;
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