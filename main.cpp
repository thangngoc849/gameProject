#include <SDL.h>

const int SCREEN_WIDTH = 300;
const int SCREEN_HEIGHT = 300;
const int CELL_SIZE = 100;

void drawX(SDL_Renderer* renderer, int row, int col) {
    int x = col * CELL_SIZE;
    int y = row * CELL_SIZE;
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_RenderDrawLine(renderer, x + 10, y + 10, x + CELL_SIZE - 10, y + CELL_SIZE - 10);
    SDL_RenderDrawLine(renderer, x + CELL_SIZE - 10, y + 10, x + 10, y + CELL_SIZE - 10);
}

void drawO(SDL_Renderer* renderer, int row, int col) {
    int x = col * CELL_SIZE + CELL_SIZE / 2;
    int y = row * CELL_SIZE + CELL_SIZE / 2;
    int r = CELL_SIZE / 2 - 10;
    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);
    for (int w = 0; w < r * 2; ++w)
        for (int h = 0; h < r * 2; ++h)
            if ((r - w) * (r - w) + (r - h) * (r - h) <= r * r &&
                (r - w) * (r - w) + (r - h) * (r - h) >= (r - 2) * (r - 2))
                SDL_RenderDrawPoint(renderer, x + w - r, y + h - r);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < 3; ++i) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT); // Dọc
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, SCREEN_WIDTH, i * CELL_SIZE);   // Ngang
    }

    drawX(renderer, 0, 0);
    drawO(renderer, 1, 1);

    SDL_RenderPresent(renderer);
    SDL_Delay(10000);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

