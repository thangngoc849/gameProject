#include <SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

const int WINDOW_SIZE = 600;
const int MAX_SIZE = 12;
int boardSize = 3;
int winLength = 3;
char board[MAX_SIZE][MAX_SIZE];
char current_marker = 'X';
int current_player = 1;

SDL_Texture* backgroundTexture = nullptr;  
SDL_Texture* mapSelectBackgroundTexture = nullptr;  

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* pvpTexture = nullptr;
SDL_Texture* pvcTexture = nullptr;
SDL_Texture* start3x3Texture = nullptr;
SDL_Texture* start4x4Texture = nullptr;
SDL_Texture* start6x6Texture = nullptr;
SDL_Texture* start9x9Texture = nullptr;
SDL_Texture* start12x12Texture = nullptr;
SDL_Texture* winner1Texture = nullptr;
SDL_Texture* winner2Texture = nullptr;
SDL_Texture* drawTexture = nullptr;
SDL_Texture* pvcWinTexture = nullptr;
SDL_Texture* pvcLoseTexture = nullptr;
SDL_Texture* pvcDrawTexture = nullptr;


bool isPvC = false;
bool playerTurn = true;


struct WinLine {
    bool active = false;
    SDL_Point start, end;
} winLine;

SDL_Texture* loadTexture(const char* path) {
    SDL_Surface* surface = SDL_LoadBMP(path);
    if (!surface) {
        std::cerr << "Cannot load image " << path << ": " << SDL_GetError() << '\n';
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
    for (int i = -2; i <= 2; ++i) { 
        SDL_RenderDrawLine(renderer, x + 10, y + 10 + i, x + cellSize - 10, y + cellSize - 10 + i);
        SDL_RenderDrawLine(renderer, x + cellSize - 10, y + 10 + i, x + 10, y + cellSize - 10 + i);
    }
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
            int distSq = dx * dx + dy * dy;
            if (distSq <= radius * radius && distSq >= (radius - 4) * (radius - 4)) 
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
            for (int i = -4; i <= 4; ++i) {
                SDL_RenderDrawLine(renderer,
                    winLine.start.x, winLine.start.y + i,
                    winLine.end.x, winLine.end.y + i);
            }
        }
        

    SDL_RenderPresent(renderer);
}

int checkWinner(bool updateLine = true) {
    int cs = getCellSize();
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j <= boardSize - winLength; ++j) {
            char c = board[i][j];
            if (c != ' ') {
                bool win = true;
                for (int k = 1; k < winLength; ++k)
                    if (board[i][j + k] != c) win = false;
                if (win) {
                    if (updateLine)
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
                    if (updateLine)
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
                    if (updateLine)
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
                    if (updateLine)
                        winLine = {{true}, {j * cs + cs / 2, (i + winLength - 1) * cs + cs / 2},
                                           {(j + winLength - 1) * cs + cs / 2, i * cs + cs / 2}};
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

bool findWinningMove(char marker, int& row, int& col) {
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (board[i][j] == ' ') {
                board[i][j] = marker;
                int result = checkWinner(false); 
                board[i][j] = ' '; 
                if (result != 0) {
                    row = i;
                    col = j;
                    return true;
                }
            }
        }
    }
    return false;
}

bool detectThreat(char marker, int& blockRow, int& blockCol) {
    const int directions[4][2] = {
        {0, 1},  
        {1, 0},  
        {1, 1},  
        {1, -1}  
    };

    int threatThreshold = winLength - 3; 

    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            for (int d = 0; d < 4; ++d) {
                int dx = directions[d][0];
                int dy = directions[d][1];
                int count = 0;
                int emptyCount = 0;
                int lastEmptyRow = -1, lastEmptyCol = -1;

                for (int k = 0; k < winLength; ++k) {
                    int ni = i + k * dx;
                    int nj = j + k * dy;
                    if (ni >= 0 && ni < boardSize && nj >= 0 && nj < boardSize) {
                        if (board[ni][nj] == marker)
                            ++count;
                        else if (board[ni][nj] == ' ') {
                            ++emptyCount;
                            lastEmptyRow = ni;
                            lastEmptyCol = nj;
                        } else {
                            break;
                        }
                    } else break;
                }

                
                if (count >= threatThreshold && (count + emptyCount == winLength)) {
                    blockRow = lastEmptyRow;
                    blockCol = lastEmptyCol;
                    return true;
                }
            }
        }
    }

    return false;
}




