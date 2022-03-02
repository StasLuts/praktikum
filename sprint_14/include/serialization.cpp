#include "serialization.h"

namespace serialize
{
	// Serialize

	void Serializer::Serialize(const std::string& filename)
	{
		std::ofstream out(filename, std::ios::binary);
		SerializeStop(trans_cat_ser_);
		SerializeDistance(trans_cat_ser_);
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
			buf_bus.set_bus_num(bus->bus_num);
			buf_bus.set_is_circular(bus->is_circular);
			for (auto stop : bus->stops)
			{
				transport_catalogue_serialize::Stop buf_stop;
				transport_catalogue_serialize::Coordinates buf_coordinates;
				buf_coordinates.set_lat(stop->coodinates.lat);
				buf_coordinates.set_lng(stop->coodinates.lng);
				buf_stop.set_name(stop->stop_name);
				*buf_stop.mutable_coordinates() = buf_coordinates;
				*buf_bus.add_stops() = buf_stop;
			}
			*trans_cat_ser.add_buses() = buf_bus;
		}
	}

	void Serializer::SerializeDistance(transport_catalogue_serialize::TransportCatalogue& trans_cat_ser)
	{
		for (const auto& distance : trans_cat_.GetAllDistances())
		{
			transport_catalogue_serialize::Distance buf_distance;
			buf_distance.set_from(distance.first.first->stop_name);
			buf_distance.set_to(distance.first.second->stop_name);
			buf_distance.set_distance(distance.second);
			*trans_cat_ser.add_distances() = buf_distance;
		}
	}

	//Deserializer

	void Deserializer::Deserialize(transport_catalogue::TransportCatalogue& trans_cat, renderer::MapRenderer& map_renderer, transport_router::TransportRouter& router, const std::string& filename)
	{
		std::ifstream in(filename, std::ios::binary);
		transport_catalogue_serialize::TransportCatalogue trans_cat_ser;
		trans_cat_ser.ParseFromIstream(&in);
		for (int i = 0; i < trans_cat_ser.stops().size(); ++i)
		{
			auto stop_ser = trans_cat_ser.stops(i);
			trans_cat.AddStopDatabase(stop_ser.name(), stop_ser.coordinates().lat(), stop_ser.coordinates().lng());
		}
		for (int i = 0; i < trans_cat_ser.buses().size(); ++i)
		{
			auto bus_ser = trans_cat_ser.buses(i);
			std::vector<std::string_view> stops;
			for (const auto& stop_ser : bus_ser.stops())
			{
				stops.push_back(stop_ser.name());
			}
			trans_cat.AddBusDatabase(bus_ser.bus_num(), stops, bus_ser.is_circular());
		}
		for (int i = 0; i < trans_cat_ser.distances_size(); ++i)
		{
			const auto deserialized_distance = DeserializeDistance(trans_cat_ser.distances(i), trans_cat);
			trans_cat.SetDistanceBetweenStops(deserialized_distance.first.first->stop_name, deserialized_distance.first.second->stop_name, deserialized_distance.second);
		}
	}

	//Deserializer private

	std::pair<std::pair<domain::StopPtr, domain::StopPtr>, int> Deserializer::DeserializeDistance(const transport_catalogue_serialize::Distance& distance_ser, const transport_catalogue::TransportCatalogue& trans_cat)
	{
		domain::StopPtr stop_from = trans_cat.FindStop(distance_ser.from());
		domain::StopPtr stop_to = trans_cat.FindStop(distance_ser.to());
		int64_t distance = distance_ser.distance();
		return std::make_pair(std::make_pair(stop_from, stop_to), distance);
	}
} // namespace serialize