#include "common_headers.hpp"

using Heights = std::vector<size_t>;

struct LocksAndKeys final
{
    size_t lockHeight{7};
    std::vector<Heights> locks;
    std::vector<Heights> keys;
};


[[nodiscard]] size_t solveFirstPart(const LocksAndKeys& locksAndKeys)
{
    size_t count{};
    for(const auto& lock : locksAndKeys.locks) {
        for(const auto& key : locksAndKeys.keys) {
            if(lock.size() != key.size()) {
                throw std::logic_error{"sizes of lock and key differ"};
            }
            bool fit{true};
            for(size_t i = 0; i < lock.size(); ++i) {
                fit = fit && (lock[i] + key[i] <= locksAndKeys.lockHeight);
            }
            count += fit;
        }
    }
    return count;
}

[[nodiscard]] size_t parseHeights(const std::vector<std::string>& inputVec, size_t beginInputId, Heights& heights)
{
    size_t inputId{beginInputId};
    heights.reserve(inputVec[0].size());
    for(size_t column = 0; column < inputVec[0].size(); ++column) {
        auto row = inputId;
        size_t height{};
        while(row < inputVec.size() && !inputVec[row].empty()) {
            height += (inputVec[row][column] == '#');
            ++row;
        }
        heights.push_back(height);
    }

    while(inputId < inputVec.size() && !inputVec[inputId].empty()) ++inputId;
    ++inputId; //< skip empty line
    return inputId;
}

[[nodiscard]] size_t parseLocks(const std::vector<std::string>& inputVec, size_t beginInputId, LocksAndKeys& locksAndKeys)
{
    size_t inputId{beginInputId};
    while(inputId < inputVec.size() && inputVec[inputId][0] == '#') {
        locksAndKeys.locks.emplace_back();
        inputId = parseHeights(inputVec, inputId, locksAndKeys.locks.back());

        if(locksAndKeys.locks.back().empty()) {
            locksAndKeys.locks.back().pop_back();
        }
    }
    return inputId;
}

[[nodiscard]] size_t parseKeys(const std::vector<std::string>& inputVec, size_t beginInputId, LocksAndKeys& locksAndKeys)
{
    size_t inputId{beginInputId};
    while(inputId < inputVec.size() && inputVec[inputId][0] == '.') {
        locksAndKeys.keys.emplace_back();
        inputId = parseHeights(inputVec, inputId, locksAndKeys.keys.back());

        if(locksAndKeys.keys.back().empty()) {
            locksAndKeys.keys.back().pop_back();
        }
    }
    return inputId;
}

[[nodiscard]] LocksAndKeys parseInput(const std::vector<std::string>& inputVec)
{
    LocksAndKeys locksAndKeys;
    size_t inputId{};
    while(inputId < inputVec.size()) {
        inputId = parseLocks(inputVec, inputId, locksAndKeys);
        inputId = parseKeys(inputVec, inputId, locksAndKeys);
    }

    return locksAndKeys;
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

    auto locksAndKeys = parseInput(inputVec);
    
    if(task == "part1") {
        std::cout << solveFirstPart(locksAndKeys);
    }
    else {
        // std::cout << solveSecondPart(program);       
    }

    return 0;
}