#include <SDL.h>
#include <iostream>

const int WINDOW_SIZE = 400;
const int MAX_SIZE = 6;
int boardSize = 3;
int winLength = 3;
char board[MAX_SIZE][MAX_SIZE];
char current_marker = 'X';
int current_player = 1;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* start3x3Texture = nullptr;
SDL_Texture* start4x4Texture = nullptr;
SDL_Texture* start6x6Texture = nullptr;
SDL_Texture* winner1Texture = nullptr;
SDL_Texture* winner2Texture = nullptr;
SDL_Texture* drawTexture = nullptr;

struct WinLine {
    bool active = false;
    SDL_Point start, end;
} winLine;

SDL_Texture* loadTexture(const char* path) {
    SDL_Surface* surface = SDL_LoadBMP(path);
    if (!surface) {
        std::cerr << "Không thể tải ảnh " << path << ": " << SDL_GetError() << '\n';
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void resetBoard() {
    for (int i = 0; i < MAX_SIZE; ++i)
        for (int j = 0; j < MAX_SIZE; ++j)
            board[i][j] = ' ';
    current_marker = 'X';
    current_player = 1;
    winLine.active = false;
}

int getCellSize() {
    return WINDOW_SIZE / boardSize;
}

void drawGrid() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    int cellSize = getCellSize();
    for (int i = 1; i < boardSize; ++i) {
        SDL_RenderDrawLine(renderer, i * cellSize, 0, i * cellSize, WINDOW_SIZE);
        SDL_RenderDrawLine(renderer, 0, i * cellSize, WINDOW_SIZE, i * cellSize);
    }
}

void drawX(int row, int col) {
    int cellSize = getCellSize();
    int x = col * cellSize;
    int y = row * cellSize;
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_RenderDrawLine(renderer, x + 10, y + 10, x + cellSize - 10, y + cellSize - 10);
    SDL_RenderDrawLine(renderer, x + cellSize - 10, y + 10, x + 10, y + cellSize - 10);
}

void drawO(int row, int col) {
    int cellSize = getCellSize();
    int x = col * cellSize + cellSize / 2;
    int y = row * cellSize + cellSize / 2;
    int radius = cellSize / 2 - 10;
    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);
    for (int w = 0; w < radius * 2; ++w)
        for (int h = 0; h < radius * 2; ++h) {
            int dx = radius - w;
            int dy = radius - h;
            if (dx * dx + dy * dy <= radius * radius &&
                dx * dx + dy * dy >= (radius - 2) * (radius - 2))
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
        }
}

void drawBoard() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    drawGrid();
    for (int i = 0; i < boardSize; ++i)
        for (int j = 0; j < boardSize; ++j) {
            if (board[i][j] == 'X') drawX(i, j);
            else if (board[i][j] == 'O') drawO(i, j);
        }

    if (winLine.active) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderDrawLine(renderer, winLine.start.x, winLine.start.y, winLine.end.x, winLine.end.y);
    }

    SDL_RenderPresent(renderer);
}

int checkWinner() {
    int cs = getCellSize();
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j <= boardSize - winLength; ++j) {
            char c = board[i][j];
            if (c != ' ') {
                bool win = true;
                for (int k = 1; k < winLength; ++k)
                    if (board[i][j + k] != c) win = false;
                if (win) {
                    winLine = {{true}, {j * cs + cs / 2, i * cs + cs / 2},
                                      {(j + winLength - 1) * cs + cs / 2, i * cs + cs / 2}};
                    return current_player;
                }
            }

            c = board[j][i];
            if (c != ' ') {
                bool win = true;
                for (int k = 1; k < winLength; ++k)
                    if (board[j + k][i] != c) win = false;
                if (win) {
                    winLine = {{true}, {i * cs + cs / 2, j * cs + cs / 2},
                                      {i * cs + cs / 2, (j + winLength - 1) * cs + cs / 2}};
                    return current_player;
                }
            }
        }
    }

    for (int i = 0; i <= boardSize - winLength; ++i)
        for (int j = 0; j <= boardSize - winLength; ++j) {
            char c = board[i][j];
            if (c != ' ') {
                bool win = true;
                for (int k = 1; k < winLength; ++k)
                    if (board[i + k][j + k] != c) win = false;
                if (win) {
                    winLine = {{true}, {j * cs + cs / 2, i * cs + cs / 2},
                                      {(j + winLength - 1) * cs + cs / 2,
                                       (i + winLength - 1) * cs + cs / 2}};
                    return current_player;
                }
            }

            c = board[i + winLength - 1][j];
            if (c != ' ') {
                bool win = true;
                for (int k = 1; k < winLength; ++k)
                    if (board[i + winLength - 1 - k][j + k] != c) win = false;
                if (win) {
                    winLine = {{true}, {j * cs + cs / 2, (i + winLength - 1) * cs + cs / 2},
                                      {(j + winLength - 1) * cs + cs / 2,
                                       i * cs + cs / 2}};
                    return current_player;
                }
            }
        }

    return 0;
}

