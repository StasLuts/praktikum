#pragma once

#include "transport_catalogue.h"
#include "domain.h"
#include "router.h"

namespace transport_router
{
	class TransportRouter
	{
	public:

		struct RouteData
		{
			double total_time = 0.0;
			std::vector<std::string_view> items;
		};

		TransportRouter(const transport_catalogue::TransportCatalogue& trans_cat);

		void SetRoutingSettings(const int bus_wait_time, const double bus_velocity);

		std::optional<graph::Router<double>::RouteInfo> GetRoute(const std::string_view from, const std::string_view to);

	private:

		const transport_catalogue::TransportCatalogue& trans_cat_;
		domain::RoutingSettings settings_;
		graph::DirectedWeightedGraph<double> graph_;
		std::shared_ptr<graph::Router<double>> router_;
		std::map<std::string_view, size_t> vertex_wait;
		std::map<std::string_view, size_t> vertex_move;

		void FillGraph();
	};

} // namespace transport_router
