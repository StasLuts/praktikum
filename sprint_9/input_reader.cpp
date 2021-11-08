#include "input_reader.h"
#include <cmath>
#include <utility>

namespace transport_catalogue
{
    namespace input
    {
        std::string ReadLine(std::istream& in)
        {
            std::string s;
            std::getline(in, s);
            return s;
        }

        int ReadLineWithNumber()
        {
            int result;
            std::cin >> result;
            ReadLine(std::cin);
            return result;
        }

        Stop SplitForStop(std::string_view text)
        {
            int64_t sp1 = text.find(' ', 0);
            int64_t dobdot = text.find(':', sp1 + 1);
            int64_t fcom = text.find(',', dobdot + 2);
            std::string_view name = text.substr(sp1 + 1, dobdot - sp1 - 1);
            double lat = std::stod(std::string(text.substr(dobdot + 2, fcom - dobdot - 2)));
            double lng = std::stod(std::string(text.substr(fcom + 2)));
            return Stop{ name, lat, lng };
        }

        std::vector<StopsDictance> SplitForDistance(std::string_view text)
        {
            std::vector<StopsDictance> result;
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
                result.emplace_back(StopsDictance{ name1, name2, distance });
                if (sp == data.npos)
                {
                    break;
                }
                data = (data.substr(sp + 2, data.length()));
            }
            return result;
        }

        Bus SplitForBus(std::string_view text)
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

        void FillData(TransportCatalogue& catalog, std::istream& in) {
            int n = ReadLineWithNumber();
            std::vector<std::string> distance_data;
            std::vector<std::string> bus_data;
            std::string text;
            distance_data.reserve(n);
            bus_data.reserve(n);
            for (int i = 0; i < n; i++) {
                text = ReadLine(in);
                if (text[0] == 'S') {
                    distance_data.push_back(text);
                    auto [name, lat, lng] = SplitForStop(text);
                    catalog.AddStopDatabase(name, lat, lng);
                }
                else {
                    bus_data.push_back(move(text));
                }
            }
            for (auto text : distance_data)
            {
                for (auto [stop1, stop2, distance] : SplitForDistance(text))
                {
                    catalog.SetDistanceBetweenStops(stop1, stop2, distance);
                }
            }
            for (auto text : bus_data) {
                auto [name, cicle, stops] = SplitForBus(text);
                catalog.AddBusDatabase(name, stops, cicle);
            }
        }
} // namespace input
} // namespace transport_catalogue