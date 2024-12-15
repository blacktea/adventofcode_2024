#include "common_headers.hpp"
#include "Position.hpp"

#include <cmath>
#include <regex>

struct RobotInfo final
{
    Position position{};

    int velocityX{};
    int velocityY{};
};

[[nodiscard]] size_t solveFirstPart(const std::vector<RobotInfo>& inputVec)
{
    constexpr size_t width{101};
    constexpr size_t height{103};
    constexpr size_t midVertical{(width / 2) + 1};
    constexpr size_t midHorizontal{(height / 2) + 1};
    std::array<int, 4> counters{};
    std::vector<std::vector<int>> map(height, std::vector<int>(width));

    for(const auto& robot : inputVec) {
        int64_t newX{robot.position.x + robot.velocityX * 100};
        int64_t newY{robot.position.y + robot.velocityY * 100};
        if(newX < 0) {
            newX = (width - (std::abs(newX) % width)) % width;
        }
        else {
            newX = newX % width;
        }
        if(newY < 0) {
            newY = (height - (std::abs(newY) % height)) % height;
        }
        else {
            newY = newY % height;
        }
        if(newX != midVertical-1 && newY != midHorizontal-1) {
            counters[2 * (newY / midHorizontal) + newX / midVertical]++;
        }
        map[newY][newX]++;
    }

    return std::accumulate(counters.begin(), counters.end(), size_t{1}, std::multiplies<>{});
}

[[nodiscard]] double EuclideanDistanceFromOrigin(double x, double y) noexcept
{
    return std::sqrt(x * x + y * y);
}

[[nodiscard]] double calculateVariance(const std::vector<std::vector<int>>& matrix) noexcept
{
    const size_t midVertical{(matrix[0].size() / 2) + 1};
    const size_t midHorizontal{(matrix.size() / 2) + 1};

    double positionCount{};
    double meanDistance{};
    for(size_t i = 0; i < matrix.size(); ++i) {
        for(size_t j = 0; j < matrix[0].size(); ++j) {
            positionCount += matrix[i][j] != 0;
            meanDistance += matrix[i][j] * EuclideanDistanceFromOrigin(i, j);
        }
    }
    meanDistance /= positionCount;

    double variance{};
    for(size_t i = 0; i < matrix.size(); ++i) {
        for(size_t j = 0; j < matrix[0].size(); ++j) {
            const auto d{EuclideanDistanceFromOrigin(i, j)};
            variance += matrix[i][j] * (d - meanDistance) * (d - meanDistance);
        }
    }

    return variance;
}


[[nodiscard]] size_t solveSecondPart(const std::vector<RobotInfo>& inputVec) {
    constexpr size_t width{101};
    constexpr size_t height{103};
    std::vector<std::vector<int>> map(height, std::vector<int>(width));
    
    std::unordered_map<double, size_t> varianceFreq;
    
    double minVariance{std::numeric_limits<double>::max()};
    size_t minVarianceId{};

    size_t iteration{};
    while(++iteration < 10000) {
        map.clear();
        map.resize(height, std::vector<int>(width));

        for(const auto& robot : inputVec) {
            int64_t newX{robot.position.x + robot.velocityX * iteration};
            int64_t newY{robot.position.y + robot.velocityY * iteration};
            if(newX < 0) {
                newX = (width - (std::abs(newX) % width)) % width;
            }
            else {
                newX = newX % width;
            }
            if(newY < 0) {
                newY = (height - (std::abs(newY) % height)) % height;
            }
            else {
                newY = newY % height;
            }
            map[newY][newX] = 1;
        }
        const double variance{calculateVariance(map)};
        varianceFreq[variance]++;
        if(variance < minVariance) {
            minVariance = variance;
            minVarianceId = iteration;
        }
    }

    return minVarianceId;
}


[[nodiscard]] RobotInfo parseRobotInfo(const std::string& input) {
    RobotInfo info;

    // Define the regex pattern to match key-value pairs like "p=0,4" or "v=3,-3"
    std::regex pattern(R"((\w+)=(-?\d+),(-?\d+))");
    std::smatch matches;

    // Use an iterator to find all matches in the input string
    auto begin = std::sregex_iterator(input.begin(), input.end(), pattern);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        matches = *it;

        // Extract key and coordinates
        const std::string key = matches[1];
        const int x{std::stoi(matches[2])};
        const int y{std::stoi(matches[3])};

        // Store the parsed values in the map
        if(key == "p") {
            info.position = Position{x, y};
        }
        else if(key == "v") {
            info.velocityX = x;
            info.velocityY = y;
        }
        else {
            throw std::logic_error{"unknown input: " + input};
        }
    }
    return info;
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
    
    std::vector<RobotInfo> inputVec;
    readInput(argv[2], std::back_inserter(inputVec), parseRobotInfo);
    
    if(task == "part1") {
        std::cout << solveFirstPart(inputVec);
    }
    else {
        std::cout << solveSecondPart(inputVec);
    }

    return 0;
}