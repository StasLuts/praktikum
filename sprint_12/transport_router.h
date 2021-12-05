#pragma once

#include "transport_catalogue.h"
#include "domain.h"
#include "router.h"

namespace transport_router
{
	class TransportRouter
	{
	public:

		TransportRouter(const transport_catalogue::TransportCatalogue& trans_cat);

		void SetRoutingSettings(const int bus_wait_time, const double bus_velocity);

		std::optional<graph::Router<double>::RouteInfo> GetRoute(const std::string_view from, const std::string_view to);

	private:

		const transport_catalogue::TransportCatalogue& trans_cat_;
		domain::RoutingSettings settings_;
		std::unique_ptr<graph::Router<double>> router_ = nullptr;
		graph::DirectedWeightedGraph<double> graph_;
		std::unordered_map<std::string_view, int> stop_vertexid_map;

		void FillGraph();
	};

} // namespace transport_router
