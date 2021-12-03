#include "request_handler.h"

namespace request_handler
{
	RequestHandler::RequestHandler(const transport_catalogue::TransportCatalogue& db, transport_router::TransportRouter& tr, const renderer::MapRenderer& renderer)
		: db_(db), tr_(tr), renderer_(renderer) {}

	std::optional<const domain::BusStat*> RequestHandler::GetBusStat(const std::string_view& bus_name) const
	{
		return db_.GetBusStat(bus_name);
	}

	std::optional<const domain::StopStat*> RequestHandler::GetStopStat(const std::string_view& stop_name) const
	{
		return db_.GetStopStat(stop_name);
	}

	svg::Document RequestHandler::RenderMap() const
	{
		return renderer_.GetRender();
	}

	std::optional<graph::Router<double>::RouteInfo> RequestHandler::GetRoute(const std::string_view from, const std::string_view to) const
	{
		return tr_.GetRoute(from, to);
	}

} // namespace request_handler