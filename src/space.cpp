#include "space.hpp"

#include <omp.h>

#include <sstream>
#include <time.h>

#include "time.hpp"
#include "utils.hpp"

int Space::start()
{
    config = load();
    // read config file

    srand(time(NULL));

    viewBuffer = (Cell *)(malloc(sizeof(Cell) * config.width * config.height));
    drawBuffer = (Cell *)(malloc(sizeof(Cell) * config.width * config.height));

    for (size_t y = 1; y < config.height - 1; y++)
        for (size_t x = 1; x < config.width - 1; x++)
            viewBuffer[y * config.height + x] = rand() % 4 == 0 ? Cell::Alive : Cell::Dead;

    InitWindow(config.screenWidth, config.screenHeight, "Conway Game");
    SetTargetFPS(config.maxFps);

    bool stop = false;

    Texture2D alive_t = LoadTexture("assets/alive.png");
    Texture2D dead_t = LoadTexture("assets/dead.png");

    size_t cellSize = 20;

    float textureSize = 32.0f;

    Vector2 v0 = {0, 0};
    Rectangle src = {0, 0, textureSize, textureSize};

    long double delta = 0, acc_s = 0, acc_ds = 0, acc_p = 0;
    size_t tick, tick_s, fps, tmp = 0, last = Time::getNanoS();
    ;

    while (!stop)
    {
        tmp = Time::getNanoS();
        delta = static_cast<long double>(tmp - last) / OneBillion;
        last = tmp;

        if (delta > 0.25L)
            delta = 0.25L;

        acc_p += delta;

        while (acc_p >= TICK_TIME)
        {
            process(TICK_TIME);
            acc_p -= TICK_TIME;

            tick++;
            tick_s++;
        }

        Vector2 direction = {0, 0};
        if (IsKeyDown(KEY_W))
            direction.y = 1;
        if (IsKeyDown(KEY_S))
            direction.y = -1;
        if (IsKeyDown(KEY_A))
            direction.x = 1;
        if (IsKeyDown(KEY_D))
            direction.x = -1;

        pos.x += direction.x * SPEED * delta;
        pos.y += direction.y * SPEED * delta;

        if (IsWindowResized())
        {
            config.screenHeight = GetScreenHeight();
            config.screenWidth = GetScreenWidth();
        }

        render(src, cellSize, v0, alive_t, dead_t);
        fps++;
        acc_s += delta;
        if (acc_s >= 1.0L)
        {
            TPS = tick_s;
            tick_s = 0;
            acc_s -= 1.0L;
        }

        acc_ds += delta;
        if (acc_ds >= 1.0L)
        {
            FPS = fps;
            fps = 0;
            acc_ds -= 1.0L;
        }
        stop = WindowShouldClose();
    }

    UnloadTexture(alive_t);
    UnloadTexture(dead_t);

    CloseWindow();

    if (viewBuffer != nullptr)
        free(viewBuffer);
    if (drawBuffer != nullptr)
        free(drawBuffer);

    return OK;
}

/**
 * Take state of viewBuffer and apply rule to drawBuffer
 */
void Space::nextIteration()
{
#pragma omp parallel for
    for (size_t y = 1; y < config.height - 1; y++)
    {
        const size_t h = y * config.width;
        for (size_t x = 1; x < config.width - 1; x++)
        {
            // noob optimization for counting neighbord
            uint8_t neigh = countNeighbord(x, y);
            Cell result;
            if (viewBuffer[h + x] == Cell::Alive)
                result = neigh == 2 || neigh == 3 ? Cell::Alive : Cell::Dead;
            else
                result = neigh == 3 ? Cell::Alive : Cell::Dead;

            drawBuffer[h + x] = result;
        }
    }

    // Swap buffer ptr
    Cell *tmp = viewBuffer;
    viewBuffer = drawBuffer;
    drawBuffer = tmp;
}

void Space::render(Rectangle src, size_t cellSize, Vector2 v0, Texture2D alive_t, Texture2D dead_t)
{
    BeginDrawing();

    ClearBackground(BLACK);

    size_t h;
    for (size_t y = 0; y < config.height; y++)
    {
        h = y * config.width;
        for (size_t x = 0; x < config.width; x++)
        {
            Rectangle tile = {(float)(cellSize * x) + pos.x, (float)(cellSize * y) + pos.y, (float)cellSize, (float)cellSize};
            if (tile.x > config.screenWidth || tile.y > config.screenHeight || tile.x+cellSize < 0 || tile.y+cellSize < 0)
                continue;
            DrawTexturePro(viewBuffer[h + x] == Cell::Alive ? alive_t : dead_t,
                           src, tile, v0, 0, WHITE);
        }
    }

    std::stringstream strm;
    strm << "FPS : " << FPS << "\nTPS : " << TPS;
    DrawText(strm.str().c_str(), 0, 0, 20, WHITE);

    EndDrawing();
}

void Space::process(long double delta)
{
    nextIteration();
}
