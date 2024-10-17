// DONT' RUIN IT COM ONNN

// My hard work!
// 03/01/2019


#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <atomic>
#include <termios.h>
#include <unistd.h>

const int WIDTH = 12;
const int HEIGHT = 22;

struct Point {
    int x, y;
};

class Tetris {
public:
    Tetris() {
        board.resize(HEIGHT, std::vector<char>(WIDTH, ' '));
        spawnNewPiece();
        running = true;
        inputThread = std::thread(&Tetris::getInput, this);
    }

    ~Tetris() {
        running = false;
        inputThread.join();
    }

    void run() {
        while (running) {
            draw();
            dropPiece();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        std::cout << "Game Over!" << std::endl;
    }

private:
    std::vector<std::vector<char>> board;
    std::vector<Point> currentPiece;
    int currentShape;
    std::atomic<bool> running;
    std::thread inputThread;

    void draw() {
        std::cout << "\033[2J\033[1;1H";
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (isPartOfCurrentPiece(x, y)) {
                    std::cout << "#";
                } else {
                    std::cout << board[y][x];
                }
            }
            std::cout << std::endl;
        }
    }

    bool isPartOfCurrentPiece(int x, int y) {
        for (const auto& p : currentPiece) {
            if (p.x == x && p.y == y) return true;
        }
        return false;
    }

    void dropPiece() {
        for (auto& p : currentPiece) {
            if (p.y + 1 >= HEIGHT || board[p.y + 1][p.x] == '#') {
                placePiece();
                return;
            }
        }
        for (auto& p : currentPiece) p.y++;
    }

    void placePiece() {
        for (const auto& p : currentPiece) {
            if (p.y < HEIGHT) board[p.y][p.x] = '#';
        }
        clearLines();
        spawnNewPiece();
        checkGameOver();
    }

    void clearLines() {
        for (int row = HEIGHT - 1; row >= 0; row--) {
            bool fullLine = true;
            for (int col = 0; col < WIDTH; col++) {
                if (board[row][col] == ' ') {
                    fullLine = false;
                    break;
                }
            }
            if (fullLine) {
                board.erase(board.begin() + row);
                board.insert(board.begin(), std::vector<char>(WIDTH, ' '));
            }
        }
    }

    void checkGameOver() {
        for (const auto& p : currentPiece) {
            if (p.y < 0) {
                running = false;
                break;
            }
        }
    }

    void spawnNewPiece() {
        currentShape = rand() % 7;
        currentPiece.clear();
        switch (currentShape) {
            case 0: currentPiece = { {0, 4}, {1, 4}, {1, 5}, {1, 6} }; break;
            case 1: currentPiece = { {0, 4}, {0, 5}, {1, 4}, {1, 5} }; break;
            case 2: currentPiece = { {0, 5}, {1, 4}, {1, 5}, {1, 6} }; break;
            case 3: currentPiece = { {0, 5}, {1, 4}, {1, 5}, {2, 5} }; break;
            case 4: currentPiece = { {0, 4}, {0, 5}, {1, 5}, {1, 6} }; break;
            case 5: currentPiece = { {0, 4}, {1, 4}, {2, 4}, {3, 4} }; break;
            case 6: currentPiece = { {0, 4}, {1, 4}, {1, 5}, {2, 5} }; break;
        }
    }

    void getInput() {
        while (running) {
            std::string input = readInput();
            handleInput(input);
        }
    }

    std::string readInput() {
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        char buf[3] = {0};
        int n = read(STDIN_FILENO, buf, sizeof(buf));
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return std::string(buf, n);
    }

    void handleInput(const std::string& input) {
        if (input.length() == 1) {
            switch (input[0]) {
                case 'a': movePiece(-1); break;
                case 'd': movePiece(1); break;
                case 's': dropPiece(); break;
                case 'w': rotatePiece(); break;
            }
        } else if (input.length() == 3 && input[0] == '\033') {
            if (input[1] == '[') {
                switch (input[2]) {
                    case 'A': rotatePiece(); break; // Up arrow
                    case 'B': dropPiece(); break;    // Down arrow
                    case 'D': movePiece(-1); break;  // Left arrow
                    case 'C': movePiece(1); break;   // Right arrow
                }
            }
        }
    }

    void movePiece(int direction) {
        for (auto& p : currentPiece) {
            if (p.x + direction < 0 || p.x + direction >= WIDTH || 
                (board[p.y][p.x + direction] == '#')) {
                return;
            }
        }
        for (auto& p : currentPiece) {
            p.x += direction;
        }
    }

    void rotatePiece() {
        for (auto& p : currentPiece) {
            if (currentShape == 0) {
                int tempX = p.x;
                p.x = p.y;
                p.y = 3 - tempX;
            }
        }
    }
};

int main() {
    std::cout << "Welcome to Tetris!" << std::endl;
    std::cout << "By Soichi, 2019" << std::endl;
    std::cout << "Please wait 5 seconds... Once you lose, press <ctrl-c> to exit" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    srand(static_cast<unsigned int>(time(0)));
    Tetris game;
    game.run();

    return 0;
}

