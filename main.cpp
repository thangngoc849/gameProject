#include <iostream>
using namespace std;

char board[3][3] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };
char current_marker;
int current_player;

void drawBoard() {
    system("clear");
    cout << "\n";
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cout << " " << board[i][j] << " ";
            if (j < 2) cout << "|";
        }
        cout << "\n";
        if (i < 2) cout << "---|---|---\n";
    }
    cout << "\n";
}

bool placeMarker(int slot) {
    int row = (slot - 1) / 3;
    int col = (slot - 1) % 3;
    if (board[row][col] != 'X' && board[row][col] != 'O') {
        board[row][col] = current_marker;
        return true;
    }
    return false;
}

int checkWinner() {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ') return current_player;
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ') return current_player;
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ') return current_player;
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ') return current_player;
    return 0;
}

void swapPlayerAndMarker() {
    current_marker = (current_marker == 'X') ? 'O' : 'X';
    current_player = (current_player == 1) ? 2 : 1;
}

void game() {
    cout << "Chon nguoi choi bat đau (1 hoac 2): ";
    cin >> current_player;
    current_marker = (current_player == 1) ? 'X' : 'O';

    int winner = 0;
    for (int i = 0; i < 9 && winner == 0; i++) {
        drawBoard();
        cout << "Nguoi chơi " << current_player << " (" << current_marker << "), nhap vi tri (1-9): ";
        int slot;
        cin >> slot;

        if (slot < 1 || slot > 9 || !placeMarker(slot)) {
            cout << "Nuoc di khong hop le! Thu lai.\n";
            i--;
            continue;
        }

        winner = checkWinner();
        if (winner == 0) swapPlayerAndMarker();
    }

    drawBoard();
    if (winner) cout << "Nguoi choi " << winner << " thang!\n";
    else cout << "Tro choi hoa!\n";
}

int main() {
    game();
    return 0;
} 
