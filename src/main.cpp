#include <SDL2/SDL.h>
#include <iostream>
using namespace std;

void drawSand(int size, int x, int y, SDL_Renderer* renderer, SDL_Color color);

int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    const int width = 5; // size of each sand grain
    const int dimensionX = 900;
    const int dimensionY = 600;
    const int gridX = dimensionX / width;
    const int gridY = dimensionY / width;

    int matrix[gridX][gridY] = {0};

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(dimensionX, dimensionY, 0, &window, &renderer);

    bool running = true;
    SDL_Event event;

    while (running) {
        // --- Handle events ---
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            int cellX = mouseX / width;
            int cellY = mouseY / width;
            if (cellX >= 0 && cellX < gridX && cellY >= 0 && cellY < gridY) {
                matrix[cellX][cellY] = 1; // Add a grain at mouse
            }
        }

        // --- Update sand (falling logic) ---
        for (int j = gridY - 2; j >= 0; --j) { // from bottom-2 up
            for (int i = 0; i < gridX; ++i) {
                if (matrix[i][j] == 1 && matrix[i][j+1] == 0) {
                    matrix[i][j] = 0;
                    matrix[i][j+1] = 1;
                } else if (matrix[i][j] == 1 && i > 0 && matrix[i-1][j+1] == 0) {
                    // Check left diagonal
                    matrix[i][j] = 0;
                    matrix[i-1][j+1] = 1;
                } else if (matrix[i][j] == 1 && i < gridX - 1 && matrix[i+1][j+1] == 0) {
                    // Check right diagonal
                    matrix[i][j] = 0;
                    matrix[i+1][j+1] = 1;
                }
            }
        }

        // --- Render ---
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < gridX; ++i) {
            for (int j = 0; j < gridY; ++j) {
                if (matrix[i][j] == 1) {
                    SDL_Color color = { Uint8(i*255/gridX), Uint8(j*255/gridY), 255, 255 };
                    drawSand(width, i * width, j * width, renderer, color);
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void drawSand(int size, int x, int y, SDL_Renderer* renderer, SDL_Color color) {
    SDL_Rect rect = {x, y, size, size};
    SDL_SetRenderDrawColor(renderer, color.r, color.b, color.g, 255);
    SDL_RenderFillRect(renderer, &rect);
}