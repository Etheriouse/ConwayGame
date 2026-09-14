#ifndef CONWAY_SPACE_HPP
#define CONWAY_SPACE_HPP

#include <stdio.h>
#include <stdlib.h>

#include <cstdint>

#define OK 0
#define ERROR -1

class Space
{
    enum Cell : uint8_t
    {
        Alive = 1,
        Dead = 0,
    };

    struct configuration
    {
        size_t width, height, screenWidth, screenHeight, maxFps;
        float textureSize;
    };

    int start();
    void nextIteration();

    inline uint8_t countNeighbord(size_t x, size_t y)
    {
        const size_t i = y * config.width + x;
        const size_t w = config.width;

        return viewBuffer[i - w - 1] +
               viewBuffer[i - w] +
               viewBuffer[i - w + 1] +
               viewBuffer[i - 1] +
               viewBuffer[i + 1] +
               viewBuffer[i + w - 1] +
               viewBuffer[i + w] +
               viewBuffer[i + w + 1];
    }

    Cell *viewBuffer = nullptr;
    Cell *drawBuffer = nullptr;

    struct configuration config;

public:
    static int run()
    {
        return Space().start();
    }
};

#endif