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

    reload();

    InitWindow(config.screenWidth, config.screenHeight, "Conway Game");
    SetTargetFPS(config.maxFps);

    bool stop = false, paused = false, mousePointer = false;

    std::stringstream s;
    s << "assets/" << config.alive;
    Texture2D alive_t = LoadTexture(s.str().c_str());
    s.str("");
    s << "assets/" << config.dead;
    Texture2D dead_t = LoadTexture(s.str().c_str());

    s.str("");
    s << "assets/" << config.backImage;
    config.back_i = LoadTexture(s.str().c_str());

    config.back_c = GetColor(TextToInteger(config.colorImage.c_str()));

    const float textureSize = 32.0f;

    Vector2 v0 = {0, 0}, centerPos = {(float)config.screenWidth / 2, (float)config.screenHeight / 2}, mousePos;
    Rectangle src = {0, 0, textureSize, textureSize};

    long double delta = 0, acc_s = 0, acc_ds = 0, acc_p = 0, TICK_TIME = 1.0L / config.tps;
    size_t tick, tick_s, fps, tmp = 0, last = Time::getNanoS();

    while (!stop)
    {
        tmp = Time::getNanoS();
        delta = static_cast<long double>(tmp - last) / OneBillion;
        last = tmp;

        if (delta > 0.25L)
            delta = 0.25L;

        if (!paused)
            acc_p += delta;

        while (acc_p >= TICK_TIME && !paused)
        {
            process(TICK_TIME);
            acc_p -= TICK_TIME;

            tick++;
            tick_s++;
        }

        mousePos = GetMousePosition();
        float scrollValue = GetMouseWheelMove();

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            Vector2 mDelta = GetMouseDelta();
            pos.x += mDelta.x;
            pos.y += mDelta.y;
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

        Vector2 mouse = mousePointer ? mousePos : centerPos;
        if (scrollValue > 0)
        {
            float oldSize = config.cellSize;
            config.cellSize = std::min(config.cellSize * ZOOM_FACTOR, config.maxSizeCell);
            float ratio = config.cellSize / oldSize;

            pos.x = mouse.x - (mouse.x - pos.x) * ratio;
            pos.y = mouse.y - (mouse.y - pos.y) * ratio;
        }

        else if (scrollValue < 0)
        {
            float oldSize = config.cellSize;
            config.cellSize = std::max(config.cellSize / ZOOM_FACTOR, config.minSizeCell);
            float ratio = config.cellSize / oldSize;

            pos.x = mouse.x - (mouse.x - pos.x) * ratio;
            pos.y = mouse.y - (mouse.y - pos.y) * ratio;
        }

        if (IsKeyPressed(KEY_SPACE))
            paused = !paused;

        if (IsKeyDown(KEY_LEFT_CONTROL))
            SPEED = config.speed * config.runFactor;
        else
            SPEED = config.speed;

        if (IsKeyPressed(KEY_DOWN))
        {
            config.tps = std::max(config.tps / TPS_FACTOR, MIN_TPS);
            TICK_TIME = 1.0L / config.tps;
        }
        if (IsKeyPressed(KEY_UP))
        {
            config.tps = std::min(config.tps * TPS_FACTOR, MAX_TPS);
            TICK_TIME = 1.0L / config.tps;
        }

        if (IsKeyPressed(KEY_R))
        {
            reload();
        }

        if (IsKeyPressed(KEY_C) && IsKeyDown(KEY_LEFT_CONTROL))
        {
            clear();
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 mouse = GetMousePosition();

            int gridX = (int)((mouse.x - pos.x) / config.cellSize);
            int gridY = (int)((mouse.y - pos.y) / config.cellSize);
            if (gridX >= 0 && gridX < config.width &&
                gridY >= 0 && gridY < config.height)
            {
                size_t index = gridY * config.width + gridX;
                viewBuffer[index] = IsKeyDown(KEY_LEFT_SHIFT) ? Cell::Dead : Cell::Alive;
            }
        }

        if (IsWindowResized())
        {
            config.screenHeight = GetScreenHeight();
            config.screenWidth = GetScreenWidth();
            centerPos = {(float)config.screenWidth / 2, (float)config.screenHeight / 2};
        }

        render(src, v0, alive_t, dead_t);
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

void Space::reload()
{
    for (size_t y = 1; y < config.height - 1; y++)
        for (size_t x = 1; x < config.width - 1; x++)
            viewBuffer[y * config.height + x] = rand() % 4 == 0 ? Cell::Alive : Cell::Dead;
}

void Space::clear()
{
    for (size_t n = 1; n < config.height * config.width; n++)
        viewBuffer[n] = Cell::Dead;
}

void Space::render(Rectangle src, Vector2 v0, Texture2D alive_t, Texture2D dead_t)
{
    BeginDrawing();

    ClearBackground(config.back_c);
    if (config.showImage)
    {
        DrawTexturePro(config.back_i, {0, 0, (float)config.back_i.width, (float)config.back_i.height},
                       {0, 0, (float)config.screenWidth, (float)config.screenHeight}, {0, 0}, 0, WHITE);
    }

    size_t h;
    for (size_t y = 0; y < config.height; y++)
    {
        h = y * config.width;
        for (size_t x = 0; x < config.width; x++)
        {
            Rectangle tile = {(float)(config.cellSize * x) + pos.x, (float)(config.cellSize * y) + pos.y, config.cellSize, config.cellSize};
            if (tile.x > config.screenWidth || tile.y > config.screenHeight || tile.x + config.cellSize < 0 || tile.y + config.cellSize < 0)
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
