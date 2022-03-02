#pragma once

#include "domain.h"

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
	public:

		void SetDistanceBetweenStops(const std::string_view from_stop, const std::string_view to_stop, const int distance);

		int GetDistanceBetweenStops(domain::StopPtr lhs, domain::StopPtr rhs) const;

		std::unordered_map<std::pair<domain::StopPtr, domain::StopPtr>, int, domain::PairStopsHasher> GetAllDistances() const;

		double ComputeRoadDistance(domain::StopPtr lhs, domain::StopPtr rhs) const;

		void AddBusDatabase(const std::string_view bus_num, const std::vector<std::string_view>& stops, const bool is_circular);

		void AddStopDatabase(const std::string_view stop_name, const double lat, const double lng);
		
		domain::BusPtr FindBus(const std::string_view bus_num) const;

		domain::StopPtr FindStop(const std::string_view stop_name) const;

		const domain::StopStat* GetStopStat(const std::string_view stop_name) const;

		const domain::BusStat* GetBusStat(const std::string_view bus_name) const;

		const std::vector<geo::Coordinates> GetAllStopsCoordinates() const;

		const std::vector<domain::StopPtr> GetAllStops() const;

		const std::vector<domain::StopPtr> GetStops(const std::string_view bus_num) const;

		const std::deque<domain::BusPtr> GetBuses() const;

	private:

		std::deque<domain::Bus> buses_;
		std::deque<domain::Stop> stops_;
		std::unordered_map<std::string_view, domain::BusPtr> buses_map_;
		std::unordered_map<std::string_view, domain::StopPtr> stops_map_;
		std::unordered_map<std::pair<domain::StopPtr, domain::StopPtr>, int, domain::PairStopsHasher> stop_to_stop_distance_;
	};

} // namespace transport_catalogue

