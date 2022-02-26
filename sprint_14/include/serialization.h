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
	};

	class Deserializer
	{
	public:
		Deserializer();
		~Deserializer();

	private:

	};

} // namespace serialize