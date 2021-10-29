#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
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