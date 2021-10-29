#include "request_handler.h"

/*
 * ����� ����� ���� �� ���������� ��� ����������� �������� � ����, ����������� ������, ������� ��
 * �������� �� �������� �� � transport_catalogue, �� � json reader.
 *
 * ���� �� ������������� �������, ��� ����� ���� �� ��������� � ���� ����,
 * ������ �������� ��� ������.
 */

namespace request_handler
{
	RequestHandler::RequestHandler(transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer)
		: db_(db), renderer_(renderer){}

	const domain::StopStat* RequestHandler::GetStopStat(const std::string_view& stop_name) const
	{
		return db_.GetStopInfo(stop_name);
	}

	const domain::BusStat* RequestHandler::GetBusStat(const std::string_view& bus_name) const
	{
		return db_.GetRoute(bus_name);
	}

	const std::unordered_set<domain::BusPtr>* RequestHandler::GetBusesByStop(const std::string_view& stop_name) const
	{
		return nullptr;
	}

	svg::Document RequestHandler::RenderMap() const
	{
		return svg::Document();
	}

} // namespace request_handler