void makeAIMove(int& winner, bool& inEndScreen) {
    int row = -1, col = -1;

    
    if (findWinningMove(current_marker, row, col)) {
        board[row][col] = current_marker;
    }
    
    else if (findWinningMove('X', row, col)) {
        board[row][col] = current_marker;
    }
    else if (detectThreat('X', row, col)) { 
    board[row][col] = current_marker;
    }
    
    else {
        std::vector<std::pair<int, int>> possibleMoves;

        for (int i = 0; i < boardSize; ++i) {
            for (int j = 0; j < boardSize; ++j) {
                if (board[i][j] == 'X') {
                    for (int dx = -1; dx <= 1; ++dx) {
                        for (int dy = -1; dy <= 1; ++dy) {
                            if (dx == 0 && dy == 0) continue;
                            int ni = i + dx;
                            int nj = j + dy;
                            if (ni >= 0 && ni < boardSize && nj >= 0 && nj < boardSize && board[ni][nj] == ' ') {
                                possibleMoves.emplace_back(ni, nj);
                            }
                        }
                    }
                }
            }
        }

        if (!possibleMoves.empty()) {
            int index = rand() % possibleMoves.size();
            row = possibleMoves[index].first;
            col = possibleMoves[index].second;
            board[row][col] = current_marker;
        } else {
            
            for (int i = 0; i < boardSize && row == -1; ++i) {
                for (int j = 0; j < boardSize && col == -1; ++j) {
                    if (board[i][j] == ' ') {
                        row = i;
                        col = j;
                        board[row][col] = current_marker;
                    }
                }
            }
        }
    }

    
    winner = checkWinner();
    if (winner) {
        drawBoard();
        SDL_Delay(2000);
        inEndScreen = true;
    } else {
        bool full = true;
        for (int i = 0; i < boardSize && full; ++i)
            for (int j = 0; j < boardSize && full; ++j)
                if (board[i][j] == ' ') full = false;

        if (full) {
            winner = 0;
            drawBoard();
            SDL_Delay(2000);
            inEndScreen = true;
        } else {
            current_marker = (current_marker == 'X') ? 'O' : 'X';
            current_player = (current_player == 1) ? 2 : 1;
            playerTurn = true;
        }
    }
}



void handleMouseClick(int x, int y, bool& inStartScreen, bool& inMapSelectionScreen, bool& inEndScreen, int& winner) {
    int xLeft = 240, xRight = 360;

    if (inStartScreen) {
        
        if (x >= xLeft && x <= xRight) {
            if (y >= 100 && y <= 150) { 
                isPvC = false; 
            } else if (y >= 170 && y <= 220) { 
                isPvC = true;  
            } else return;

            
            inStartScreen = false;
            inMapSelectionScreen = true;
            return;
        }
    }

    if (inMapSelectionScreen) {
        
        if (x >= xLeft && x <= xRight) {
            if (y >= 100 && y <= 150) { boardSize = 3; winLength = 3; }
            else if (y >= 170 && y <= 220) { boardSize = 4; winLength = 3; }
            else if (y >= 240 && y <= 290) { boardSize = 6; winLength = 4; }
            else if (y >= 310 && y <= 360) { boardSize = 9; winLength = 5; }
            else if (y >= 380 && y <= 430) { boardSize = 12; winLength = 5; }
            else return;

            
            inMapSelectionScreen = false;
            resetBoard();
            playerTurn = true;
            return;
        }
    }

    if (inEndScreen) {
        
        inEndScreen = false;
        inStartScreen = true;
        resetBoard(); 
        playerTurn = true; 
        return;
    }

    
    if (isPvC && !playerTurn) return; 

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
            bool full = true;
            for (int i = 0; i < boardSize && full; ++i)
                for (int j = 0; j < boardSize && full; ++j)
                    if (board[i][j] == ' ') full = false;

            if (full) {
                winner = 0;
                drawBoard();
                SDL_Delay(2000);
                inEndScreen = true;
            } else {
                current_marker = (current_marker == 'X') ? 'O' : 'X';
                current_player = (current_player == 1) ? 2 : 1;

                if (isPvC) playerTurn = false;
            }
        }
    }
}




void drawStartScreen() {
    
    if (backgroundTexture) {
        SDL_Rect dstRect = { 0, 0, WINDOW_SIZE, WINDOW_SIZE };  
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &dstRect);
    }

    int centerX = 240;
    
    if (pvpTexture) SDL_RenderCopy(renderer, pvpTexture, NULL, &(SDL_Rect{240, 100, 120, 50}));
    if (pvcTexture) SDL_RenderCopy(renderer, pvcTexture, NULL, &(SDL_Rect{240, 170, 120, 50}));

    SDL_RenderPresent(renderer);
}


