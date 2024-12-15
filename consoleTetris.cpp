#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <random>
#include <windows.h>

// Class representing the game board
class Board {
private:
    int width;  // Width of the board
    int height; // Height of the board
    std::vector<std::vector<int>> grid; // 2D grid representing the board

public:
    // Constructor to initialize the board with given dimensions
    Board(int w, int h) : width(w), height(h) {
        grid.resize(height, std::vector<int>(width, 0)); // Fill the grid with zeros
    }

    // Display the current state of the board
    void display() {
        for (const auto& row : grid) {
            for (int cell : row) {
                if (cell == 0)
                    std::cout << ". "; // Empty cell
                else
                    std::cout << "# "; // Filled cell
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    // Check if a tetromino can be placed at a given position
    bool canPlaceTetromino(const std::vector<std::vector<int>>& tetromino, int x, int y) {
        for (size_t i = 0; i < tetromino.size(); ++i) {
            for (size_t j = 0; j < tetromino[i].size(); ++j) {
                if (tetromino[i][j] == 1) { // Check each cell of the tetromino
                    int newX = x + j;
                    int newY = y + i;

                    // Verify boundaries and collision with existing pieces
                    if (newX < 0 || newX >= width || newY < 0 || newY >= height || grid[newY][newX] == 1) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    // Add a tetromino to the board at the specified position
    void addTetromino(const std::vector<std::vector<int>>& tetromino, int x, int y) {
        for (size_t i = 0; i < tetromino.size(); ++i) {
            for (size_t j = 0; j < tetromino[i].size(); ++j) {
                if (tetromino[i][j] == 1) { // Check each cell of the tetromino
                    int newX = x + j;
                    int newY = y + i;
                    grid[newY][newX] = 1; // Mark the grid cell as filled
                }
            }
        }
    }

    // Clear rows that are completely filled
    void clearFullLines() {
        for (int i = height - 1; i >= 0; --i) { // Start from the bottom row
            if (std::all_of(grid[i].begin(), grid[i].end(), [](int cell) { return cell == 1; })) {
                grid.erase(grid.begin() + i); // Remove the filled row
                grid.insert(grid.begin(), std::vector<int>(width, 0)); // Add an empty row at the top
                ++i; // Re-check the same row index
            }
        }
    }

    // Clear a tetromino from its current position on the board
    void clearTetromino(const std::vector<std::vector<int>>& tetromino, int x, int y) {
        for (size_t i = 0; i < tetromino.size(); ++i) {
            for (size_t j = 0; j < tetromino[i].size(); ++j) {
                if (tetromino[i][j] == 1) { // Check each cell of the tetromino
                    int newX = x + j;
                    int newY = y + i;

                    // Ensure the position is within bounds
                    if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                        grid[newY][newX] = 0; // Clear the cell
                    }
                }
            }
        }
    }

    // Attempt to drop a tetromino down by one row
    bool dropTetromino(const std::vector<std::vector<int>>& tetromino, int& x, int& y) {
        clearTetromino(tetromino, x, y); // Clear the current position
        if (canPlaceTetromino(tetromino, x, y + 1)) { // Check if it can drop
            ++y; // Move down
            addTetromino(tetromino, x, y); // Add to the new position
            return true; // Drop successful
        } else {
            addTetromino(tetromino, x, y); // Lock the piece in place
            return false; // Cannot drop further
        }
    }
};

// Class representing a tetromino and its operations
class Tetromino {
public:
    std::vector<std::vector<std::vector<int>>> shapes; // Different tetromino shapes

    // Constructor to initialize tetromino shapes
    Tetromino() {
        shapes.push_back({ {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} }); // Line
        shapes.push_back({ {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} }); // Square
        shapes.push_back({ {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0} }); // L-shape
        shapes.push_back({ {0, 1, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0} }); // Reverse L
        shapes.push_back({ {0, 0, 1, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} }); // S-shape
        shapes.push_back({ {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} }); // Z-shape
        shapes.push_back({ {1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} }); // T-shape
    }

    // Get a specific shape by index
    const std::vector<std::vector<int>>& getShape(int index) {
        return shapes[index];
    }

    // Rotate a tetromino 90 degrees clockwise
    void rotateRight(std::vector<std::vector<int>>& shape, Board& board, int x, int y) {
        std::vector<std::vector<int>> rotated(4, std::vector<int>(4, 0));
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                rotated[j][4 - i - 1] = shape[i][j];
            }
        }
        if (board.canPlaceTetromino(rotated, x, y)) { // Check if rotation is valid
            shape = rotated;
        }
    }

    // Rotate a tetromino 90 degrees counterclockwise
    void rotateLeft(std::vector<std::vector<int>>& shape, Board& board, int x, int y) {
        std::vector<std::vector<int>> rotated(4, std::vector<int>(4, 0));
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                rotated[4 - j - 1][i] = shape[i][j];
            }
        }
        if (board.canPlaceTetromino(rotated, x, y)) { // Check if rotation is valid
            shape = rotated;
        }
    }

    // Move a tetromino left if possible
    void shiftLeft(std::vector<std::vector<int>>& shape, int& x, int y, Board& board) {
        if (board.canPlaceTetromino(shape, x - 1, y)) {
            --x; // Move left
        }
    }

    // Move a tetromino right if possible
    void shiftRight(std::vector<std::vector<int>>& shape, int& x, int y, Board& board) {
        if (board.canPlaceTetromino(shape, x + 1, y)) {
            ++x; // Move right
        }
    }
};

int main() {
    Tetromino tetromino;
    Board board(10, 20); // Create a 10x20 game board
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_int_distribution<int> dist(0, 6);

    while (true) {
        int x = 3; // Initial x position
        int y = 0; // Initial y position
        auto shape = tetromino.getShape(dist(gen)); // Get a random tetromino shape

        if (!board.canPlaceTetromino(shape, x, y)) { // Check if the shape can be placed
            break; // End game if not
        }

        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Delay for smooth dropping
            system("cls"); // Clear the screen
            board.clearTetromino(shape, x, y); // Clear current position

            // Check user input for rotation and movement
            if (GetAsyncKeyState(68)) { // Press 'D' to rotate right
                tetromino.rotateRight(shape, board, x, y);
            }
            if (GetAsyncKeyState(65)) { // Press 'A' to rotate left
                tetromino.rotateLeft(shape, board, x, y);
            }
            if (GetAsyncKeyState(37)) { // Left arrow to move left
                tetromino.shiftLeft(shape, x, y, board);
            }
            if (GetAsyncKeyState(39)) { // Right arrow to move right
                tetromino.shiftRight(shape, x, y, board);
            }

            if (board.dropTetromino(shape, x, y)) { // Attempt to drop the tetromino
                board.clearFullLines(); // Clear full lines if any
                board.display(); // Render the board
            } else {
                std::cout << "Game Over :("; // End game message
                break;
            }
        }
    }
}
