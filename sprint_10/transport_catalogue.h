#pragma once
#include "geo.h"
#include "domain.h"

#include <string_view>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <vector>
#include <set>
#include <functional>

namespace transport_catalogue
{
	class TransportCatalogue
	{
	private:

		std::deque<domain::Bus> buses_;
		std::deque<domain::Stop> stops_;
		std::unordered_map<std::string_view, const domain::Bus*> buses_map_;
		std::unordered_map<std::string_view, const domain::Stop*> stops_map_;
		std::unordered_map<std::pair<const domain::Stop*, const domain::Stop*>, int, domain::PairStopsHasher> stop_to_stop_distance_;

	public:

		void SetDistanceBetweenStops(std::string_view from_stop, std::string_view to_stop, int distance);

		int GetDistanceBetweenStops(const domain::Stop* lhs, const domain::Stop* rhs) const;

		void AddingBusDatabase(std::string_view bus_num, std::vector<std::string_view>& stops, bool cicle_type);

		void AddingStopDatabase(std::string_view stop_name, const double lat, const double lng);

		const domain::Bus* FindBus(std::string_view bus_num) const;

		const domain::Stop* FindStop(std::string_view stop_name) const;

		const domain::StopStat* GetStopStat(std::string_view stop_name) const;

		const domain::BusStat* GetBusStat(const std::string_view& bus_name) const;

		const std::vector<geo::Coordinates> GetAllStopsCoordinates() const;

		const std::vector<geo::Coordinates> GetStopsCoordinates(const std::string_view) const;

		const std::deque<domain::BusPtr> GetBuses() const;
	};

} // namespace transport_catalogue

