#include "transport_catalogue.h"
#include <utility>
#include <algorithm>

namespace transport_catalogue
{
	//------------------------------TransportCatalogue--------------------

	void TransportCatalogue::SetDistanceBetweenStops(const std::string_view from_stop, const std::string_view to_stop, const int distance)
	{
		auto lhs = FindStop(from_stop);
		auto rhs = FindStop(to_stop);
		stop_to_stop_distance_[std::make_pair(lhs, rhs)] = distance;
	}

	int TransportCatalogue::GetDistanceBetweenStops(domain::StopPtr lhs, domain::StopPtr rhs) const
	{
		return (stop_to_stop_distance_.find(std::make_pair(lhs, rhs)) == stop_to_stop_distance_.end()) ?
			stop_to_stop_distance_.at(std::make_pair(rhs, lhs)) :
			stop_to_stop_distance_.at(std::make_pair(lhs, rhs));
	}

	void TransportCatalogue::AddBusDatabase(const std::string_view bus_num, const std::vector<std::string_view>& stops, const bool is_circular)
	{
		std::vector<domain::StopPtr>stops_ptr;
		std::unordered_set<domain::StopPtr> unique_stops_ptr;
		for (auto& stop : stops)
		{
			auto a = FindStop(stop);
			stops_ptr.emplace_back(a);
			unique_stops_ptr.emplace(a);
		}
		buses_.emplace_front(domain::Bus(bus_num, stops_ptr, move(unique_stops_ptr), is_circular));
		buses_map_[buses_.front().bus_num] = &buses_.front();
	}

	void TransportCatalogue::AddStopDatabase(const std::string_view stop_name, const double lat, const double lng)
	{
		stops_.emplace_front(domain::Stop(stop_name, lat, lng));
		stops_map_[stops_.front().stop_name] = &stops_.front();
	}

	domain::BusPtr TransportCatalogue::FindBus(const std::string_view bus_num) const
	{
		return (buses_map_.find(bus_num) == buses_map_.end()) ? nullptr : buses_map_.at(bus_num);
	}

	domain::StopPtr TransportCatalogue::FindStop(const std::string_view stop_name) const
	{
		return (stops_map_.find(stop_name) == stops_map_.end()) ? nullptr : stops_map_.at(stop_name);
	}

	const domain::StopStat* TransportCatalogue::GetStopStat(const std::string_view stop_name) const
	{
		auto stop = FindStop(stop_name);
		if (stop == nullptr)
		{
			return nullptr;
		}
		std::set<std::string_view> buses;
		for (const auto& bus : buses_)
		{
			if (bus.unique_stops.find(stop) != bus.unique_stops.end())
			{
				buses.emplace(bus.bus_num);
			}
		}
		return new domain::StopStat(stop->stop_name, buses);
	}

	const domain::BusStat* TransportCatalogue::GetBusStat(const std::string_view bus_num) const
	{
		auto bus = FindBus(bus_num);
		if (bus == nullptr)
		{
			return nullptr;
		}
		int64_t route_length = 0;
		double curvature = 0.0;
		for (auto it = bus->stops.begin(); it < bus->stops.end() - 1; ++it)
		{
			auto stop = *it;
			auto d_it = it;
			auto second_stop = *++d_it;
			curvature += geo::ComputeDistance(second_stop->coodinates, stop->coodinates);
			route_length += GetDistanceBetweenStops(stop, second_stop);
			if (bus->is_circular == false)
			{
				curvature += geo::ComputeDistance(stop->coodinates, second_stop->coodinates);
				route_length += GetDistanceBetweenStops(second_stop, stop);
			}
		}
		double C = (curvature > route_length) ? curvature / route_length : route_length / curvature;
		return new domain::BusStat(bus->bus_num, (bus->is_circular == false) ? bus->stops.size() * 2 - 1 : bus->stops.size(),
			bus->unique_stops.size(), route_length, C);
	}

	const std::vector<geo::Coordinates> TransportCatalogue::GetAllStopsCoordinates() const
	{
		std::vector<geo::Coordinates>stops_coordinates;
		for (const auto& bus : buses_)
		{
			for (const auto& stop : bus.stops)
			{
				stops_coordinates.emplace_back(stop->coodinates);
			}
		}
		return stops_coordinates;
	}

	const std::vector<domain::StopPtr> TransportCatalogue::GetAllStops() const
	{
		std::vector<domain::StopPtr>stops;
		for (const auto& [name, bus_ptr] : stops_map_)
		{
			stops.push_back(bus_ptr);
		}
		return stops;
	}

	const std::vector<domain::StopPtr> TransportCatalogue::GetStops(const std::string_view bus_num) const
	{
		std::vector<domain::StopPtr>stops;
		for (const auto& stop : buses_map_.at(bus_num)->stops)
		{
			stops.emplace_back(stop);
		}
		return stops;
	}

	const std::deque<domain::BusPtr> TransportCatalogue::GetBuses() const
	{
		std::deque<domain::BusPtr> buses;
		for (const auto& bus : buses_)
		{
			buses.emplace_back(&bus);
		}
		std::sort(buses.begin(), buses.end(), [](domain::BusPtr lhs, domain::BusPtr rhs)
			{
				return lhs->bus_num <= rhs->bus_num;
			});
		return buses;
	}

	double TransportCatalogue::ComputeFactGeoLength(domain::StopPtr const prev_stop, domain::StopPtr const next_stop) const
	{
		double fact_distanse = 0.0;
		const auto dist_it = stop_to_stop_distance_.find({ prev_stop, next_stop });
		if (dist_it == stop_to_stop_distance_.end()) {
			const auto dist_reverse_it_ = stop_to_stop_distance_.find({ next_stop, prev_stop });
			(dist_reverse_it_ != stop_to_stop_distance_.end()) ?
				fact_distanse = dist_reverse_it_->second :
				fact_distanse = -1.0;
		}
		else {
			fact_distanse = dist_it->second;
		}
		return fact_distanse;
	}


} // namespace transport_catalogue

