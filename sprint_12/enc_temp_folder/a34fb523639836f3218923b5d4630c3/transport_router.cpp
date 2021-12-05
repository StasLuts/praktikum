#include "transport_router.h"

namespace transport_router
{
	//-----------TransportRouter---------------

	TransportRouter::TransportRouter(const transport_catalogue::TransportCatalogue& trans_cat)
		: trans_cat_(trans_cat) {}

	void TransportRouter::SetRoutingSettings(const int bus_wait_time, const double bus_velocity)
	{
		settings_.bus_wait_time = bus_wait_time;
		settings_.bus_velocity = bus_velocity;
	}

	std::optional<graph::Router<double>::RouteInfo> TransportRouter::GetRoute(const std::string_view from, const std::string_view to)
	{
		if (router_)
		{
			router_.release();
		}
		FillGraph();
		return router_->BuildRoute(stop_vertexid_map.find(from)->second, stop_vertexid_map.find(to)->second);
	}

	//-----------TransportRouter(private)---------------

	void TransportRouter::FillGraph()
	{
		graph_ = graph::DirectedWeightedGraph<double>(trans_cat_.GetAllStops().size());
		for (const auto& route : trans_cat_.GetBuses())
		{
			double road_distance = 0.0;
			for (size_t stop_from = 0, stop_to = 1; stop_from < route->stops.size(); ++stop_from, ++stop_to)
			{
				stop_vertexid_map.insert({ route->stops[stop_from]->stop_name, stop_from });
				graph_.AddEdge({ stop_from, stop_from, settings_.bus_wait_time * 1.0, route->stops[stop_from]->stop_name ,graph::EdgeType::WAIT });
				road_distance += trans_cat_.GetDistanceBetweenStops(route->stops[stop_from], route->stops[stop_to]);
				graph_.AddEdge({ stop_from, stop_to, (road_distance / (settings_.bus_velocity * 1.0 * 1000)) * 60, route->bus_num ,graph::EdgeType::BUS, stop_to });
			}
		}
		router_ = std::make_unique<graph::Router<double>>(graph_);
	}

} // namespace transport_router