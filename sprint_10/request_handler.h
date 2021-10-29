#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"

namespace request_handler
{
    class RequestHandler
    {
    public:

        // MapRenderer понадобится в следующей части итогового проекта
        RequestHandler(transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer);

        const domain::StopStat* GetStopStat(const std::string_view&) const;

        // Возвращает информацию о маршруте (запрос Bus)
        const domain::BusStat* GetBusStat(const std::string_view& bus_name) const;

        // Возвращает маршруты, проходящие через
        const std::unordered_set<domain::BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

        // Этот метод будет нужен в следующей части итогового проекта
        svg::Document RenderMap() const;

    private:
        // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
        transport_catalogue::TransportCatalogue& db_;
        const renderer::MapRenderer& renderer_;
    };

} // request_handler