#include <SDL.h>
#include <iostream>

const int SCREEN_WIDTH = 300;
const int SCREEN_HEIGHT = 300;
const int CELL_SIZE = 100;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
char board[3][3];
char current_marker = 'X';
int current_player = 1;

void resetBoard() {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            board[i][j] = ' ';
}

void drawGrid() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < 3; ++i) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, SCREEN_WIDTH, i * CELL_SIZE);
    }
}

void drawX(int row, int col) {
    int x = col * CELL_SIZE;
    int y = row * CELL_SIZE;
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_RenderDrawLine(renderer, x + 10, y + 10, x + CELL_SIZE - 10, y + CELL_SIZE - 10);
    SDL_RenderDrawLine(renderer, x + CELL_SIZE - 10, y + 10, x + 10, y + CELL_SIZE - 10);
}

void drawO(int row, int col) {
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

void drawBoard() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    drawGrid();

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == 'X') drawX(i, j);
            else if (board[i][j] == 'O') drawO(i, j);
        }

    SDL_RenderPresent(renderer);
}

// Đánh dấu ô nếu còn trống
bool placeMarker(int row, int col) {
    if (board[row][col] == ' ') {
        board[row][col] = current_marker;
        return true;
    }
    return false;
}

// Kiểm tra người thắng
int checkWinner() {
    for (int i = 0; i < 3; i++) {
        // Hàng
        if (board[i][0] != ' ' &&
            board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return current_player;

        // Cột
        if (board[0][i] != ' ' &&
            board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return current_player;
    }

    // Chéo
    if (board[0][0] != ' ' &&
        board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return current_player;

    if (board[0][2] != ' ' &&
        board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return current_player;

    return 0;
}

// Xử lý sự kiện click chuột
void handleMouseClick(int x, int y) {
    int row = y / CELL_SIZE;
    int col = x / CELL_SIZE;

    if (row < 0 || row > 2 || col < 0 || col > 2) return;

    if (placeMarker(row, col)) {
        int winner = checkWinner();
        if (winner) {
            std::cout << "Người chơi " << winner << " (" << current_marker << ") thắng!\n";
            SDL_Delay(2000);
            resetBoard();
            return;
        }

        current_marker = (current_marker == 'X') ? 'O' : 'X';
        current_player = (current_player == 1) ? 2 : 1;
    }
}


int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    resetBoard();

    bool running = true;
    SDL_Event event;
    while (running) {
        drawBoard();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
                handleMouseClick(event.button.x, event.button.y);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
