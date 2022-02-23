#pragma once

#include "transport_catalogue.h"
#include "transport_router.h"
#include "map_renderer.h"

namespace request_handler
{
    class RequestHandler
    {
    public:

        RequestHandler(const transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer);

        std::optional<const domain::BusStat*> GetBusStat(const std::string_view& bus_name) const;

        std::optional<const domain::StopStat*> GetStopStat(const std::string_view& stop_name) const;

        svg::Document RenderMap() const;

        std::optional<graph::Router<double>::RouteInfo> GetRoute(const std::string_view from, const std::string_view to) const;

    private:

        const transport_catalogue::TransportCatalogue& db_;
        const renderer::MapRenderer& renderer_;
    };

} // namespace request_handler