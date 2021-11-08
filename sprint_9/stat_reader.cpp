#include "stat_reader.h"
#include "input_reader.h"
#include <iomanip>
#include <iostream>

namespace transport_catalogue
{
    namespace output
    {
        std::string_view SplitForOutput(const std::string_view& text)
        {
            int64_t sp1 = text.find(' ', 0);
            std::string_view bus_num = text.substr(sp1 + 1, text.back());
            return bus_num;
        }

        void PrintRouteInfo(const TransportCatalogue& tc, const std::string_view& query, std::ostream& out)
        {
            auto bus = tc.GetBusStat(query);
            out << "Bus " << query << ": ";
            (bus == nullptr) ?
                out << "not found" << std::endl :
                out << bus->stops_on_route_ << " stops on route, " << bus->unique_stops_ << " unique stops, "
                << std::setprecision(6) << bus->route_length_ << " route length, "
                << bus->curvature_ << " curvature" << std::endl;
        }

        void PrintStopInfo(const TransportCatalogue& tc, const std::string_view& query, std::ostream& out)
        {
            auto stop = tc.GetStopStat(query);
            out << "Stop " << query << ": ";
            if (stop == nullptr)
            {
                out << "not found" << std::endl;
            }
            else if (stop->buses_.empty())
            {
                out << "no buses" << std::endl;
            }
            else
            {
                out << "buses";
                for (const auto& bus : stop->buses_)
                {
                    out << ' ' << bus;
                }
                out << std::endl;
            }
        }

        void OutputData(const TransportCatalogue& catalog, std::ostream& out)
        {
            int n = transport_catalogue::input::ReadLineWithNumber();
            std::string text;
            for (int i = 0; i < n; i++) {
                text = transport_catalogue::input::ReadLine(std::cin);
                if (text[0] == 'B') {
                    auto bus_num = SplitForOutput(text);
                    PrintRouteInfo(catalog, bus_num, out);
                }
                if (text[0] == 'S') {
                    auto stop_name = SplitForOutput(text);
                    PrintStopInfo(catalog, stop_name, out);
                }
            }
        }
    } // namespace output
} // namespace transport_catalogue


