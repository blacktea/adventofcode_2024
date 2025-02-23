#include "common_headers.hpp"
#include "utils/numeric_algorithm.hpp"

#include <cmath>
#include <cstdint>
#include <sstream>


using HashTable = std::unordered_map<size_t, std::unordered_map<size_t, uint64_t>>;
size_t calculateStones(size_t maxBlinks, size_t blinks, uint64_t num, HashTable& cache) { 

    if(cache[num].count(blinks)) {
        return cache[num][blinks];
    }
    if(blinks == maxBlinks) {
        return 1;
    }
    if(num == 0) {
        const auto stones = calculateStones(maxBlinks, blinks + 1, 1, cache);
        cache[num][blinks] = stones ;
    }
    else if(auto countOfDigits = numOfDigits(num); countOfDigits % 2 == 0) {
        const auto [num1, num2] = splitNum(num, countOfDigits);
        
        const auto stones1 = calculateStones(maxBlinks, blinks + 1, num1, cache);
        const auto stones2 = calculateStones(maxBlinks, blinks + 1, num2, cache);
        cache[num][blinks] = stones1 + stones2;
    }
    else {
        const auto stones = calculateStones(maxBlinks, blinks + 1, num * 2024, cache);
        cache[num][blinks] = stones;
    }

    return cache[num][blinks];
}

size_t solveFirstPart(const std::vector<uint64_t>& stones, size_t maxBlinks) { 
    HashTable cache;
    size_t stoneCounter{};
    for(auto it = stones.rbegin(); it != stones.rend(); ++it) {
        stoneCounter += calculateStones(maxBlinks, 0, *it, cache);
    }
    return stoneCounter;
}

std::vector<uint64_t> lineParser(const std::string& line) {
    std::stringstream ss(line);
    uint64_t num{};
    std::vector<uint64_t> nums;
    while ((ss >> num))
    {
        nums.push_back(num);
    }
    return nums;
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
    
    std::vector<std::vector<uint64_t>> inputVec;
    readInput(argv[2], std::back_inserter(inputVec), lineParser);

    if(task == "part1") {
        const size_t maxBlinks{25};
        std::cout << solveFirstPart(inputVec.at(0), maxBlinks);
    }
    else {
        const size_t maxBlinks{75};
        std::cout << solveFirstPart(inputVec.at(0), maxBlinks);
    }

    return 0;
}