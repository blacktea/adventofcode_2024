#include "common_headers.hpp"
#include <queue>
#include <set>


[[nodiscard]] size_t solveFirstPart(const std::vector<std::string>& map)
{
    size_t totalScore{};
    int dirs[] = {0, 1, 0, -1, 0};
    std::queue<std::pair<int, int>> bfs;

    std::set<std::pair<int, int>> uniqueHeights;
    for(size_t r = 0; r < map.size(); ++r) {
        for(size_t c = 0; c < map[0].size(); ++c) {
            if(map[r][c] != '0') continue;
            uniqueHeights.clear();
            bfs.emplace(r, c);
            while(!bfs.empty()) {
                const auto [row, col] = bfs.front();
                bfs.pop();
                if(map[row][col] == '9') {
                    uniqueHeights.emplace(row, col);
                }
                else {
                    for(size_t idir = 0; idir < 4; ++idir) {
                        const int new_row{row + dirs[idir]};
                        const int new_col{col + dirs[idir + 1]};

                        if(new_row < 0 || new_row >= map.size() || new_col < 0 || new_col >= map[0].size()) {
                            continue;
                        }
                        if(map[new_row][new_col] - map[row][col] == 1) {
                            bfs.emplace(new_row, new_col);
                        }
                    }
                }
            }
            totalScore += uniqueHeights.size();
        }
    }
    return totalScore;
}


[[nodiscard]] size_t solveSecondPart(const std::vector<std::string>& map)
{
    size_t totalRating{};
    int dirs[] = {0, 1, 0, -1, 0};
    std::queue<std::pair<int, int>> bfs;

    for(size_t r = 0; r < map.size(); ++r) {
        for(size_t c = 0; c < map[0].size(); ++c) {
            if(map[r][c] != '0') continue;

            bfs.emplace(r, c);
            while(!bfs.empty()) {
                const auto [row, col] = bfs.front();
                bfs.pop();
                if(map[row][col] == '9') {
                    ++totalRating;
                }
                else {
                    for(size_t idir = 0; idir < 4; ++idir) {
                        const int new_row{row + dirs[idir]};
                        const int new_col{col + dirs[idir + 1]};

                        if(new_row < 0 || new_row >= map.size() || new_col < 0 || new_col >= map[0].size()) {
                            continue;
                        }
                        if(map[new_row][new_col] - map[row][col] == 1) {
                            bfs.emplace(new_row, new_col);
                        }
                    }
                }
            }
        }
    }
    return totalRating;
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