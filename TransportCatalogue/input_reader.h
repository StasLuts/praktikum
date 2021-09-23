#pragma once

#include "transport_catalogue.h"
#include <string>
#include <vector>
#include <tuple>
#include <string_view>

std::string ReadLine();

int ReadLineWithNumber();

std::tuple<std::string_view, double, double> SplitForStop(std::string_view text);

std::tuple<std::string_view, bool, std::vector<std::string_view>> SplitForBus(std::string_view text);

std::vector<std::tuple<std::string_view, std::string_view, int>> SplitForDistance(std::string_view text);

void FillData(TransportCatalogue& catalog);
