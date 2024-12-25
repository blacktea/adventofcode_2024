#include "common_headers.hpp"
#include "Cursor.hpp"

#include <vector>
#include <set>
#include <unordered_set>


using Graph = std::vector<std::string>;


std::tuple<Cursor::Direction, int, int> findInitialPosition(const Graph& graph)
{
    std::array<char, 4> dirs{'^', '>', 'v', '<'};
    for(size_t row = 0; row < graph.size(); ++row) {
        for(size_t col = 0; col < graph[0].size(); ++col) {
            const auto it = std::find(dirs.begin(), dirs.end(), graph[row][col]);
            if(it != dirs.end()) {
                const Cursor::Direction d{std::distance(dirs.begin(), it)};
                return {d, static_cast<int>(row), static_cast<int>(col)};
            }
        }
    }
    throw std::logic_error{"Initial positon not found"};
}


template<typename TGraph>
[[nodiscard]] constexpr size_t solveFirstPart(TGraph& graph)
{
    auto [direction, row, col] = findInitialPosition(graph);
    Cursor cursor{direction, row, col};
    size_t moves{};
    while(cursor.x() < graph.size() && cursor.y() < graph[0].size() && cursor.x() >= 0 && cursor.y() >= 0) {
        const auto c{graph[cursor.x()][cursor.y()]};
        
        if(c == '#') {
            cursor.stepBack();
            cursor.turnRight();
        }
        else {
            moves += c != 'X';
            graph[cursor.x()][cursor.y()] = 'X';
            cursor.move();
        }
    }
    return moves;
}

