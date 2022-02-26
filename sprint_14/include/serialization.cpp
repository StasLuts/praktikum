#include "serialization.h"

namespace serialize
{
	// Serialize

	void Serializer::Serialize(const std::string& filename)
	{
		std::ofstream out(filename, std::ios::binary);
		SerializeStop(trans_cat_ser_);
		//SerializeDistance(trans_cat_ser_);
		SerializeBus(trans_cat_ser_);
		trans_cat_ser_.SerializeToOstream(&out);
	}

	// Serialize private

	void Serializer::SerializeStop(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser)
	{
		for (const auto& stop : trans_cat_.GetAllStops())
		{
			transport_catalogue_serialize::Stop buf_stop;
			transport_catalogue_serialize::Coordinates buf_coordinates;
			buf_coordinates.set_lat(stop->coodinates.lat);
			buf_coordinates.set_lng(stop->coodinates.lng);
			buf_stop.set_name(stop->stop_name);
			*buf_stop.mutable_coordinates() = buf_coordinates;
			*trans_cat_ser.add_stops() = buf_stop;
		}
	}

	void Serializer::SerializeBus(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser)
	{
		for (const auto& bus : trans_cat_.GetBuses())
		{
			transport_catalogue_serialize::Bus buf_bus;
			buf_bus.set_unique_stops(bus->unique_stops.size());
			buf_bus.set_stops_number(bus->stops.size());
			buf_bus.set_number(bus->bus_num);
			buf_bus.set_is_round_trip(bus->is_circular);
			buf_bus.set_distance(trans_cat_.GetBusStat(bus->bus_num)->route_length);

			//raw

			for (auto stop : bus->unique_stops)
			{
				transport_catalogue_serialize::Stop buf_stop;
				transport_catalogue_serialize::Coordinates buf_coordinates;
				buf_coordinates.set_lat(stop->coodinates.lat);
				buf_coordinates.set_lng(stop->coodinates.lng);
				buf_stop.set_name(stop->stop_name);
				*buf_stop.mutable_coordinates() = buf_coordinates;
				*buf_bus.add_route() = buf_stop;
			}
			*trans_cat_ser.add_buses() = buf_bus;
		}
	}

	//Deserializer

	Deserializer::Deserializer(transport_catalogue::TransportCatalogue& trans_cat, renderer::MapRenderer& map_renderer, transport_router::TransportRouter& router, const std::string& filename)
	{
		std::ifstream in(filename, std::ios::binary);
		transport_catalogue_serialize::TransportCatalogue trans_cat_ser;
		trans_cat_ser.ParseFromIstream(&in);
		for (int i = 0; i < trans_cat_ser.stops().size(); ++i)
		{

		}

		for (int i = 0; i < trans_cat_ser.buses().size(); ++i)
		{

		}

		for (int i = 0; i < trans_cat_ser.distances_size(); ++i)
		{

		}
	}

	//Deserializer private

	domain::Stop Deserializer::DeserializeStop(const transport_catalogue_serialize::Stop& stop_ser)
	{
		return domain::Stop();
	}

	domain::Bus Deserializer::DeserializeBus(const transport_catalogue_serialize::Bus& bus_ser)
	{
		return domain::Bus();
	}

} // namespace serialize