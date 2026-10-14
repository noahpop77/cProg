#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>

// Alias for a 2d array 
using Grid = std::vector<std::vector<int>>;

// Represents 1 cell in our grid
struct Cell {
    int r, c;
};

void printGrid(const Grid& grid, int blockDim) {
    int size = blockDim * blockDim;
    int lineLength = size * 3 + blockDim;

    for (int r = 0; r < size; ++r) {
        // r > 0 to avoid printing lines on outer edges of grid
        // r % blockDim to print inner band lines
        if (r > 0 && r % blockDim == 0) {
            std::cout << std::string(lineLength, '-') << "\n";
        }

        for (int c = 0; c < size; ++c) {
            // Inner wall between bands
            if (c > 0 && c % blockDim == 0) {
                std::cout << "| ";
            }
            // . for zeroed out cells
            if (grid[r][c] == 0) {
                std::cout << " . ";
            } else {
                // Makes single and double digit numbers align properly
                if (grid[r][c] < 10) {
                    std::cout << " "; 
                }
                std::cout << grid[r][c] << " ";
            }
        }
        std::cout << "\n"; // New line at end of row
    }
    std::cout << "\n";  // New line at end of grid
}

Grid generateLatinSquare(int blockDim) {
    int size = blockDim * blockDim;
    
    Grid grid = Grid(size, std::vector<int>(size, 0));

    for (int r = 0; r < size; r++) {    // O(N^2)
        for (int c = 0; c < size; c++) {
            // Generates the latin square with numbered rows on shifted offsets 
            // Sudoku rule valid
            grid[r][c] = ((r % blockDim) * blockDim + (r / blockDim) + c) % size + 1;
        }
    }
    return grid;
}

// The latin square will never have 0 in it so 0 will never be accidentally
// placed inside of the square with the randomized digits
// Its a number swap excluding zero
void shuffleDigits(Grid& grid, int size, std::mt19937& rng) {
// Vector -  9x9 = size 10
// iota -    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
// shuffle - [0, 7, 4, 1, 9, 2, 5, 8, 3, 6] - Randomize numbers from 1 onwards
    std::vector<int> numMap(size + 1);           
    std::iota(numMap.begin(), numMap.end(), 0); 
    std::shuffle(numMap.begin() + 1, numMap.end(), rng);

    for (auto& row : grid) {      // Auto cause rows datatype is vector
        for (int& cell : row) {
            cell = numMap[cell]; // Maps grid cell to randomized value
        }
    }
}

// Shuffles rows in band
// As long as row shuffles are in band they sodoku rules arent broken
void shuffleRowInBand(Grid& grid, int blockDim, std::mt19937& rng) {
    for (int band = 0; band < blockDim; band++) {
        int bandStart = band * blockDim;
        std::shuffle(grid.begin() + bandStart, grid.begin() + bandStart + blockDim, rng);
    }
}

// You need the originalCol otherwise the values in the grid get overwritten
// and corrupts value offset accuracy during transition
void shuffleColInBand(Grid& grid, int blockDim, std::mt19937& rng) {
    int size = blockDim * blockDim;

    for (int band = 0; band < blockDim; band++) {
        // usual shuffle setup, Iota, randomize order, set offsets
        std::vector<int> colOffsets(blockDim);                  // [       ]
        std::iota(colOffsets.begin(), colOffsets.end(), 0);     // [0, 1, 2]
        std::shuffle(colOffsets.begin(), colOffsets.end(), rng);// [2, 0, 1]

        for (int r = 0; r < size; r++) {
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
    shuffleRowInBand(grid, blockDim, rng);
    shuffleColInBand(grid, blockDim, rng);
}

void zeroOutCells(Grid& grid, int blockDim, double removeRatio, std::mt19937& rng) {
    int size = blockDim * blockDim;
    int totalCells = size * size;

    // Counts cells removed. Uses ratio * total cells & casting to int
    int targetRemovals = static_cast<int>(totalCells * removeRatio);
    
    // Generates vector with all possible coordinates in the total grid
    std::vector<Cell> cells;
    cells.reserve(totalCells); // preallocates memory for vec, no alloc needed
    for (int r = 0; r < size; r++) {
        for (int c = 0; c < size; c++) {
            cells.push_back({r, c});
        }
    }

    std::shuffle(cells.begin(), cells.end(), rng);

    int removed = 0;
    for (int i = 0; i < cells.size(); i++) {
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

// Main CLI flag loop - Add additional flags here if necessary
void cliFlags(int argc, char* argv[], int &blockDim, double &removalRatio) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if ((arg == "-b" || arg == "--block-dim") && i + 1 < argc) {
            blockDim = std::stoi(argv[i + 1]);
            i++;
        } else if ((arg == "-r" || arg == "--removal-ratio") && i + 1 < argc) {
            removalRatio = std::stod(argv[i + 1]);
            i++;
        }
    }
}

int main(int argc, char* argv[]) {
    std::random_device rd;
    std::mt19937 rng(rd());
    
    // Default values that get overwritten by cli flags
    int blockDim = 3;
    double removalRatio = 0.50;

    cliFlags(argc, argv, blockDim, removalRatio);
    int size = blockDim * blockDim;   
    
    std::cout << "=========================================\n";
    std::cout << " Sudoku Generator - " << size << "x" << size << "\n";
    std::cout << "=========================================\n\n";

    // Create solution grid
    Grid solution = generateLatinSquare(blockDim);
    shuffleGrid(solution, blockDim, rng);
    
    // Create puzzle grid
    Grid puzzle = solution;
    zeroOutCells(puzzle, blockDim, removalRatio, rng);

    // Print both grids to stdout
    std::cout << "-------- PUZZLE --------\n";
    printGrid(puzzle, blockDim);
    std::cout << "-------- SOLUTION --------\n";
    printGrid(solution, blockDim);

    return 0;
}

/*
=====================================================
Tests I Would Implement:
=====================================================

Mandatory -----------------------
- Input parameter sanity checking
    - Type checks
    - Bounds checks
- Sudoku rule checking
    - If there are duplicate numbers in the 
      same row, column, or band sector

Nice-To-Haves -------------------
- RNG seeding determinism
    - 2 different mt19937 rng devices with the 
      same seed should produce the same output
        - Might want to "replay" specific grid states 
          for testing or just replaying the same puzzle

- Removal ratio accuracy
    - Odd numbers & rounding behavior
    - Values of 0.0 or 1.0


=====================================================
Optimizations:
=====================================================

I did not explore if there are mathematical formula
that eliminate the N^2 functions in the program.
Might be possible.

Pros ------------------------------------------------
    Could offer O(N*log(N)) time complexity or better
    as opposed to O(N^2) time complexity

Cons ------------------------------------------------
    Chances of a human playing a game of 25x25+ game
    of sudoku is extremely small and optimizations
    would mainly show benefit in these cases.

Ex --------------------------------------------------
    A formula to calculate sum of odd squares in a series
    rather than brute forcing it.



Compiler Flags ======================================

-O0
    No optimizations
-O2
    Performs general optimizations

-O3
    Turns on agressive performance optimizations.
    This can lead to larger binary sizes which hurts
    CPU cache efficiency

-Oz
    Prioritizes space saving over execution time improvements
    Can improve cache efficiency which might provide
    more improvements over -03

Example Case:
9x9 with 0.5 removal rate

-O0 No optimizations
    avg: 0.204s
    bin: 115K
-O2
    avg: 0.184s
    bin: 41K
-O3
    More variance in the tests avg time.
    avg: 0.185s
    bin: 41K
-Oz
    avg: 0.185s
    bin: 48K

*/