template<typename TGraph>
[[nodiscard]] constexpr size_t solveSecondPart(TGraph& graph)
{
    std::set<std::tuple<int, int, Cursor::Direction>> unique;
    enum Dirs : char {
        Up = 1 << 0,
        Down = 1 << 1,
        Left = 1 << 2,
        Right = 1 << 3,
    };

    auto set_char = [](char& c, char new_val) {
        if(c == '.') c = 0;
        c |= new_val;
    };

    auto [direction, row1, col1] = findInitialPosition(graph);
    graph[row1][col1] = '.';
    Cursor cursor{direction, row1, col1};
    size_t moves{};
    while(cursor.x() < graph.size() && cursor.y() < graph[0].size() && cursor.x() >= 0 && cursor.y() >= 0) {
        const auto curChar{graph[cursor.x()][cursor.y()]};
        
        if(curChar == '#') {
            cursor.stepBack();
            cursor.turnRight();
        }
        else {
            switch (cursor.direction())
            {
            case Cursor::Direction::Right:
                if(cursor.x() > 0 && graph[cursor.x() - 1][cursor.y()] == '#') {
                    for(int r = cursor.x(); r < graph.size() && graph[r][cursor.y()] != '#'; ++r) {
                        set_char(graph[r][cursor.y()], Dirs::Up);
                    }
                }
                break;
            case Cursor::Direction::Left:
                if(cursor.x() + 1 < graph.size() && graph[cursor.x() + 1][cursor.y()] == '#') {
                    for(int r = cursor.x(); r >= 0 && graph[r][cursor.y()] != '#'; --r) {
                        set_char(graph[r][cursor.y()], Dirs::Down);
                    }
                }
                break;
            case Cursor::Direction::Up:
                if(cursor.y() > 0 && graph[cursor.x()][cursor.y() - 1] == '#') {
                    for(int col = cursor.y(); col < graph[0].size() && graph[cursor.x()][col] != '#'; ++col) {
                        set_char(graph[cursor.x()][col], Dirs::Left);
                    }
                }
                break;
            case Cursor::Direction::Down:
                if(cursor.y() + 1 < graph[0].size() && graph[cursor.x()][cursor.y() + 1] == '#') {
                    for(int col = cursor.y(); col >= 0 && graph[cursor.x()][col] != '#'; --col) {
                        set_char(graph[cursor.x()][col], Dirs::Right);
                    }
                }
                break; 
            }

            if(curChar != '.') {
                switch (cursor.direction())
                {
                case Cursor::Direction::Right:
                    if(curChar & Dirs::Down) {
                        ++moves;
                        unique.emplace(cursor.x(), cursor.y(), cursor.direction());
                        std::cerr << "\nhit right: " << cursor.x() << " " << cursor.y();
                    }
                    break;
                case Cursor::Direction::Up:
                    if(curChar & Dirs::Right) {
                        ++moves;
                        unique.emplace(cursor.x(), cursor.y(), cursor.direction());

                        std::cerr << "\nhit up: " << cursor.x() << " " << cursor.y();
                    }
                    break;
                case Cursor::Direction::Down:
                    if(curChar & Dirs::Left) {
                        ++moves;
                        unique.emplace(cursor.x(), cursor.y(), cursor.direction());

                        std::cerr << "\nhit down: " << cursor.x() << " " << cursor.y();
                    }
                    break; 
                case Cursor::Direction::Left:
                    if(curChar & Dirs::Up) {
                        ++moves;
                        unique.emplace(cursor.x(), cursor.y(), cursor.direction());

                        std::cerr << "\nhit left: " << cursor.x() << " " << cursor.y();
                    }
                    break;
                }
            }
            
            cursor.move();
        }
    }
    std::cout << "\nmoves " << moves;
    return unique.size();
}
#if 0
template<typename TGraph>
[[nodiscard]] size_t solveSecondPart2(TGraph& graph)
{
    std::vector<std::set<int>> colObst(graph.size());
    std::vector<std::set<int>> rowObst(graph[0].size());

    for(int r = 0; r < graph.size(); ++r) {
        for(int c = 0; c < graph[0].size(); ++c) {
            if(graph[r][c] == '#') {
                colObst[c].emplace(r);
                rowObst[r].emplace(c);
            } 
        }
    }

    constexpr std::array<char, 4> figures{'|', '-', '|', '-'};
    auto [direction, row, col] = findInitialPosition(graph);
    graph[row][col] = '.';
    Cursor cursor{direction, row, col};
    size_t moves{};
    while(cursor.x() < graph.size() && cursor.y() < graph[0].size() && cursor.x() >= 0 && cursor.y() >= 0) {
        const auto curChar{graph[cursor.x()][cursor.y()]};
        
        if(curChar == '#') {
            cursor.stepBack();
            cursor.turnRight();
        }
        else {

            if(curChar != '.') {
                switch (cursor.direction())
                {
                case Cursor::Direction::Right:
                    if(curChar & Dirs::Down) {
                        ++moves;
                        unique.emplace(cursor.x(), cursor.y(), cursor.direction());
                        std::cerr << "\nhit right: " << cursor.x() << " " << cursor.y();
                    }
                    break;
                case Cursor::Direction::Up:
                    if(curChar & Dirs::Right) {
                        ++moves;
                        unique.emplace(cursor.x(), cursor.y(), cursor.direction());

                        std::cerr << "\nhit up: " << cursor.x() << " " << cursor.y();
                    }
                    break;
                case Cursor::Direction::Down:
                    if(curChar & Dirs::Left) {
                        ++moves;
                        unique.emplace(cursor.x(), cursor.y(), cursor.direction());

                        std::cerr << "\nhit down: " << cursor.x() << " " << cursor.y();
                    }
                    break; 
                case Cursor::Direction::Left:
                    if(curChar & Dirs::Up) {
                        ++moves;
                        unique.emplace(cursor.x(), cursor.y(), cursor.direction());

                        std::cerr << "\nhit left: " << cursor.x() << " " << cursor.y();
                    }
                    break;
                }
            }
            
            cursor.move();
        }
    }
    std::cout << "\nmoves " << moves;
    return unique.size();
}
#endif

void printHelp()
{
    std::cerr << "\nUsage:\n"
    << "The program requires 2 args: (part1, part2) and the path to the file."
    << "\nFor example, ./day5 part1 data/day5.txt";
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

    std::ifstream ifile(argv[2]);
    if(!ifile) {
        std::cerr << "\nFile cannot be open";
        return 1;
    }

    size_t ans{};
    Graph graph;

    std::string line;
    while((ifile >> line)) {
        // std::cerr << "\n" << line;
        graph.push_back(std::move(line));
    }

    if(task == "part1") {
        std::cout << solveFirstPart(graph);
    }
    else {
        const auto s = solveSecondPart(graph);
        std::cout << "\n" << s;
    }

    // for(auto& row : graph) {
    //     std::cout << '\n';
    //     for(auto ch : row) {
    //         std::cout << " " << ch;
    //     }
    // }

    
    // std::cout << ans;
    return 0;
}