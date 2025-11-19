/*
    John conway's game of life(1970), written in C++17.
    This game runs on your console.
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <string>
#include <vector>
#include <array>
#include <utility>
#include <thread>
#include <chrono>
#include <cstdint>
#include <cstdlib>

using CellGroup = std::vector<std::pair<int32_t, int32_t>>;

template<uint32_t H, uint32_t W>
class GameOfLife {
    static constexpr uint32_t DELAY_MILLISEC = 439;
    static constexpr char EMPTY = ' ';
    static constexpr char ALIVE = '*';

    std::array<std::array<bool, W>, H> data;
    std::array<std::array<bool, W>, H> next;

    void clear_data() noexcept {
        for (auto& line : data) {
            line.fill(false);
        }
    }

    void clear_next() noexcept {
        for (auto& line : next) {
            line.fill(false);
        }
    }

    uint32_t count_neighbors(int32_t r, int32_t c) const noexcept {
        uint32_t num = 0;

        for (int32_t dr = -1; dr <= 1; ++dr) {
            for (int32_t dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) {
                    continue;
                }

                int32_t rr = (r + dr + H) % H;
                int32_t cc = (c + dc + W) % W;

                if (data[rr][cc]) {
                    ++num;
                }
            }
        }

        return num;
    }

    void clear_screen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    void render() {
        std::cout << "x";

        for (uint32_t i = 0; i < W; ++i) {
            std::cout << "-";
        }

        std::cout << "x\n";

        for (uint32_t r = 0; r < H; ++r) {
            std::cout << "|";
            for (uint32_t c = 0; c < W; ++c) {
                std::cout << (data[r][c] ? ALIVE : EMPTY);
            }

            std::cout << "|\n";
        }

        std::cout << "x";

        for (uint32_t i = 0; i < W; ++i) {
            std::cout << "-";
        }

        std::cout << "x\n";
    }

    void update() {
        for (int32_t r = 0; r < H; ++r) {
            for (int32_t c = 0; c < W; ++c) {
                uint32_t neighbors = count_neighbors(r, c);

                if (data[r][c]) {
                    next[r][c] = (neighbors == 2 || neighbors == 3);
                }
                else {
                    next[r][c] = (neighbors == 3);
                }
            }
        }

        std::swap(data, next);
        clear_next();
    }
public:
    GameOfLife() {
        clear();
    }

    void set_cell(int32_t r, int32_t c, bool alive) noexcept {
        data[r][c] = alive;
    }

    void set_cell_group(const CellGroup& group, bool alive) noexcept {
        for (const auto& cellPos : group) {
            set_cell(cellPos.first, cellPos.second, alive);
        }
    }

    void clear() noexcept {
        clear_data();
        clear_next();
    }

    void run() {
        while (true) {
            clear_screen();
            render();
            update();

            std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MILLISEC));
        }
    }
};

template<uint32_t H, uint32_t W>
CellGroup build_cell_group_from_file(const std::string& path) {
    CellGroup group;

    std::ifstream in{ path };
    if (!in.is_open()) {
        throw std::invalid_argument{ "build_cell_group_from_file: cannot open file: " + path };
    }

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream iss{ line };
        uint32_t row;
        uint32_t col;

        iss >> row >> col;
        if (!iss) {
            throw std::runtime_error{ "build_cell_group_from_file: file read failed: " + path };
        }

        if (row >= H) {
            throw std::runtime_error{ "build_cell_group_from_file: row number should be less than: " + std::to_string(H) };
        }

        if (col >= W) {
            throw std::runtime_error{ "build_cell_group_from_file: column number should be less than: " + std::to_string(W) };
        }

        group.emplace_back(row, col);
    }

    return group;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "game_of_life usage: " << argv[0] << " <file_path>\n";
        return 1;
    }

    try {
        GameOfLife<20, 40> gol;
        CellGroup group = build_cell_group_from_file<20, 40>(argv[1]);

        gol.set_cell_group(group, true);
        gol.run();
    }
    catch(const std::exception& e) {
        std::cerr << "error, " << e.what() << "\n";
    }
    
    return 0;
}
