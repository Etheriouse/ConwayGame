#include "utils.hpp"
#include "inipp.h"
#include <fstream>
#include <string>

struct Space::configuration load()
{
    struct Space::configuration config = {10, 10,  1600, 900, 50, 10};

    inipp::Ini<char> ini;
    std::ifstream file("config.ini");
    ini.parse(file);
	ini.strip_trailing_comments();
	ini.default_section(ini.sections["DEFAULT"]);
	ini.interpolate();

    inipp::get_value(ini.sections["game"], "width", config.width);
    inipp::get_value(ini.sections["game"], "height", config.height);
    inipp::get_value(ini.sections["game"], "maxfps", config.maxFps);
    inipp::get_value(ini.sections["game"], "tps", config.tps);
    inipp::get_value(ini.sections["window"], "width", config.screenWidth);
    inipp::get_value(ini.sections["window"], "height", config.screenHeight);
    return config;
}
