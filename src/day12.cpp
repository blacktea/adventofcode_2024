#include "common_headers.hpp"
#include <array>
#include <set>
#include <stack>

constexpr std::array<int,5> offsets{0, 1, 0, -1, 0};

using CoordinateMap = std::set<std::pair<int, int>>;

[[nodiscard]] size_t calculatePerimeter(const std::vector<std::string>& map, int row, int col) {
    size_t perimeter{};
    for(int i = 0; i < 4; ++i) {
        const int newRow{row + offsets[i]};
        const int newCol{col + offsets[i + 1]};
        if(newRow < 0 || newCol < 0 || newRow >= map.size() || newCol >= map[0].size()) {
            ++perimeter;
        }
        else {
            perimeter += map[row][col] != map[newRow][newCol];
        }
    }
    return perimeter;
}

[[nodiscard]] size_t calculateCorner(const std::vector<std::string>& map, int row, int col) {
    size_t counter{};
    const char plot{map[row][col]};
    // top left outer
    if((row - 1 < 0 || plot != map[row - 1][col]) && (col - 1 < 0 || map[row][col - 1] != plot)) {
        ++counter;
    }
    // top right outer
    if((row - 1 < 0 || plot != map[row - 1][col]) && (col + 1 >= map[0].size() || map[row][col + 1] != plot)) {
        ++counter;
    }
    // bottom left
    if((row + 1 >= map.size() || map[row + 1][col] != plot) && (col - 1 < 0 || map[row][col - 1] != plot)) {
        ++counter;
    }
    // bottom right
    if((row + 1 >= map.size() || map[row + 1][col] != plot) && (col + 1 >= map[0].size() || map[row][col + 1] != plot)) {
        ++counter;
    }

    // top left inner
    if(row - 1 >= 0 && map[row - 1][col] == plot && col - 1 >= 0 && map[row][col - 1] == plot && map[row - 1][col - 1] != plot) {
        ++counter;
    }

    // top right inner
    if(row - 1 >= 0 && map[row - 1][col] == plot && col + 1 < map[0].size() && map[row][col + 1] == plot && map[row - 1][col + 1] != plot) {
        ++counter;
    }

    // bottom left inner
    if(row + 1 < map.size() && map[row + 1][col] == plot && col - 1 >= 0 && map[row][col - 1] == plot && map[row + 1][col - 1] != plot) {
        ++counter;
    }
    // bottom right inner
    if(row + 1 < map.size() && map[row + 1][col] == plot && col + 1 < map[0].size() && map[row][col + 1] == plot && map[row + 1][col + 1] != plot) {
        ++counter;
    }

    return counter;
}


template<typename Handler>
void traverse(const std::vector<std::string>& map, int row, int col, CoordinateMap& visited, Handler& handler) {
    
    const char plot{map[row][col]};

    std::stack<std::pair<int, int>> dfs;
    dfs.emplace(row, col);

    while(!dfs.empty()) {
        const auto [r, c] = dfs.top();
        dfs.pop();
        if(visited.emplace(std::pair{r, c}).second == false) continue;

        handler(map, r, c);

        for(int i = 0; i < 4; ++i) {
            const int newRow{r + offsets[i]};
            const int newCol{c + offsets[i + 1]};
            if(newRow < 0 || newCol < 0 || newRow >= map.size() || newCol >= map[0].size()) {
                continue;
            }
            else if(plot == map[newRow][newCol]) {
                dfs.emplace(newRow, newCol);
            }
        }
    }
}

[[nodiscard]] size_t solveFirstPart(const std::vector<std::string>& map) {
    size_t totalPrice{};
    CoordinateMap visited;
    

    for(int row = 0; row < map.size(); ++row) {
        for(int col = 0; col < map[0].size(); ++col) {
            if(visited.count(std::pair(row, col))) continue;

            size_t area{};
            size_t perimeter{};
            auto calculator = [&](const std::vector<std::string>& map, int row, int col) mutable {
                ++area;
                perimeter += calculatePerimeter(map, row, col);
            };

            traverse(map, row, col, visited, calculator);
            totalPrice += area * perimeter;
        }
    }
    return totalPrice;
}

[[nodiscard]] size_t solveSecondPart(const std::vector<std::string>& map) {
    size_t totalPrice{};
    CoordinateMap visited;

    for(int row = 0; row < map.size(); ++row) {
        for(int col = 0; col < map[0].size(); ++col) {
            if(visited.count(std::pair(row, col))) continue;

            size_t area{};
            size_t perimeter{};
            auto calculator = [&](const std::vector<std::string>& map, int row, int col) mutable {
                ++area;
                perimeter += calculateCorner(map, row, col);
            };
            traverse(map, row, col, visited, calculator);
            totalPrice += area * perimeter;
        }
    }

    return totalPrice;
}


void printHelp()
{
    std::cerr << "\nUsage:\n"
    << "The program requires 2 args: (part1, part2) and the path to the file."
    << "\nFor example, ./day7 part1 data/day7.txt";
}

int main(int argc, char* argv[])
{
    if(argc != 3) {
        printHelp();
        return 1;
    }

    std::string_view task{argv[1]};
    if(task != "part1" && task != "part2") {
        std::cerr << "\nfirst arg can be either `part1` or `part2`\n";
        printHelp();
        return 1;
    }
    
    std::vector<std::string> inputVec;
    readInput(argv[2], std::back_inserter(inputVec));

    if(task == "part1") {
        std::cout << solveFirstPart(inputVec);
    }
    else {
        std::cout << solveSecondPart(inputVec);
    }

    return 0;
}