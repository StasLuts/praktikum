#pragma once

#include "transport_catalogue.h"
#include <string_view>

std::string_view SplitForOutput(std::string_view text);

void Print_Route_Info(TransportCatalogue& tc, std::string_view query);

void Print_Stop_Info(TransportCatalogue& tc, std::string_view query);

void FillOutputData(TransportCatalogue& catalog);
