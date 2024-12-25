#include "common_headers.hpp"

#include <sstream>
#include <unordered_set>

[[nodiscard]] size_t solveFirstPart(const std::unordered_set<std::string> &patterns, const std::vector<std::string>& designs)
{
    size_t count{};
    std::string search;
    std::vector<int> match;
    for(const auto& design : designs) {
        match.clear();
        match.resize(design.size() + 1, 0);
        match[0] = 1;

        for(size_t i = 0; i < design.size(); ++i) {
            search.clear();
            for(int j = i; j >= 0 && match[i + 1] == 0; --j) {
                const auto substr =  design.substr(j, i - j + 1);
                match[i + 1] = match[j] && (patterns.count(substr) > 0);
            }
        }
        count += (match.back() == 1);
    }
    std::cerr << "\n";
    return count;
}

[[nodiscard]] size_t solveSecondPart(const std::unordered_set<std::string> &patterns, const std::vector<std::string>& designs)
{
    size_t count{};
    std::string search;
    std::vector<size_t> match;
    for(const auto& design : designs) {
        match.clear();
        match.resize(design.size() + 1, 0);
        match[0] = 1;

        for(size_t i = 0; i < design.size(); ++i) {
            search.clear();
            for(int j = i; j >= 0; --j) {
                const auto substr =  design.substr(j, i - j + 1);
                if(match[j] != 0 && (patterns.count(substr) > 0)) {
                    match[i + 1] += match[j];
                }
            }
        }

        count += match.back();
    }
    std::cerr << "\n";
    return count;
}




[[nodiscard]] std::unordered_set<std::string> parsePatterns(const std::string& pattern)
{
    std::unordered_set<std::string> patterns;

    std::stringstream ss{pattern};
    std::string line;
    while((ss >> line)) {
        if(line.back() == ',') {
            line.pop_back();
        }
        patterns.emplace(line);
    }
    return patterns;
}

[[nodiscard]] std::pair<std::unordered_set<std::string>, std::vector<std::string>> parseInput(const std::vector<std::string>& inputVec)
{
    auto patterns = parsePatterns(inputVec.at(0));
    std::vector<std::string> designs(std::next(inputVec.begin(), 2), inputVec.end());
    return {std::move(patterns), std::move(designs)};
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


    auto [patterns, designs] = parseInput(inputVec);

    if(task == "part1") {
        std::cout << solveFirstPart(patterns, designs);
    }
    else {
        std::cout << solveSecondPart(patterns, designs);
    }

    return 0;
}