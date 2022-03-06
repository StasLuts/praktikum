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
	const int METERS_TO_KM = 1'000;
	const int HOURS_TO_MINUTES = 60;

	class Serializer
	{
	public:
		
		Serializer(const transport_catalogue::TransportCatalogue& trans_cat, const renderer::MapRenderer& map_renderer)
			: trans_cat_(trans_cat), map_renderer_(map_renderer) {}

		void Serialize(const std::string& filename);

	private:

		const transport_catalogue::TransportCatalogue& trans_cat_;
		const renderer::MapRenderer& map_renderer_;
		transport_catalogue_serialize::TransportCatalogue trans_cat_ser_;

		/*std::unordered_map<domain::StopPtr, int> stops_to_vertex_ids_;
		transport_catalogue_serialize::Router buf_router_;
		transport_catalogue_serialize::Graph buf_graph_;
		double bus_wait_time_ = 0.0;
		double bus_velocity_ = 0.0;*/

		void SerializeStop(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser);
		void SerializeBus(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser);
		void SerializeDistance(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser);
		void SerealizeRenderSettings(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser);
		transport_catalogue_serialize::Color SerealizeColor(const svg::Color& color);
	};

	class Deserializer
	{
	public:

		void Deserialize(transport_catalogue::TransportCatalogue& trans_cat, renderer::MapRenderer& map_renderer, transport_router::TransportRouter& router, const std::string& filename);

	private:

		std::pair<std::pair<domain::StopPtr, domain::StopPtr>, int> DeserializeDistance(const transport_catalogue_serialize::Distance& distance_ser, const transport_catalogue::TransportCatalogue& trans_cat);
		renderer::RenderSettings DeserealizeRenderSettings(const transport_catalogue_serialize::RenderSettings& render_settings_ser);
		svg::Color DserealizeColor(const transport_catalogue_serialize::Color& color_ser);
	};

} // namespace serialize