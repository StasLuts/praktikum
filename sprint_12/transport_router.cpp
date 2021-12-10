#include "transport_router.h"
#include <execution>
#include <mutex>

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
		RouteData result;
		if (vertex_wait.find(from) != vertex_wait.end() && vertex_wait.find(to) != vertex_wait.end())
		{
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

		const auto all_rauts = trans_cat_.GetBuses(); //route
		std::mutex mut;
		std::for_each(std::execution::par, all_rauts.begin(), all_rauts.end(), [&](const auto& route)
			{
				// туда
				for (size_t it_from = 0; it_from < route->stops.size() - 1; ++it_from)
				{
					int span_count = 0;
					for (size_t it_to = it_from + 1; it_to < route->stops.size(); ++it_to)
					{
						double road_distance = 0.0;
						for (size_t k = it_from + 1; k <= it_to; ++k) {
							road_distance += trans_cat_.ComputeFactGeoLength(route->stops[k - 1], route->stops[k]);
						}
						std::lock_guard<std::mutex> lock(mut);
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
				if (*route->stops.begin() != route->stops.back())
				{
					for (int it_from = route->stops.size() - 1; it_from > 0; --it_from)
					{
						int span_count = 0;
						for (int it_to = it_from - 1; it_to >= 0; --it_to)
						{
							double road_distance = 0.0;
							for (int k = it_from; k > it_to; --k) {
								road_distance += trans_cat_.ComputeFactGeoLength(route->stops[k], route->stops[k - 1]);
							}
							std::lock_guard<std::mutex> lock(mut);
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
			});
		graph::Router<double> router(graph_);
		router_ = std::make_unique<graph::Router<double>>(router);
	}

} // namespace transport_router