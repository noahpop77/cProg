#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <iomanip>

// short hand for a 2d array -> Vector of vectors that hold ints (sudoku numbers)
using Grid = std::vector<std::vector<int>>;

struct Cell {
    int r, c;
};

void printGrid(const Grid& grid, int blockDim) {
    int size = blockDim * blockDim;
    int lineLength = size * 3 + blockDim; // magic number-ish

    for (int r = 0; r < size; ++r) {
        // r > 0 to avoid printing lines on outer edges of grid
        // r % blockDim to print inner band lines
        if (r > 0 && r % blockDim == 0) {
            std::cout << std::string(lineLength, '-') << "\n";
        }

        // Iterates over each col
        for (int c = 0; c < size; ++c) {
            if (c > 0 && c % blockDim == 0) {
                std::cout << "| ";
            }
            if (grid[r][c] == 0) {
                std::cout << " . "; // . represents zeroed out cells
            } else {
                std::cout << std::setw(2) << grid[r][c] << " ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

// Time/space complexity: O(N^2) where N is the dimensions of the puzzle
Grid generateBasePattern(int blockDim) {
    int size = blockDim * blockDim;
    Grid grid = Grid(size, std::vector<int>(size, 0));

    for (int r = 0; r < size; ++r) {    // O(N^2)
        for (int c = 0; c < size; ++c) {
            // Generates the latin square (rows of 0-9 with incrementing offset) 
            grid[r][c] = ((r % blockDim) * blockDim + (r / blockDim) + c) % size + 1;
        }
    }
    return grid;
}

/*
Transformations:
9x9 = size 10
Iota:       [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
Shuffle:    [0, 7, 4, 1, 9, 2, 5, 8, 3, 6]
0 is kept for zeroed out/blank cells in the puzzle
*/
void shuffleDigits(Grid& grid, int size, std::mt19937& rng) {
    std::vector<int> numMap(size + 1);
    std::iota(numMap.begin(), numMap.end(), 0);
    std::shuffle(numMap.begin() + 1, numMap.end(), rng);

    for (auto& row : grid) {
        for (int& cell : row) {
            cell = numMap[cell];
        }
    }
}

// Shuffles rows in band
// As long as row/col shuffles are in band, sodoku rules arent broken
void shuffleRowsInBands(Grid& grid, int blockDim, std::mt19937& rng) {
    for (int band = 0; band < blockDim; ++band) {
        int bandStart = band * blockDim;
        std::shuffle(grid.begin() + bandStart, grid.begin() + bandStart + blockDim, rng);
    }
}

void shuffleColumnsInBands(Grid& grid, int blockDim, std::mt19937& rng) {
    int size = blockDim * blockDim;

    for (int band = 0; band < blockDim; band++) {
        std::vector<int> colOffsets(blockDim);                  // [       ]
        std::iota(colOffsets.begin(), colOffsets.end(), 0);     // [0, 1, 2]
        std::shuffle(colOffsets.begin(), colOffsets.end(), rng);// [2, 0, 1]

        for (int r = 0; r < size; ++r) {
            // Snapshots the row contents before overwriting them
            std::vector<int> originalCol(blockDim); 
            
            // Sets starting col 0, 3, or 6 (for 9x9)
            int bandStart = band * blockDim;

            // Writes current cells to the buffer
            for (int c = 0; c < blockDim; c++) {
                originalCol[c] = grid[r][bandStart + c];
            }
            // Writes new values based on offsets
            for (int c = 0; c < blockDim; c++) {
                grid[r][bandStart + c] = originalCol[colOffsets[c]];
            }
        }
    }
}

void shuffleGrid(Grid& grid, int blockDim, std::mt19937& rng) {
    int size = blockDim * blockDim;
    shuffleDigits(grid, size, rng);
    shuffleRowsInBands(grid, blockDim, rng);
    shuffleColumnsInBands(grid, blockDim, rng);
}

void digOutCells(Grid& grid, int blockDim, double removeRatio, std::mt19937& rng) {
    int size = blockDim * blockDim;
    int totalCells = size * size;

    // Gets int value of number of cells to remove 
    int targetRemovals = static_cast<int>(totalCells * removeRatio);
    
    // Generates vector with all possible coordinates in the total grid
    std::vector<Cell> cells;
    cells.reserve(totalCells); // preallocates memory for vec
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            cells.push_back({r, c});
        }
    }

    std::shuffle(cells.begin(), cells.end(), rng);

    int removed = 0;
    for (int i = 0; i < cells.size(); ++i) {
        if (removed >= targetRemovals) { break; }

        const Cell& cell = cells[i];

        if (grid[cell.r][cell.c] != 0) {
            grid[cell.r][cell.c] = 0;
            removed++;
        }
    }
}

int main(int argc, char* argv[]) {
    std::random_device rd;
    std::mt19937 rng(rd());

    int blockDim = 3;
    double removalRatio = 0.50;
    int size = blockDim * blockDim;

    // Command-line flags - TODO: Throw this in its own function
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if ((arg == "-b" || arg == "--block-dim") && i + 1 < argc) {
            blockDim = std::stoi(argv[++i]);
        } else if ((arg == "-r" || arg == "--removal-ratio") && i + 1 < argc) {
            removalRatio = std::stod(argv[++i]);
        }
    }
    
    std::cout << "=========================================\n";
    std::cout << " Sudoku Generator - " << size << "x" << size << "\n";
    std::cout << "=========================================\n\n";

    Grid solution = generateBasePattern(blockDim);
    shuffleGrid(solution, blockDim, rng);

    Grid puzzle = solution;
    digOutCells(puzzle, blockDim, removalRatio, rng);

    std::cout << "-------- PUZZLE --------\n";
    printGrid(puzzle, blockDim);

    std::cout << "-------- SOLUTION --------\n";
    printGrid(solution, blockDim);

    return 0;
}

