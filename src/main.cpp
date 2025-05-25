#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <random>
using namespace std;

void drawPixel(int size, int x, int y, SDL_Renderer* renderer);

int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    
    bool running = true;
    SDL_Event event;
    const int width = 3;
    const int dimensionX = 900;
    const int dimensionY = 600;
    int matrix[dimensionX/width][dimensionY/width] = {0};
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(dimensionX, dimensionY, 0, &window, &renderer);
    
    while(running) {
        
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        drawPixel(width, mouseX, mouseY, renderer);
        matrix[mouseX/width][mouseY/width] = 1;

        for (int j = (dimensionY/width) - 2; j >= 0; --j) {
            for (int i = 0; i < dimensionX/width; ++i) {
                if (matrix[i][j] == 1 && matrix[i][j+1] == 0) {
                    matrix[i][j] = 0;
                    matrix[i][j+1] = 1;
                } else if (matrix[i+1][j+1] == 0 && matrix[i][j] == 1) {
                    matrix[i][j] = 0;
                    matrix[i+1][j+1] = 1;
                } else if (matrix[i-1][j+1] == 0 && matrix[i][j] == 1) {
                    matrix[i][j] = 0;
                    matrix[i-1][j+1] = 1;
                }
            }
        }

        for (int i = 0; i < dimensionX/width; ++i) {
            for (int j = 0; j < dimensionY/width; ++j) {
                if (matrix[i][j] == 1) {
                    drawPixel(width, i * width, j * width, renderer);

                }
            }
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void drawPixel(int size, int x, int y, SDL_Renderer* renderer) {
        
    int range = size / 2;
        int snappedX = (x / size) * size + range;
        int snappedY = (y / size) * size + range;

        // Set draw color to white and draw a point in the center
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for(int i = -range; i < range; ++i) {
            for (int j = -range; j < range; ++j) {
                SDL_RenderDrawPoint(renderer, snappedX + i, snappedY + j);
            }
        }
}

