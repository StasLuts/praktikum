#pragma once

#include "geo.h"

#include <string_view>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <vector>
#include <set>
#include <functional>
#include <map>

namespace transport_catalogue
{
	class TransportCatalogue
	{
	private:

		struct Stop
		{
			Stop(const std::string_view&, const double&, const double&);
			transport_catalogue::geo::Coordinates coodinates_;
			std::string stop_name_;
		};

		using StopPtr = const Stop*;

		struct Bus
		{
			Bus(const std::string_view&, const std::vector<StopPtr>&, const std::unordered_set<StopPtr>&, const bool&);
			std::string bus_num_;
			std::vector<StopPtr> stops_;
			std::unordered_set<const Stop*> unique_stops_;
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

		std::deque<Bus> buses_;
		std::deque<Stop> stops_;
		std::unordered_map<std::string_view, BusPtr> buses_map_;
		std::unordered_map<std::string_view, StopPtr> stops_map_;
		std::unordered_map<std::pair<StopPtr, StopPtr>, int, PairStopsHasher> stop_to_stop_distance_;

	public:

		void SetDistanceBetweenStops(const std::string_view&, const std::string_view&, const int&);

		int GetDistanceBetweenStops(StopPtr, StopPtr) const;

		void AddBusDatabase(const std::string_view&, const std::vector<std::string_view>&, const bool&);

		void AddStopDatabase(const std::string_view&, const double&, const double&);

		BusPtr FindBus(const std::string_view&) const;

		StopPtr FindStop(const std::string_view&) const;

		const StopStat* GetStopStat(const std::string_view&) const;

		const BusStat* GetBusStat(const std::string_view&) const;
	};

} // namespace transport_catalogue

