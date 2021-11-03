#include "transport_catalogue.h"
#include <utility>
#include <algorithm>

namespace transport_catalogue
{
	void TransportCatalogue::SetDistanceBetweenStops(std::string_view from_stop, std::string_view to_stop, int distance)
	{
		auto lhs = FindStop(from_stop);
		auto rhs = FindStop(to_stop);
		stop_to_stop_distance_[std::make_pair(lhs, rhs)] = distance;
	}

	int TransportCatalogue::GetDistanceBetweenStops(const domain::Stop* lhs, const domain::Stop* rhs) const
	{
		return (stop_to_stop_distance_.find(std::make_pair(lhs, rhs)) == stop_to_stop_distance_.end()) ?
			stop_to_stop_distance_.at(std::make_pair(rhs, lhs)) :
			stop_to_stop_distance_.at(std::make_pair(lhs, rhs));//¿œ—Õ€
	}

	void TransportCatalogue::AddingBusDatabase(std::string_view bus_num, std::vector<std::string_view>& stops, bool cicle_type)
	{
		std::vector<const domain::Stop*>stops_ptr;
		std::unordered_set<const domain::Stop*> unicue_stops_ptr;
		for (auto& stop : stops)
		{
			auto a = FindStop(move(stop));
			stops_ptr.emplace_back(a);
			unicue_stops_ptr.emplace(a);
		}
		buses_.emplace_front(domain::Bus(bus_num, stops_ptr, unicue_stops_ptr, cicle_type));
		buses_map_[buses_.front().bus_num_] = &buses_.front();
	}

	void TransportCatalogue::AddingStopDatabase(std::string_view stop_name, const double lat, const double lng)
	{
		stops_.emplace_front(domain::Stop(stop_name, lat, lng));
		stops_map_[stops_.front().stop_name_] = &stops_.front();
	}

	const domain::Bus* TransportCatalogue::FindBus(std::string_view bus_num) const
	{
		return (buses_map_.find(bus_num) == buses_map_.end()) ? nullptr : buses_map_.at(bus_num);
	}

	const domain::Stop* TransportCatalogue::FindStop(std::string_view stop_name) const
	{
		return (stops_map_.find(stop_name) == stops_map_.end()) ? nullptr : stops_map_.at(stop_name);
	}

	const domain::StopStat* TransportCatalogue::GetStopStat(std::string_view stop_name) const
	{
		auto stop = FindStop(stop_name);
		if (stop == nullptr)
		{
			return nullptr;
		}
		std::set<std::string_view> buses;
		for (const auto& bus : buses_)
		{
			if (bus.unicue_stops_.find(stop) != bus.unicue_stops_.end())
			{
				buses.emplace(bus.bus_num_);
			}
		}
		return new domain::StopStat(stop->stop_name_, buses);
	}

	const domain::BusStat* TransportCatalogue::GetBusStat(const std::string_view& bus_name) const
	{
		auto bus = FindBus(bus_name);
		if (bus == nullptr)
		{
			return nullptr;
		}
		int64_t route_length = 0;
		double curvature = 0.0;
		for (auto it = bus->stops_.begin(); it < bus->stops_.end() - 1; ++it)
		{
			auto stop = *it;
			auto d_it = it;
			auto second_stop = *++d_it;
			curvature += geo::ComputeDistance(second_stop->coodinates_, stop->coodinates_);
			route_length += GetDistanceBetweenStops(stop, second_stop);
		}
		if (bus->cicle_type_ == false)
		{
			for (auto it = bus->stops_.end() - 1; it > bus->stops_.begin(); --it)
			{
				auto stop = *it;
				auto d_it = it;
				auto second_stop = *--d_it;
				route_length += GetDistanceBetweenStops(stop, second_stop);
				curvature += geo::ComputeDistance(second_stop->coodinates_, stop->coodinates_);
			}
		}
		double C = (curvature > route_length) ? curvature / route_length : route_length / curvature;
		return new domain::BusStat(bus->bus_num_, (bus->cicle_type_ == false) ? bus->stops_.size() * 2 - 1 : bus->stops_.size(),
			bus->unicue_stops_.size(), route_length, C);
	}

	const std::vector<geo::Coordinates> TransportCatalogue::GetAllStopsCoordinates() const
	{
		std::vector<geo::Coordinates>stops_coordinates;
		for (const auto& bus : buses_)
		{
			for (const auto& stop : bus.stops_)
			{
				stops_coordinates.emplace_back(stop->coodinates_);
			}
		}
		return stops_coordinates;
	}

	const std::vector<geo::Coordinates> TransportCatalogue::GetStopsCoordinates(const std::string_view bus_name) const
	{
		std::vector<geo::Coordinates>stops_coordinates;
		for (const auto& stop : buses_map_.at(bus_name)->stops_)
		{
			stops_coordinates.emplace_back(stop->coodinates_);
		}
		return stops_coordinates;
	}

	const std::deque<domain::BusPtr> TransportCatalogue::GetBuses() const
	{
		std::deque<domain::BusPtr> buses; 
		for (const auto& bus : buses_)
		{
			buses.emplace_back(&bus);
		}
		return buses;
	}
} // namespace transport_catalogue

