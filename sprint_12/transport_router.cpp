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
		return router_->BuildRoute(0, 0);
	}

	//-----------TransportRouter(private)---------------

	void TransportRouter::FillGraph()
	{
		size_t edge_num = 0;
		for (const auto& bus : trans_cat_.GetBuses())
		{
			for(const auto& stop : bus->stops)
			{
				stop_vertexid_map[stop->stop_name] = edge_num;
				graph_.AddEdge({edge_num, edge_num, settings_.bus_wait_time, graph::EdgeType::WAIT, });
			}
		}
	}

} // namespace transport_router