#include "transport_router.h"

namespace transport_router
{
	//-----------TransportRouter---------------

	TransportRouter::TransportRouter(const transport_catalogue::TransportCatalogue& trans_cat)
		: trans_cat_(trans_cat), graph_(trans_cat_.GetAllStops().size() * 2) {}

	void TransportRouter::SetRoutingSettings(const int bus_wait_time, const double bus_velocity)
	{
		settings_.bus_wait_time = bus_wait_time;
		settings_.bus_velocity = bus_velocity;
	}

	const TransportRouter::RouteData TransportRouter::GetRoute(const std::string_view from, const std::string_view to)
	{
		if (!router_)
		{
			FillGraph();
		}
		auto r = router_->BuildRoute(vertex_wait.at(from), vertex_wait.at(to));
		RouteData result;
		for (const auto& ro : r->edges)
		{
			auto f = graph_.GetEdge(ro);
			result.total_time += f.weight;
			result.items.emplace_back(
				f.bus_or_stop_name,
				(f.type == graph::EdgeType::BUS) ? f.span_count : 0,
				f.weight,
				f.type);
		}
		return result;
	}

	//-----------TransportRouter(private)---------------

	void TransportRouter::FillGraph()
	{
		size_t vertex_id = 0;
		for (const auto& stop : trans_cat_.GetAllStops())
		{
			vertex_wait.insert({ stop->stop_name, vertex_id });
			vertex_move.insert({ stop->stop_name, ++vertex_id });
			graph_.AddEdge({
					vertex_wait.at(stop->stop_name),
					vertex_move.at(stop->stop_name),
					settings_.bus_wait_time * 1.0,
					stop->stop_name,
					graph::EdgeType::WAIT
				});
			++vertex_id;
		}
		for (const auto& route : trans_cat_.GetBuses())
		{
			// туда
			for (size_t it_from = 0; it_from < route->stops.size(); ++it_from)
			{ 
				double road_distance = 0.0;
				for (size_t it_to = it_from + 1; it_to < route->stops.size(); ++it_to)
				{
 					road_distance += geo::ComputeDistance(route->stops[it_from]->coodinates, route->stops[it_to]->coodinates);
					graph_.AddEdge({
							vertex_move.at(route->stops[it_from]->stop_name),
							vertex_wait.at(route->stops[it_to]->stop_name),
							road_distance / (settings_.bus_velocity * 1000 / 60),
							route->bus_num,
							graph::EdgeType::BUS,
							it_to
						});
				}
			}
			if (!route->is_circular)
			{
				for (int it_from = route->stops.size() - 1; it_from > 0; --it_from)
				{
					double road_distance = 0.0;
					for (int it_to = it_from - 1; it_to > 0; --it_to)
					{
						road_distance += geo::ComputeDistance(route->stops[it_from]->coodinates, route->stops[it_to]->coodinates);
						graph_.AddEdge({
								vertex_move.at(route->stops[it_from]->stop_name),
								vertex_wait.at(route->stops[it_to]->stop_name),
								road_distance / (settings_.bus_velocity * 1000 / 60),
								route->bus_num,
								graph::EdgeType::BUS,
								it_to
							});
					}
				}
			}
		}
		graph::Router<double> router(graph_);
		router_ = std::make_unique<graph::Router<double>>(router);
	}

} // namespace transport_router