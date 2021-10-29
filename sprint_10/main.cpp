#include "map_renderer.h"
#include "json_reader.h"
#include "request_handler.h"

#include <iostream>

using namespace std::literals;

int main()
{
    json_reader::JsonRead(std::cin);
    return 0;
}