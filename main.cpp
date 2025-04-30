#include <SDL.h>
#include <iostream>

const int SCREEN_WIDTH = 300;
const int SCREEN_HEIGHT = 300;
const int CELL_SIZE = 100;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

char board[3][3]; // Bàn cờ
char current_marker = 'X';
int current_player = 1;

// Hàm khởi tạo lại bàn cờ
void resetBoard() {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            board[i][j] = ' ';
    current_marker = 'X';
    current_player = 1;
}

// Vẽ đường lưới
void drawGrid() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < 3; ++i) {
        // Dọc
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT);
        // Ngang
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, SCREEN_WIDTH, i * CELL_SIZE);
    }
}

// Vẽ dấu X
void drawX(int row, int col) {
    int x = col * CELL_SIZE;
    int y = row * CELL_SIZE;

    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_RenderDrawLine(renderer, x + 10, y + 10, x + CELL_SIZE - 10, y + CELL_SIZE - 10);
    SDL_RenderDrawLine(renderer, x + CELL_SIZE - 10, y + 10, x + 10, y + CELL_SIZE - 10);
}

// Vẽ dấu O
void drawO(int row, int col) {
    int x = col * CELL_SIZE + CELL_SIZE / 2;
    int y = row * CELL_SIZE + CELL_SIZE / 2;
    int radius = CELL_SIZE / 2 - 10;

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);
    for (int w = 0; w < radius * 2; ++w) {
        for (int h = 0; h < radius * 2; ++h) {
            int dx = radius - w;
            int dy = radius - h;
            if (dx * dx + dy * dy <= radius * radius &&
                dx * dx + dy * dy >= (radius - 2) * (radius - 2)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

// Vẽ toàn bộ bàn cờ và các ô đã đánh
void drawBoard() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // nền trắng
    SDL_RenderClear(renderer);

    drawGrid();

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == 'X') drawX(i, j);
            else if (board[i][j] == 'O') drawO(i, j);
        }

    SDL_RenderPresent(renderer);
}

// Vẽ màn hình chờ ban đầu
void drawStartScreen() {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // nền xám
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect box = {50, 130, 200, 40};
    SDL_RenderDrawRect(renderer, &box);

    // Dòng tượng trưng chữ (có thể dùng SDL_ttf sau)
    SDL_RenderDrawLine(renderer, 60, 140, 240, 140);

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
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return current_player;
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return current_player;
    }

    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return current_player;

    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
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

    bool running = true;
    bool in_start_screen = true;

    SDL_Event event;
    resetBoard();

    while (running) {
        if (in_start_screen) {
            drawStartScreen();
        } else {
            drawBoard();
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;

            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                if (in_start_screen) {
                    in_start_screen = false;
                    resetBoard();
                } else {
                    handleMouseClick(event.button.x, event.button.y);
                }
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
