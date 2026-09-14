#include "utils.hpp"
#include "inipp.h"
#include <fstream>
#include <string>

struct Space::configuration load()
{
    struct Space::configuration config; // = {10, 10,  1600, 900, 50, 10};

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
    inipp::get_value(ini.sections["game"], "speed", config.speed);
    inipp::get_value(ini.sections["game"], "runFactor", config.runFactor);
    inipp::get_value(ini.sections["cells"], "cellSize", config.cellSize);
    inipp::get_value(ini.sections["cells"], "minSize", config.minSizeCell);
    inipp::get_value(ini.sections["cells"], "maxSize", config.maxSizeCell);

    config.alive = ini.sections["textures"]["alive"];
    config.dead = ini.sections["textures"]["dead"];
    config.backImage = ini.sections["textures"]["backgroundImage"];
    config.colorImage = ini.sections["textures"]["backgroundColor"];
    inipp::get_value(ini.sections["textures"], "showImage", config.showImage);

    inipp::get_value(ini.sections["window"], "width", config.screenWidth);
    inipp::get_value(ini.sections["window"], "height", config.screenHeight);

    return config;
}
