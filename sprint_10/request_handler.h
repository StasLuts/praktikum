#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"

namespace request_handler
{
    class RequestHandler
    {
    public:

        // MapRenderer ����������� � ��������� ����� ��������� �������
        RequestHandler(transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer);

        const domain::StopStat* GetStopStat(const std::string_view&) const;

        // ���������� ���������� � �������� (������ Bus)
        const domain::BusStat* GetBusStat(const std::string_view& bus_name) const;

        // ���������� ��������, ���������� �����
        const std::unordered_set<domain::BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

        // ���� ����� ����� ����� � ��������� ����� ��������� �������
        svg::Document RenderMap() const;

    private:
        // RequestHandler ���������� ��������� �������� "������������ ����������" � "������������ �����"
        transport_catalogue::TransportCatalogue& db_;
        const renderer::MapRenderer& renderer_;
    };

} // request_handler