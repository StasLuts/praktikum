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

	renderer::RenderSettings ReadRenderSettings(const json::Dict&);

	void SetMapRenderer(const transport_catalogue::TransportCatalogue&, renderer::MapRenderer&, const json::Dict&);

	//------------------outnput-------------------------

	void MakeResponse(transport_catalogue::TransportCatalogue&, const renderer::MapRenderer&, const json::Array&);

	const json::Dict GetStopInfo(const transport_catalogue::TransportCatalogue&, const json::Dict&);

	const json::Dict GetBusInfo(transport_catalogue::TransportCatalogue&, const json::Dict&);

	//const json::Dict GetMapRender(const transport_catalogue::TransportCatalogue&, map_renderer::MapRenderer&, const json::Dict&);

} // json_reader 