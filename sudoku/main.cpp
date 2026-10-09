#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <iomanip>

/*
Time Complexity: Nearly every function barring shuffleRowsInBands() is O(N^2)

*/

// short hand for a 2d array -> Vector of vectors that hold ints (sudoku numbers)
using Grid = std::vector<std::vector<int>>;

// Struct that holds a row and col we will associate with a cell in the grid
struct Cell {
    int r, c;
};

void printGrid(const Grid& grid, int blockDim) {
    int size = blockDim * blockDim;
    int lineLength = size * 3 + blockDim; // just there to make pretty

    // Iterates over each row
    for (int r = 0; r < size; ++r) {
        // r > 0 to avoid printing lines on outer edges of grid
        // r % blockDim to print inner band lines
        if (r > 0 && r % blockDim == 0) {
            std::cout << std::string(lineLength, '-') << "\n";
        }

        // Iterates over each col
        for (int c = 0; c < size; ++c) {
            if (c > 0 && c % blockDim == 0) { // Inner wall between bands
                std::cout << "| ";
            }
            if (grid[r][c] == 0) {
                std::cout << " . "; // . for zeroed out cells
            } else {
                std::cout << std::setw(2) << grid[r][c] << " ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

// Time/space complexity: O(N^2) where N is the dimensions of the puzzle
// However nearly nobody is gonna do a 600x600 sudoku puzzle.
// Most common case is a 3x3 and up to a 5x5 at MAX I would bet
Grid generateBasePattern(int blockDim) {
    int size = blockDim * blockDim;
    
    // Creates a 2d vector grid with zeroed out cells of dimmensions size x size
    Grid grid = Grid(size, std::vector<int>(size, 0));

    for (int r = 0; r < size; ++r) {    // O(N^2)
        for (int c = 0; c < size; ++c) {
            // Generates the latin square with numbered rows on shifted offsets 
            // Sudoku rule valid
            grid[r][c] = ((r % blockDim) * blockDim + (r / blockDim) + c) % size + 1;
        }
    }
    return grid;
}

void shuffleDigits(Grid& grid, int size, std::mt19937& rng) {
    std::vector<int> numMap(size + 1);                  // 9x9 = size 10
    std::iota(numMap.begin(), numMap.end(), 0); // [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
    // Randomize numbers from 1 onwards = numMap = [0, 7, 4, 1, 9, 2, 5, 8, 3, 6]
    std::shuffle(numMap.begin() + 1, numMap.end(), rng);

    for (auto& row : grid) {        // Loops over each vector
        for (int& cell : row) {     // Loops over each cell inside inner vector
            cell = numMap[cell];    // Maps grid cell to randomized value
        }
    }
}

// Shuffles rows in band
// As long as row shuffles are in band they sodoku rules arent broken
void shuffleRowsInBands(Grid& grid, int blockDim, std::mt19937& rng) {
    for (int band = 0; band < blockDim; ++band) {
        int bandStart = band * blockDim;
        std::shuffle(grid.begin() + bandStart, grid.begin() + bandStart + blockDim, rng);
    }
}

// You need the originalCol because otherwise the values in the grid get overwritten
// too soon and your grid would be corrupted before it would finish setting new
// offset values
void shuffleColumnsInBands(Grid& grid, int blockDim, std::mt19937& rng) {
    int size = blockDim * blockDim;

    for (int band = 0; band < blockDim; band++) {
        // usual shuffle setup, Iota, randomize order, set offsets
        std::vector<int> colOffsets(blockDim);                  // [       ]
        std::iota(colOffsets.begin(), colOffsets.end(), 0);     // [0, 1, 2]
        std::shuffle(colOffsets.begin(), colOffsets.end(), rng);// [2, 0, 1]

        for (int r = 0; r < size; ++r) {    // Loops over every row in the grid
            // Snapshots the row contents before overwriting them
            std::vector<int> originalCol(blockDim); 
            
            // Sets starting column to begin counting with
            int bandStart = band * blockDim;

            // Writes the contents of the current bands cells to the buffer
            for (int c = 0; c < blockDim; c++) {
                originalCol[c] = grid[r][bandStart + c];
            }

            // Writes the offsets using the original buffer
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

    // Counts cells removed. Uses ratio * total cells & casting to int
    int targetRemovals = static_cast<int>(totalCells * removeRatio);
    
    // Generates vector with all possible coordinates in the total grid
    std::vector<Cell> cells;
    cells.reserve(totalCells); // preallocates memory for vec, no alloc needed
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            cells.push_back({r, c});
        }
    }

    std::shuffle(cells.begin(), cells.end(), rng); // Shuffles the coordinates

    int removed = 0;
    for (int i = 0; i < cells.size(); ++i) {
        if (removed >= targetRemovals) { break; }

        const Cell& cell = cells[i];

        if (grid[cell.r][cell.c] != 0) {
            // Cells in the randomized vector of cells are zeroed out a until
            // the "removed" count is at the ratio specified in targetRemovals
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

    // Parse command-line flags
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
