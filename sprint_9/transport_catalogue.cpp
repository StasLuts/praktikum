#include "transport_catalogue.h"
#include <utility>
#include <algorithm>

transport_catalogue::TransportCatalogue::PrintStopData::PrintStopData(std::string_view stop_name, const std::set<std::string_view>& buses)
	:stop_name_(stop_name), buses_(buses)
{
}

transport_catalogue::TransportCatalogue::PrintData::PrintData(std::string_view bus_num, int stops_on_route, int unique_stops, int64_t route_length, const double curvature)
	:bus_num_(bus_num), stops_on_route_(stops_on_route), unique_stops_(unique_stops), route_length_(route_length), curvature_(curvature)
{
}

transport_catalogue::TransportCatalogue::Stop::Stop(std::string_view stop_name, const double lat, const double lng)
	:stop_name_(stop_name)
{
	coodinates_.lat = lat;
	coodinates_.lng = lng;
}

transport_catalogue::TransportCatalogue::Bus::Bus(std::string_view bus_num, const std::vector<const Stop*>& stops, const std::unordered_set<const Stop*>& unicue_stops, bool cicle_type)
	:bus_num_(bus_num), stops_(stops), unicue_stops_(unicue_stops), cicle_type_(cicle_type)
{
}

void transport_catalogue::TransportCatalogue::Set_Distance_Between_Stops(std::string_view from_stop, std::string_view to_stop, int distance)
{
	auto lhs = Find_Stop(from_stop);
	auto rhs = Find_Stop(to_stop);
	stop_to_stop_distance_[std::make_pair(lhs, rhs)] = distance;
}

int transport_catalogue::TransportCatalogue::Get_Distance_Between_Stops(const Stop* lhs, const Stop* rhs)
{
	return (stop_to_stop_distance_.find(std::make_pair(lhs, rhs)) == stop_to_stop_distance_.end()) ?
		stop_to_stop_distance_.at(std::make_pair(rhs, lhs)) :
		stop_to_stop_distance_[std::make_pair(lhs, rhs)];
}

void transport_catalogue::TransportCatalogue::Adding_Bus_Database(std::string_view bus_num, std::vector<std::string_view>& stops, bool cicle_type)
{
	std::vector<const Stop*>stops_ptr;
	std::unordered_set<const Stop*> unicue_stops_ptr;
	for (auto& stop : stops)
	{
		auto a = Find_Stop(move(stop));
		stops_ptr.emplace_back(a);
		unicue_stops_ptr.emplace(a);
	}
	buses_.emplace_front(Bus(bus_num, stops_ptr, unicue_stops_ptr, cicle_type));
	buses_map_[buses_.front().bus_num_] = &buses_.front();
}

void transport_catalogue::TransportCatalogue::Adding_Stop_Database(std::string_view stop_name, const double lat, const double lng)
{
	stops_.emplace_front(Stop(stop_name, lat, lng));
	stops_map_[stops_.front().stop_name_] = &stops_.front();
}

const transport_catalogue::TransportCatalogue::Bus* transport_catalogue::TransportCatalogue::Find_Bus(std::string_view bus_num) const
{
	return (buses_map_.find(bus_num) == buses_map_.end()) ? nullptr : buses_map_.at(bus_num);
}

const transport_catalogue::TransportCatalogue::Stop* transport_catalogue::TransportCatalogue::Find_Stop(std::string_view stop_name) const
{
	return (stops_map_.find(stop_name) == stops_map_.end()) ? nullptr : stops_map_.at(stop_name);
}

transport_catalogue::TransportCatalogue::PrintStopData* transport_catalogue::TransportCatalogue::Get_Stop_Info(std::string_view stop_name)
{
	auto stop = Find_Stop(stop_name);
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
	return new PrintStopData(stop->stop_name_, buses);
}

transport_catalogue::TransportCatalogue::PrintData* transport_catalogue::TransportCatalogue::Get_Route(std::string_view bus_num)
{
	auto bus = Find_Bus(bus_num);
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
		curvature += transport_catalogue::geo::ComputeDistance(second_stop->coodinates_, stop->coodinates_);
		route_length += Get_Distance_Between_Stops(stop, second_stop);
	}
	if (bus->cicle_type_ == false)
	{
		for (auto it = bus->stops_.end() - 1; it > bus->stops_.begin(); --it)
		{
			auto stop = *it;
			auto d_it = it;
			auto second_stop = *--d_it;
			route_length += Get_Distance_Between_Stops(stop, second_stop);
			curvature += transport_catalogue::geo::ComputeDistance(second_stop->coodinates_, stop->coodinates_);
		}
	}
	double C = (curvature > route_length) ? curvature / route_length : route_length / curvature;
	return new PrintData(bus->bus_num_, (bus->cicle_type_ == false) ? bus->stops_.size() * 2 - 1 : bus->stops_.size(),
		bus->unicue_stops_.size(), route_length, C);
}

