#pragma once

#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "request_handler.h"

namespace json_reader
{
	void JsonRead(std::istream&);

	//------------------input-------------------------

	void MakeBase(transport_catalogue::TransportCatalogue&, const json::Array&);

	void ReadStopData(transport_catalogue::TransportCatalogue&, const json::Dict&);

	void ReadStopDistance(transport_catalogue::TransportCatalogue&, const json::Dict&);

	void ReadBusData(transport_catalogue::TransportCatalogue&, const json::Dict&);

	//------------------render-------------------------

	const svg::Color GetColor(const json::Node&);

	void SetMapRenderer(const transport_catalogue::TransportCatalogue&, renderer::MapRenderer&, const json::Dict&);

	//------------------outnput-------------------------

	void MakeResponse(const request_handler::RequestHandler&, const json::Array&);

	const json::Dict GetStopInfo(const request_handler::RequestHandler&, const json::Dict&);

	const json::Dict GetBusInfo(const request_handler::RequestHandler&, const json::Dict&);

	const json::Dict GetMapRender(const request_handler::RequestHandler&, const json::Dict& dict);

} // json_reader 