void drawMapSelectionScreen(){
    
    if (mapSelectBackgroundTexture) {
        SDL_Rect dstRect = { 0, 0, WINDOW_SIZE, WINDOW_SIZE };  
        SDL_RenderCopy(renderer, mapSelectBackgroundTexture, NULL, &dstRect);
    }

    int centerX = 240;
    
    if (start3x3Texture) SDL_RenderCopy(renderer, start3x3Texture, NULL, &(SDL_Rect{centerX, 100, 120, 50}));
    if (start4x4Texture) SDL_RenderCopy(renderer, start4x4Texture, NULL, &(SDL_Rect{centerX, 170, 120, 50}));
    if (start6x6Texture) SDL_RenderCopy(renderer, start6x6Texture, NULL, &(SDL_Rect{centerX, 240, 120, 50}));
    if (start9x9Texture) SDL_RenderCopy(renderer, start9x9Texture, NULL, &(SDL_Rect{centerX, 310, 120, 50}));
    if (start12x12Texture) SDL_RenderCopy(renderer, start12x12Texture, NULL, &(SDL_Rect{centerX, 380, 120, 50}));

    SDL_RenderPresent(renderer);
}


void drawEndScreen(int winner) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 220, 255);
    SDL_RenderClear(renderer);

    SDL_Texture* texture = nullptr;

    if (isPvC) {
        if (winner == 1) texture = pvcWinTexture;      
        else if (winner == 2) texture = pvcLoseTexture; 
        else texture = pvcDrawTexture;                
    } else {
        if (winner == 1) texture = winner1Texture;
        else if (winner == 2) texture = winner2Texture;
        else texture = drawTexture;
    }

    if (texture) {
        SDL_Rect dst = {0, 0, 600, 600};
        SDL_RenderCopy(renderer, texture, NULL, &dst);
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    srand(static_cast<unsigned>(time(0)));

    window = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE, WINDOW_SIZE, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


    backgroundTexture = loadTexture("background.bmp");  
    mapSelectBackgroundTexture = loadTexture("map_select_background.bmp"); 

    pvpTexture = loadTexture("pvp.bmp");
    pvcTexture = loadTexture("pvc.bmp");
    start3x3Texture = loadTexture("start3x3.bmp");
    start4x4Texture = loadTexture("start4x4.bmp");
    start6x6Texture = loadTexture("start6x6.bmp");
    start9x9Texture = loadTexture("start9x9.bmp");
    start12x12Texture = loadTexture("start12x12.bmp");
    winner1Texture = loadTexture("winner1.bmp");
    winner2Texture = loadTexture("winner2.bmp");
    drawTexture = loadTexture("draw.bmp");
    pvcWinTexture = loadTexture("pvc_win.bmp");
    pvcLoseTexture = loadTexture("pvc_lose.bmp");
    pvcDrawTexture = loadTexture("pvc_draw.bmp");


    bool running = true;
    bool inStartScreen = true;
    bool inMapSelectionScreen = false; 
    bool inEndScreen = false;
    int winner = 0;

    SDL_Event event;
    resetBoard();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                handleMouseClick(event.button.x, event.button.y, inStartScreen, inMapSelectionScreen, inEndScreen, winner);
            }
        }

        if (inStartScreen) {
            drawStartScreen();
        } else if (inMapSelectionScreen) {
            drawMapSelectionScreen();
        } else if (inEndScreen) {
            drawEndScreen(winner);
        } else {
            if (isPvC && !playerTurn) {
                SDL_Delay(300); 
                makeAIMove(winner, inEndScreen);
            }
            drawBoard();
        }
    }

    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(mapSelectBackgroundTexture);

    SDL_DestroyTexture(pvpTexture);
    SDL_DestroyTexture(pvcTexture);
    SDL_DestroyTexture(start3x3Texture);
    SDL_DestroyTexture(start4x4Texture);
    SDL_DestroyTexture(start6x6Texture);
    SDL_DestroyTexture(start9x9Texture);
    SDL_DestroyTexture(start12x12Texture);
    SDL_DestroyTexture(winner1Texture);
    SDL_DestroyTexture(winner2Texture);
    SDL_DestroyTexture(drawTexture);
    SDL_DestroyTexture(pvcWinTexture);
    SDL_DestroyTexture(pvcLoseTexture);
    SDL_DestroyTexture(pvcDrawTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
