#include "json_reader.h"
#include "json_builder.h"

#include <iostream>

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        PrintUsage();
        return 1;
    }
    const std::string_view mode(argv[1]);
    if (mode == "make_base"sv)
    {
        json_reader::JsonSerialize(std::cin);
    }
    else if (mode == "process_requests"sv)
    {
        json_reader::JsonDeserialize(std::cin, std::cout);
    }
    else
    {
        PrintUsage();
        return 1;
    }
    return 0;
}