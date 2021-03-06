#pragma once

#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "request_handler.h"

namespace json_reader
{
	void JsonRead(std::istream& input);

	//------------------input-------------------------

	void MakeBase(transport_catalogue::TransportCatalogue& trans_cat, const json::Array& arr);

	void ReadStopData(transport_catalogue::TransportCatalogue& trans_cat, const json::Dict& dict);

	void ReadStopDistance(transport_catalogue::TransportCatalogue& trans_cat, const json::Dict& dict);

	void ReadBusData(transport_catalogue::TransportCatalogue& trans_cat, const json::Dict& dict);

	//------------------render-------------------------

	const svg::Color GetColor(const json::Node& color);

	void SetMapRenderer(const transport_catalogue::TransportCatalogue& trans_cat, renderer::MapRenderer& map_renderer, const json::Dict& dict);

	//------------------outnput-------------------------

	void MakeResponse(const request_handler::RequestHandler& request_handler, const json::Array& arr);

	const json::Dict GetStopInfo(const request_handler::RequestHandler& request_handler, const json::Dict& dict);

	const json::Dict GetBusInfo(const request_handler::RequestHandler& request_handler, const json::Dict& dict);

	const json::Dict GetMapRender(const request_handler::RequestHandler& request_handler, const json::Dict& dict);

} // json_reader 