#include "transport_catalogue.h"
#include <utility>
#include <algorithm>

namespace transport_catalogue
{

	TransportCatalogue::StopStat::StopStat(const std::string_view& stop_name, const std::set<std::string_view>& buses)
		: stop_name_(stop_name), buses_(buses) {}

	TransportCatalogue::BusStat::BusStat(const std::string_view& bus_num, const int& stops_on_route, const int& unique_stops, const int64_t& route_length, const double& curvature)
		: bus_num_(bus_num), stops_on_route_(stops_on_route), unique_stops_(unique_stops), route_length_(route_length), curvature_(curvature) {}

	TransportCatalogue::Stop::Stop(const std::string_view& stop_name, const double& lat, const double& lng)
		: stop_name_(stop_name)
	{
		coodinates_.lat = lat;
		coodinates_.lng = lng;
	}

	TransportCatalogue::Bus::Bus(const std::string_view& bus_num, const std::vector<StopPtr>& stops, const std::unordered_set<StopPtr>& unicue_stops, const bool& cicle_type)
		: bus_num_(bus_num), stops_(stops), unique_stops_(unicue_stops), cicle_type_(cicle_type) {}

	void TransportCatalogue::SetDistanceBetweenStops(const std::string_view& from_stop, const std::string_view& to_stop, const int& distance)
	{
		auto lhs = FindStop(from_stop);
		auto rhs = FindStop(to_stop);
		stop_to_stop_distance_[std::make_pair(lhs, rhs)] = distance;
	}

	int TransportCatalogue::GetDistanceBetweenStops(StopPtr lhs, StopPtr rhs) const
	{
		return (stop_to_stop_distance_.find(std::make_pair(lhs, rhs)) == stop_to_stop_distance_.end()) ?
			stop_to_stop_distance_.at(std::make_pair(rhs, lhs)) :
			stop_to_stop_distance_.at(std::make_pair(lhs, rhs));
	}

	void TransportCatalogue::AddBusDatabase(const std::string_view& bus_num, const std::vector<std::string_view>& stops, const bool& cicle_type)
	{
		std::vector<StopPtr>stops_ptr;
		std::unordered_set<StopPtr> unicue_stops_ptr;
		for (auto& stop : stops)
		{
			auto a = FindStop(stop);
			stops_ptr.emplace_back(a);
			unicue_stops_ptr.emplace(a);
		}
		buses_.emplace_front(Bus(bus_num, stops_ptr, move(unicue_stops_ptr), cicle_type));
		buses_map_[buses_.front().bus_num_] = &buses_.front();
	}

	void TransportCatalogue::AddStopDatabase(const std::string_view& stop_name, const double& lat, const double& lng)
	{
		stops_.emplace_front(Stop(stop_name, lat, lng));
		stops_map_[stops_.front().stop_name_] = &stops_.front();
	}

	TransportCatalogue::BusPtr TransportCatalogue::FindBus(const std::string_view& bus_num) const
	{
		return (buses_map_.find(bus_num) == buses_map_.end()) ? nullptr : buses_map_.at(bus_num);
	}

	TransportCatalogue::StopPtr TransportCatalogue::FindStop(const std::string_view& stop_name) const
	{
		return (stops_map_.find(stop_name) == stops_map_.end()) ? nullptr : stops_map_.at(stop_name);
	}

	const TransportCatalogue::StopStat* TransportCatalogue::GetStopStat(const std::string_view& stop_name) const
	{
		auto stop = FindStop(stop_name);
		if (stop == nullptr)
		{
			return nullptr;
		}
		std::set<std::string_view> buses;
		for (const auto& bus : buses_)
		{
			if (bus.unique_stops_.find(stop) != bus.unique_stops_.end())
			{
				buses.emplace(bus.bus_num_);
			}
		}
		return new StopStat(stop->stop_name_, buses);
	}

	const TransportCatalogue::BusStat* TransportCatalogue::GetBusStat(const std::string_view& bus_name) const
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
			if (bus->cicle_type_ == false)
			{
				curvature += geo::ComputeDistance(stop->coodinates_, second_stop->coodinates_);
				route_length += GetDistanceBetweenStops(second_stop, stop);
			}
		}
		double C = (curvature > route_length) ? curvature / route_length : route_length / curvature;
		return new BusStat(bus->bus_num_, (bus->cicle_type_ == false) ? bus->stops_.size() * 2 - 1 : bus->stops_.size(),
			bus->unique_stops_.size(), route_length, C);
	}

} // namespace transport_catalogue

