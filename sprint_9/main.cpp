#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

int main()
{
	transport_catalogue::TransportCatalogue transport_catalogue;
	transport_catalogue::input::FillData(transport_catalogue, std::cin);
	transport_catalogue::output::OutputData(transport_catalogue, std::cout);
	return 0;
}