#include "space.hpp"
#include <raylib.h>

#include <time.h>

int Space::start()
{
    config = {50, 50, 1980, 1080, 60, 32.0f};
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

    Vector2 v0 = {0, 0};
    Rectangle src = {0, 0, config.textureSize, config.textureSize};

    while (!stop)
    {
        BeginDrawing();

        ClearBackground(BLACK);

        size_t h;
        for (size_t y = 0; y < config.height; y++)
        {
            h = y * config.width;
            for (size_t x = 0; x < config.width; x++)
            {
                Rectangle cellTile = {(float)cellSize * x, (float)cellSize * y, (float)cellSize, (float)cellSize};
                DrawTexturePro(viewBuffer[h + x] == Cell::Alive ? alive_t : dead_t,
                               src, cellTile, v0, 0, WHITE);
            }
        }

        EndDrawing();
        nextIteration();
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
    size_t h;
    Cell result;
    for (size_t y = 1; y < config.height - 1; y++)
    {
        h = y * config.width;
        for (size_t x = 1; x < config.width - 1; x++)
        {
            // noob optimization for counting neighbord
            uint8_t neigh = countNeighbord(x, y);
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
