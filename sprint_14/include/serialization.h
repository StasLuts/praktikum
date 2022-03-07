#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "svg.h"
#include "transport_router.h"
#include "graph.h"
#include "transport_catalogue.pb.h"

#include <fstream>
#include <vector>

namespace serialize
{
	class Serializer
	{
	public:
		
		Serializer(const transport_catalogue::TransportCatalogue& trans_cat, const renderer::MapRenderer& map_renderer, const transport_router::TransportRouter& trans_rote)
			: trans_cat_(trans_cat), map_renderer_(map_renderer), trans_rote_(trans_rote) {}

		void Serialize(const std::string& filename);

	private:

		const transport_catalogue::TransportCatalogue& trans_cat_;
		const renderer::MapRenderer& map_renderer_;
		const transport_router::TransportRouter& trans_rote_;
		transport_catalogue_serialize::TransportCatalogue trans_cat_ser_;

		void SerializeStop(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser);
		void SerializeBus(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser);
		void SerializeDistance(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser);
		void SerealizeRenderSettings(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser);
		transport_catalogue_serialize::Color SerealizeColor(const svg::Color& color);
		void SerealizeRoutingSettings(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser);
	};

	class Deserializer
	{
	public:

		void DeserializeCatalogAndRenderer(transport_catalogue::TransportCatalogue& trans_cat, renderer::MapRenderer& map_renderer, const std::string& filename);
		void DeserealizeRouter(transport_router::TransportRouter& trans_rote, const std::string& filename);

	private:

		std::pair<std::pair<domain::StopPtr, domain::StopPtr>, int> DeserializeDistance(const transport_catalogue_serialize::Distance& distance_ser, const transport_catalogue::TransportCatalogue& trans_cat);
		renderer::RenderSettings DeserealizeRenderSettings(const transport_catalogue_serialize::RenderSettings& render_settings_ser);
		svg::Color DserealizeColor(const transport_catalogue_serialize::Color& color_ser);
	};

} // namespace serialize