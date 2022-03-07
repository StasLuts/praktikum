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

	const domain::RoutingSettings& TransportRouter::GetRoutingSettings() const
	{
		return settings_;
	}

	const TransportRouter::RouteData TransportRouter::GetRoute(const std::string_view from, const std::string_view to)
	{
		if (!router_)
		{
			FillGraph();
		}
		RouteData result;
		auto r = router_->BuildRoute(vertex_wait.at(from), vertex_wait.at(to));
		if (r)
		{
			result.find = true;
			for (const auto& ro : r->edges)
			{
				auto f = graph_.GetEdge(ro);
				result.total_time += f.weight;
				result.items.emplace_back(Item{
					f.bus_or_stop_name,
					(f.type == graph::EdgeType::BUS) ? f.span_count : 0,
					f.weight,
					f.type });
			}
		}
		return result;
	}

	//-----------TransportRouter(private)---------------

	void TransportRouter::FillGraph()
	{
		int vertex_id = 0;
		for (const auto& stop : trans_cat_.GetAllStops())
		{
			vertex_wait.insert({ stop->stop_name, vertex_id });
			vertex_move.insert({ stop->stop_name, ++vertex_id });
			graph_.AddEdge({
					vertex_wait.at(stop->stop_name),
					vertex_move.at(stop->stop_name),
					settings_.bus_wait_time * 1.0,
					stop->stop_name,
					graph::EdgeType::WAIT,
					0
				});
			++vertex_id;
		}
		for (const auto& route : trans_cat_.GetBuses())
		{
			for (size_t it_from = 0; it_from < route->stops.size() - 1; ++it_from)
			{
				int span_count = 0;
				for (size_t it_to = it_from + 1; it_to < route->stops.size(); ++it_to)
				{
					double road_distance = 0.0;
					for (size_t it = it_from + 1; it <= it_to; ++it)
					{
						road_distance += trans_cat_.ComputeRoadDistance(route->stops[it - 1], route->stops[it]);
					}
					graph_.AddEdge({
							vertex_move.at(route->stops[it_from]->stop_name),
							vertex_wait.at(route->stops[it_to]->stop_name),
							road_distance / (settings_.bus_velocity * 1000 / 60),
							route->bus_num,
							graph::EdgeType::BUS,
							++span_count
						});
				}
			}
			if (!route->is_circular)
			{
				for (int it_from = route->stops.size() - 1; it_from > 0; --it_from)
				{
					int span_count = 0;
					for (int it_to = it_from - 1; it_to >= 0; --it_to)
					{
						double road_distance = 0.0;
						for (int it = it_from; it > it_to; --it)
						{
							road_distance += trans_cat_.ComputeRoadDistance(route->stops[it], route->stops[it - 1]);
						}
						graph_.AddEdge({
								vertex_move.at(route->stops[it_from]->stop_name),
								vertex_wait.at(route->stops[it_to]->stop_name),
								road_distance / (settings_.bus_velocity * 1000 / 60),
								route->bus_num,
								graph::EdgeType::BUS,
								++span_count
							});
					}
				}
			}
		}
		router_ = std::make_unique<graph::Router<double>>(graph_);
	}

} // namespace transport_router