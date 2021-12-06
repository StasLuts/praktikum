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
		FillGraph();
		auto r = router_->BuildRoute(vertex_wait.find(from)->second, vertex_move.find(to)->second);
		RouteData result;
		for (const auto& ro : r->edges)
		{
			auto f = graph_.GetEdge(ro);
			result.total_time += f.weight;
			result.items.push_back(f.bus_or_stop_name);
		}
		return std::nullopt;
	}

	//-----------TransportRouter(private)---------------

	void TransportRouter::FillGraph()
	{
		graph_ = graph::DirectedWeightedGraph<double>(trans_cat_.GetAllStops().size() * 3); // инициализируем колличество вершин

		// заполняем карту вершин ожидания и движения
		size_t vertex_id = 0;
		for (const auto& stop : trans_cat_.GetAllStops())
		{
			vertex_wait.insert({ stop->stop_name, vertex_id });
			vertex_move.insert({ stop->stop_name, ++vertex_id });
			graph_.AddEdge(
				{
					vertex_wait.at(stop->stop_name),
					vertex_move.at(stop->stop_name),
					settings_.bus_wait_time * 1.0,
					stop->stop_name,
					graph::EdgeType::WAIT
				});
			++vertex_id;
		}

		constexpr double km_to_min = 1000 * 1.0 / 60;

		// между оствновками делвем ребро поездки
		for (const auto& route : trans_cat_.GetBuses())
		{
			for (size_t it_from = 0; it_from < route->stops.size(); ++it_from)
			{
				double road_distance = 0.0;
				for (size_t it_to = it_from + 1; it_to < route->stops.size(); ++it_to)
				{
					if (route->stops[it_from] == route->stops[it_to] || (route->stops[it_from] == *route->stops.begin() && route->stops[it_to] == route->stops.back()))
					{
						road_distance = trans_cat_.GetBusStat(route->bus_num)->route_length;
					}
					else
					{
						road_distance += trans_cat_.GetDistanceBetweenStops(route->stops[it_from], route->stops[it_to]);
					}
					graph_.AddEdge(
						{
							vertex_move.at(route->stops[it_from]->stop_name),
							vertex_move.at(route->stops[it_to]->stop_name),
							road_distance / (settings_.bus_velocity * km_to_min),
							route->bus_num,
							graph::EdgeType::BUS
						});
				}
			}

			/*for (size_t it_from = 0; it_from < route->stops.size(); ++it_from)
			{
				for (size_t it_to = 1; it_from < route->stops.size() - 1; ++it_from)
				{

				}
			}*/
		}

		graph::Router<double> router(graph_);
		router_ = std::make_unique<graph::Router<double>>(router);
	}

} // namespace transport_router