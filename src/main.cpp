#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Window* new_window = nullptr;

    int mouseX, mouseY;
    bool running = true;
    SDL_Event event;
    const int width = 9;
    const int dimensionX = 900;
    const int dimensionY = 600;
    int matrix[dimensionX/width][dimensionY/width] = {0};

    // Initialize SDL video subsystem
    SDL_Init(SDL_INIT_VIDEO);

    // Create a window and renderer
    SDL_CreateWindowAndRenderer(dimensionX, dimensionY, 0, &window, &renderer);

    while(running) {

        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false; // Exit the loop if quit event is received
            }
        }
        // Set draw color to black and clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_GetMouseState(&mouseX, &mouseY);
        int range = width / 2;
        int snappedX = (mouseX / width) * width + range;
        int snappedY = (mouseY / width) * width + range;

        // Set draw color to white and draw a point in the center
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for(int i = -range; i < range; ++i) {
            for (int j = -range; j < range; ++j) {
                SDL_RenderDrawPoint(renderer, snappedX + i, snappedY + j);
                matrix[snappedX / width][snappedY / width] = 1; // Mark the matrix position
            }
        }

        // Present the renderer
        SDL_RenderPresent(renderer);

        SDL_Delay(1);
    }
    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}