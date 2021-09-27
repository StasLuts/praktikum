#include "input_reader.h"
#include <cmath>
#include <iostream>
#include <utility>

std::string transport_catalogue::input::ReadLine()
{
    std::string s;
    std::getline(std::cin, s);
    return s;
}

int transport_catalogue::input::ReadLineWithNumber()
{
    int result;
    std::cin >> result;
    ReadLine();
    return result;
}

std::tuple<std::string_view, double, double> transport_catalogue::input::SplitForStop(std::string_view text)
{
    int64_t sp1 = text.find(' ', 0);
    int64_t dobdot = text.find(':', sp1 + 1);
    int64_t fcom = text.find(',', dobdot + 2);
    std::string_view name = text.substr(sp1 + 1, dobdot - sp1 - 1);
    double lat = std::stod(std::string(text.substr(dobdot + 2, fcom - dobdot - 2)));
    double lng = std::stod(std::string(text.substr(fcom + 2)));
    return { name, lat, lng };
}

std::vector<std::tuple<std::string_view, std::string_view, int>> transport_catalogue::input::SplitForDistance(std::string_view text)
{
    std::vector<std::tuple<std::string_view, std::string_view, int>> result;
    int64_t sp1 = text.find(' ', 0);
    int64_t dobdot = text.find(':', sp1 + 1);
    int64_t fcom = text.find(',', dobdot + 2);
    std::string_view name1 = text.substr(sp1 + 1, dobdot - sp1 - 1);
    int64_t space = text.find(',', fcom + 1);
    if (space == text.npos)
    {
        return result;
    }
    std::string_view data = text.substr(space + 2, text.length());
    while (true)
    {
        int64_t sp = data.find('m', 0);
        int distance = abs(std::stoi(std::string(data.substr(0, sp))));
        sp = data.find(' ');
        data = (data.substr(sp + 1, data.length()));
        sp = data.find(' ');
        data = (data.substr(sp + 1, data.length()));
        sp = data.find(',');
        std::string_view name2 = data.substr(0, sp);
        result.emplace_back(make_tuple( name1, name2, distance ));
        if (sp == data.npos)
        {
            break;
        }
        data = (data.substr(sp + 2, data.length()));
    }
    return result;
}

std::tuple<std::string_view, bool, std::vector<std::string_view>> transport_catalogue::input::SplitForBus(std::string_view text)
{
    int64_t sp1 = text.find(' ', 0);
    int64_t dobdot = text.find(':', sp1 + 1);
    std::string_view name = text.substr(sp1 + 1, dobdot - sp1 - 1);
    std::string delim = " - ";
    bool cicle = false;
    int64_t fdel = text.find(delim, dobdot + 2);
    if (fdel == text.npos) {
        cicle = true;
        delim = " > ";
    }
    fdel = text.find(delim, dobdot + 2);
    std::vector<std::string_view> stops;
    stops.push_back(text.substr(dobdot + 2, fdel - dobdot - 2));
    int64_t sdel = text.find(delim, fdel + 3);
    while (sdel != text.npos) {
        stops.push_back(text.substr(fdel + 3, sdel - fdel - 3));
        fdel = sdel;
        sdel = text.find(delim, fdel + 3);
    }
    stops.push_back(text.substr(fdel + 3));
    return { name,cicle,stops };
}

void transport_catalogue::input::FillData(TransportCatalogue& catalog) {
    int n = ReadLineWithNumber();
    std::vector<std::string> distance_data;
    std::vector<std::string> bus_data;
    std::string text;
    distance_data.reserve(n);
    bus_data.reserve(n);
    for (int i = 0; i < n; i++) {
        text = ReadLine();
        if (text[0] == 'S') {
            distance_data.push_back(text);
            auto [name, lat, lng] = SplitForStop(text);
            catalog.Adding_Stop_Database(name, lat, lng);
        }
        else {
            bus_data.push_back(move(text));
        }
    }
    for (auto text : distance_data)
    {
        for (auto [stop1, stop2, distance] : SplitForDistance(text))
        {
            catalog.Set_Distance_Between_Stops(stop1, stop2, distance);
        }
    }
    for (auto text : bus_data) {
        auto [name, cicle, stops] = SplitForBus(text);
        catalog.Adding_Bus_Database(name, stops, cicle);
    }
}