#include "common_headers.hpp"
#include "Point.hpp"
#include "utils/input_parser.hpp"


[[nodiscard]] bool isAntinode(std::vector<std::string>&map, Position position) {
    return map[position.x][position.y] == '#';
}

void bookSlotForAntinode(std::vector<std::string>& map, Position position) {
    map[position.x][position.y] = '#';
}

[[nodiscard]] size_t solveFirstPart(std::vector<std::string>& map, const std::unordered_map<char, std::vector<Position>>& antennas) {
    const size_t rows{map.size()};
    const size_t cols{map.at(0).size()};

    size_t antinodes{};
    auto countAntinode = [&](Position position) {
        if(position.inRange(rows, cols) && !isAntinode(map, position)) {
            bookSlotForAntinode(map, position);
            ++antinodes;
        }
    };

    for(const auto& [antena, places] : antennas) {
        for(size_t i = 0; i < places.size(); ++i) {
            const Position originPlace{places[i]};
            for(size_t j = i + 1; j < places.size(); ++j) {
                const int diffX{(originPlace.x - places[j].x) * 2};
                const int diffY{(originPlace.y - places[j].y) * 2};

                const Position posA{places[j].x + diffX, places[j].y + diffY};
                countAntinode(posA);
                const Position posB{originPlace.x - diffX, originPlace.y - diffY};
                countAntinode(posB);
            }
        }
    }
    return antinodes;
}

[[nodiscard]] size_t solveSecondPart(std::vector<std::string>& map, const std::unordered_map<char, std::vector<Position>>& antennas) {
    const size_t rows{map.size()};
    const size_t cols{map.at(0).size()};

    size_t antinodes{};
    auto countAntinode = [&](Position posA, Position posB) {
        const int diffX{posA.x - posB.x};
        const int diffY{posA.y - posB.y};
        Position newPos{posA.x + diffX, posA.y + diffY};

        if(!isAntinode(map, posA)) {
                bookSlotForAntinode(map, posA);
                ++antinodes;
        }

        while(newPos.inRange(rows, cols)) {
            if(!isAntinode(map, newPos)) {
                bookSlotForAntinode(map, newPos);
                ++antinodes;
            }
            newPos.x += diffX;
            newPos.y += diffY;
        }
    };

    for(const auto& [antena, places] : antennas) {
        for(size_t i = 0; i < places.size(); ++i) {
            const Position originPlace{places[i]};
            for(size_t j = i + 1; j < places.size(); ++j) {
                countAntinode(originPlace, places[j]);
                countAntinode(places[j], originPlace);
            }
        }
    }
    return antinodes;
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
    
    std::vector<std::string> map;
    readInput(argv[2], std::back_inserter(map));

    std::unordered_map<char, std::vector<Position>> antennas;
    for(size_t id = 0; id < map.size(); ++id) {
        const auto& line = map[id];
        for(size_t cid = 0; cid < line.size(); ++cid) {
            if(line[cid] == '.') continue;
            antennas[line[cid]].emplace_back(static_cast<int>(id), static_cast<int>(cid));
        }
    }

    if(task == "part1") {
        std::cout << solveFirstPart(map, antennas);
    }
    else {
        std::cout << solveSecondPart(map, antennas);
    }

    for(auto& r : map) {
        std::cerr << "\n";
        for(auto ch : r) {
            std::cerr << " " << ch;
        }
    }

    return 0;
}