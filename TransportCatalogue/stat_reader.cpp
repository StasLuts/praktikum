#include "stat_reader.h"
#include "input_reader.h"
#include <iomanip>
#include <iostream>

using namespace std;

string_view SplitForOutput(string_view text) {
    int64_t sp1 = text.find(' ', 0);
    string_view bus_num = text.substr(sp1 + 1, text.back());
    return bus_num;
}

void Print_Route_Info(TransportCatalogue& tc, string_view query)
{
    auto bus = tc.Get_Route(query);
    cout << "Bus " << query << ": ";
    (bus == nullptr) ?
        cout << "not found" << endl :
        cout << bus->stops_on_route_ << " stops on route, " << bus->unique_stops_ << " unique stops, "
        << setprecision(6) << bus->route_length_ << " route length, "
        << bus->curvature_  << " curvature" << endl;
}

void Print_Stop_Info(TransportCatalogue& tc, string_view query)
{
    auto stop = tc.Get_Stop_Info(query);
    cout << "Stop " << query << ": ";
    if (stop == nullptr)
    {
        cout << "not found" << endl;
    }
    else if (stop->buses_.empty())
    {
        cout << "no buses" << endl;
    }
    else
    {
        cout << "buses";
        for (const auto& bus : stop->buses_)
        {
            cout << ' ' << bus;
        }
        cout << endl;
    }
}

void FillOutputData(TransportCatalogue& catalog) {
    int n = ReadLineWithNumber();
    string text;
    for (int i = 0; i < n; i++) {
        text = ReadLine();
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


