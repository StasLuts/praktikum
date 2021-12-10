#pragma once

#include "transport_catalogue.h"
#include "domain.h"
#include "router.h"
#include <memory>

namespace transport_router
{
	class TransportRouter
	{
	public:

		struct Item
		{
			std::string bus_or_stop_name;
			int span_count;
			double time;
			graph::EdgeType type;
		};

		struct RouteData
		{
			double total_time = 0.0;
			std::vector<Item> items;
			bool find = false;
		};

		TransportRouter(const transport_catalogue::TransportCatalogue& trans_cat);

		void SetRoutingSettings(const int bus_wait_time, const double bus_velocity);

		const RouteData GetRoute(const std::string_view from, const std::string_view to);

	private:

		const transport_catalogue::TransportCatalogue& trans_cat_;
		domain::RoutingSettings settings_;
		graph::DirectedWeightedGraph<double> graph_;
		std::unique_ptr<graph::Router<double>> router_;
		std::unordered_map<std::string_view, size_t> vertex_wait;
		std::unordered_map<std::string_view, size_t> vertex_move;

		void FillGraph();
	};

} // namespace transport_router