bool placeMarker(int row, int col) {
    if (board[row][col] == ' ') {
        board[row][col] = current_marker;
        return true;
    }
    return false;
}

void handleMouseClick(int x, int y, bool& inStartScreen, bool& inEndScreen, int& winner) {
    if (inStartScreen) {
        if (x >= 140 && x <= 260 && y >= 80 && y <= 120) {
            boardSize = 3; winLength = 3;
        } else if (x >= 140 && x <= 260 && y >= 140 && y <= 180) {
            boardSize = 4; winLength = 3;
        } else if (x >= 140 && x <= 260 && y >= 200 && y <= 240) {
            boardSize = 6; winLength = 4;
        } else return;

        SDL_SetWindowSize(window, WINDOW_SIZE, WINDOW_SIZE);
        inStartScreen = false;
        resetBoard();
        return;
    }

    if (inEndScreen) {
        inEndScreen = false;
        inStartScreen = true;
        return;
    }

    int cs = getCellSize();
    int row = y / cs;
    int col = x / cs;
    if (row < 0 || row >= boardSize || col < 0 || col >= boardSize) return;

    if (placeMarker(row, col)) {
        winner = checkWinner();
        if (winner) {
            drawBoard();
            SDL_Delay(2000);
            inEndScreen = true;
        } else {
            // Kiểm tra hòa
            bool full = true;
            for (int i = 0; i < boardSize && full; ++i)
                for (int j = 0; j < boardSize && full; ++j)
                    if (board[i][j] == ' ') full = false;

            if (full) {
                winner = 0; // hòa
                drawBoard();
                SDL_Delay(2000);
                inEndScreen = true;
            } else {
                current_marker = (current_marker == 'X') ? 'O' : 'X';
                current_player = (current_player == 1) ? 2 : 1;
            }
        }
    }
}

void drawStartScreen() {
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderClear(renderer);

    if (start3x3Texture) SDL_RenderCopy(renderer, start3x3Texture, NULL, &(SDL_Rect{140, 80, 100, 40}));
    if (start4x4Texture) SDL_RenderCopy(renderer, start4x4Texture, NULL, &(SDL_Rect{140, 140, 100, 40}));
    if (start6x6Texture) SDL_RenderCopy(renderer, start6x6Texture, NULL, &(SDL_Rect{140, 200, 100, 40}));

    SDL_RenderPresent(renderer);
}

void drawEndScreen(int winner) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 220, 255);
    SDL_RenderClear(renderer);

    SDL_Texture* texture = nullptr;
    if (winner == 1) texture = winner1Texture;
    else if (winner == 2) texture = winner2Texture;
    else texture = drawTexture;

    if (texture) {
        SDL_Rect dst = {90, 180, 220, 40};
        SDL_RenderCopy(renderer, texture, NULL, &dst);
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              WINDOW_SIZE, WINDOW_SIZE, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    start3x3Texture = loadTexture("start3x3.bmp");
    start4x4Texture = loadTexture("start4x4.bmp");
    start6x6Texture = loadTexture("start6x6.bmp");
    winner1Texture = loadTexture("winner1.bmp");
    winner2Texture = loadTexture("winner2.bmp");
    drawTexture = loadTexture("draw.bmp");

    bool running = true;
    bool inStartScreen = true;
    bool inEndScreen = false;
    int winner = 0;

    SDL_Event event;
    resetBoard();

    while (running) {
        if (inStartScreen) drawStartScreen();
        else if (inEndScreen) drawEndScreen(winner);
        else drawBoard();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
                handleMouseClick(event.button.x, event.button.y, inStartScreen, inEndScreen, winner);
        }
    }

    SDL_DestroyTexture(start3x3Texture);
    SDL_DestroyTexture(start4x4Texture);
    SDL_DestroyTexture(start6x6Texture);
    SDL_DestroyTexture(winner1Texture);
    SDL_DestroyTexture(winner2Texture);
    SDL_DestroyTexture(drawTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
