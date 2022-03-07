#pragma once

#include "transport_router.h"
#include "json_builder.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "serialization.h"

namespace json_reader
{

	void JsonSerialize(std::istream& input);

	void JsonDeserialize(std::istream& input, std::ostream& output);

	//------------------input-------------------------

	void MakeBase(transport_catalogue::TransportCatalogue& trans_cat, const json::Array& arr);

	void ReadStopData(transport_catalogue::TransportCatalogue& trans_cat, const json::Dict& dict);

	void ReadStopDistance(transport_catalogue::TransportCatalogue& trans_cat, const json::Dict& dict);

	void ReadBusData(transport_catalogue::TransportCatalogue& trans_cat, const json::Dict& dict);

	void SetRoutingSettings(transport_router::TransportRouter& trans_roter, const json::Dict& dict);

	//------------------render-------------------------

	const svg::Color GetColor(const json::Node& color);

	void SetMapRenderer(const transport_catalogue::TransportCatalogue& trans_cat, renderer::MapRenderer& map_renderer, const json::Dict& dict);

	//------------------outnput-------------------------

	void MakeResponse(const request_handler::RequestHandler& request_handler, transport_router::TransportRouter& trans_roter, const json::Array& arr);

	const json::Node GetStopInfo(const request_handler::RequestHandler& request_handler, const json::Dict& dict);

	const json::Node GetBusInfo(const request_handler::RequestHandler& request_handler, const json::Dict& dict);

	const json::Node GetMapRender(const request_handler::RequestHandler& request_handler, const json::Dict& dict);

	const json::Node GetRouteInfo(transport_router::TransportRouter& trans_roter, const json::Dict& dict);

} // json_reader 