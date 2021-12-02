#include "map_renderer.h"
#include "json_reader.h"
#include "request_handler.h"

#include <iostream>
#include <fstream>

using namespace std::literals;

int main()
{
    //std::ifstream in;
    //in.open("C:\\Users\\User\\Downloads\\s10_final_opentest\\s10_final_opentest_1.json");
    json_reader::JsonRead(std::cin);
    return 0;
}