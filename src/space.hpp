#ifndef CONWAY_SPACE_HPP
#define CONWAY_SPACE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstdint>

#include <raylib.h>

#define OneBillion 1'000'000'000.0L

#define OK 0
#define ERROR -1

constexpr float TPS_FACTOR = 1.5f;
constexpr float ZOOM_FACTOR = 1.2f;
constexpr float MIN_TPS = 2.0f;
constexpr float MAX_TPS = 1'000'000.0f;

class Space
{
public:
    struct configuration
    {
        size_t width, height, screenWidth, screenHeight, maxFps, tps, speed;
        float cellSize, runFactor, minSizeCell, maxSizeCell;
        std::string alive, dead, backImage, colorImage;
        Texture2D back_i;
        Color back_c;
        bool showImage, fullscreen, borderless;
    };

private:
    enum Cell : uint8_t
    {
        Alive = 1,
        Dead = 0,
    };

    int start();
    void nextIteration();
    void reload();
    void clear();

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

    void render(Rectangle src, Vector2 v0, Texture2D alive_t, Texture2D dead_t);
    void process(long double delta);

    Cell *viewBuffer = nullptr;
    Cell *drawBuffer = nullptr;

    size_t TPS = 0, FPS = 0;

    Vector2 pos;
    size_t SPEED = 0;

    struct configuration config;

public:
    static int run()
    {
        return Space().start();
    }
};

#endif