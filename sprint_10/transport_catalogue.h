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
	private:

		std::deque<domain::Bus> buses_;
		std::deque<domain::Stop> stops_;
		std::unordered_map<std::string_view, domain::BusPtr> buses_map_;
		std::unordered_map<std::string_view, domain::StopPtr> stops_map_;
		std::unordered_map<std::pair<domain::StopPtr, domain::StopPtr>, int, domain::PairStopsHasher> stop_to_stop_distance_;

	public:

		void SetDistanceBetweenStops(const std::string_view&, const std::string_view&, const int&);

		int GetDistanceBetweenStops(domain::StopPtr, domain::StopPtr) const;

		void AddBusDatabase(const std::string_view&, const std::vector<std::string_view>&, const bool&);

		void AddStopDatabase(const std::string_view&, const double&, const double&);
		
		domain::BusPtr FindBus(const std::string_view&) const;

		domain::StopPtr FindStop(const std::string_view&) const;

		const domain::StopStat* GetStopStat(const std::string_view&) const;

		const domain::BusStat* GetBusStat(const std::string_view&) const;

		const std::vector<geo::Coordinates> GetAllStopsCoordinates() const;

		const std::vector<domain::StopPtr> GetStops(const std::string_view&) const;

		const std::deque<domain::BusPtr> GetBuses() const;
	};

} // namespace transport_catalogue

