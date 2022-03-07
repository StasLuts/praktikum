#include "json_reader.h"
#include "json_builder.h"

#include <iostream>

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main()
{
    json_reader::JsonSerialize(std::cin);
    json_reader::JsonDeserialize(std::cin, std::cout);
    return 0;
}