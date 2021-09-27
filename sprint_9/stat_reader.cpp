#include "stat_reader.h"
#include "input_reader.h"
#include <iomanip>
#include <iostream>

std::string_view transport_catalogue::output::SplitForOutput(std::string_view text)
{
    int64_t sp1 = text.find(' ', 0);
    std::string_view bus_num = text.substr(sp1 + 1, text.back());
    return bus_num;
}

void transport_catalogue::output::Print_Route_Info(TransportCatalogue& tc, std::string_view query)
{
    auto bus = tc.Get_Route(query);
    std::cout << "Bus " << query << ": ";
    (bus == nullptr) ?
        std::cout << "not found" << std::endl :
        std::cout << bus->stops_on_route_ << " stops on route, " << bus->unique_stops_ << " unique stops, "
        << std::setprecision(6) << bus->route_length_ << " route length, "
        << bus->curvature_  << " curvature" << std::endl;
}

void transport_catalogue::output::Print_Stop_Info(TransportCatalogue& tc, std::string_view query)
{
    auto stop = tc.Get_Stop_Info(query);
    std::cout << "Stop " << query << ": ";
    if (stop == nullptr)
    {
        std::cout << "not found" << std::endl;
    }
    else if (stop->buses_.empty())
    {
        std::cout << "no buses" << std::endl;
    }
    else
    {
        std::cout << "buses";
        for (const auto& bus : stop->buses_)
        {
            std::cout << ' ' << bus;
        }
        std::cout << std::endl;
    }
}

void transport_catalogue::output::FillOutputData(TransportCatalogue& catalog) {
    int n = transport_catalogue::input::ReadLineWithNumber();
    std::string text;
    for (int i = 0; i < n; i++) {
        text = transport_catalogue::input::ReadLine();
        if (text[0] == 'B') {
            auto bus_num = SplitForOutput(text);
            Print_Route_Info(catalog, bus_num);
        }
        if (text[0] == 'S') {
            auto stop_name = SplitForOutput(text);
            Print_Stop_Info(catalog, stop_name);
        }
    }
}


