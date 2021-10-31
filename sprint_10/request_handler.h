#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"

namespace request_handler
{
    class RequestHandler
    {
    public:
        // MapRenderer ����������� � ��������� ����� ��������� �������
        RequestHandler(const transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer);

        // ���������� ���������� � �������� (������ Bus)
        std::optional<const domain::BusStat*> GetBusStat(const std::string_view& bus_name) const;

        std::optional<const domain::StopStat*> GetStopStat(const std::string_view& stop_name) const;

        // ���������� ��������, ���������� �����
        const std::unordered_set<domain::BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

        // ���� ����� ����� ����� � ��������� ����� ��������� �������
        svg::Document RenderMap() const;

    private:
        // RequestHandler ���������� ��������� �������� "������������ ����������" � "������������ �����"
        const transport_catalogue::TransportCatalogue& db_;
        const renderer::MapRenderer& renderer_;
    };

} // namespace request_handler