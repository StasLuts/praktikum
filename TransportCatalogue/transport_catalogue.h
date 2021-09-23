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

class TransportCatalogue
{

private:

	struct PrintStopData
	{
		PrintStopData(std::string_view stop_name, const std::set<std::string_view>& buses);
		std::string stop_name_;
		std::set<std::string_view> buses_;
	};

	struct PrintData
	{
		PrintData(std::string_view bus_num, int stops_on_route, int unique_stops, int64_t route_length, const double curvature);
		std::string bus_num_;
		int stops_on_route_;
		int unique_stops_;
		int64_t route_length_;
		double curvature_;
	};

	struct Stop
	{
		Stop(std::string_view stop_name, const double lat, const double lng);
		std::string stop_name_;
		Coordinates coodinates_;
	};

	struct Bus
	{
		Bus(std::string_view bus_num, const std::vector<const Stop*>& stops, const std::unordered_set<const Stop*>& unicue_stops, bool cicle_type);
		std::string bus_num_;
		std::vector<const Stop*> stops_;
		std::unordered_set<const Stop*> unicue_stops_;
		bool cicle_type_;
	};

	struct PairStopsHasher
	{
		size_t operator() (std::pair<const Stop*, const Stop*> stops) const 
		{
			return pair_hasher_(stops.first) + pair_hasher_(stops.second);
		}

	private:

		std::hash<const void*> pair_hasher_;
	};

	std::deque<Bus> buses_;
	std::deque<Stop> stops_;
	std::unordered_map<std::string_view, const Bus*> buses_map_;
	std::unordered_map<std::string_view, const Stop*> stops_map_;
	std::unordered_map<std::pair<const Stop*, const Stop*>, int, PairStopsHasher> stop_to_stop_distance_;

public:

	void Set_Distance_Between_Stops(std::string_view from_stop, std::string_view to_stop, int distance);

	int Get_Distance_Between_Stops(const Stop* lhs, const Stop* rhs);

	//int Get_Distance_Between_Stops(std::string_view from_stop, std::string_view to_stop);

	void Adding_Bus_Database(std::string_view bus_num, std::vector<std::string_view>& stops, bool cicle_type);

	void Adding_Stop_Database(std::string_view stop_name, const double lat, const double lng);

	const Bus* Find_Bus(std::string_view bus_num) const;

	const Stop* Find_Stop(std::string_view stop_name) const;

	PrintStopData* Get_Stop_Info(std::string_view stop_name);

	PrintData* Get_Route(std::string_view bus_num);
};