#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"

namespace request_handler
{
    class RequestHandler
    {
    public:

        RequestHandler(const transport_catalogue::TransportCatalogue&, const renderer::MapRenderer&);

        std::optional<const domain::BusStat*> GetBusStat(const std::string_view&) const;

        std::optional<const domain::StopStat*> GetStopStat(const std::string_view&) const;

        svg::Document RenderMap() const;

    private:

        const transport_catalogue::TransportCatalogue& db_;
        const renderer::MapRenderer& renderer_;
    };

} // namespace request_handler