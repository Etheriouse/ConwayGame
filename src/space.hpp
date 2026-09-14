#ifndef CONWAY_SPACE_HPP
#define CONWAY_SPACE_HPP

#include <stdio.h>
#include <stdlib.h>

#include <cstdint>

#include <raylib.h>

#define _TPS 5.0L
#define TICK_TIME 1.0L / _TPS
#define OneBillion 1'000'000'000.0L

#define OK 0
#define ERROR -1

class Space
{
public:
    struct configuration
    {
        size_t width, height, screenWidth, screenHeight, maxFps, tps;
    };

private:
    enum Cell : uint8_t
    {
        Alive = 1,
        Dead = 0,
    };

    int start();
    void nextIteration();

    // possiblement transformer ca en shaders et faire le calcule sur le gpu
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

    void render(Rectangle src, size_t cellSize, Vector2 v0, Texture2D alive_t, Texture2D dead_t);
    void process(long double delta);

    Cell *viewBuffer = nullptr;
    Cell *drawBuffer = nullptr;

    size_t TPS = 0, FPS = 0;

    Vector2 pos;
    size_t SPEED = 200;

    struct configuration config;

public:
    static int run()
    {
        return Space().start();
    }
};

#endif