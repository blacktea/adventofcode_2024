#include "common_headers.hpp"
#include <regex>

/*
Part 1.
The task is to multiply values. The values are stored in the following pattern mul(123, 321).
However, there might be other options of it such as mul(4*, mul(6,9!, ?(12,34), or mul ( 2 , 4 ),
which should be skipped.

Part 2.
More instructions are introduced: do() and don't(). The do() instruction enables multiplication.
The don't() disables it.

The task becomes simpler when using regex.
*/

int64_t doMul(const std::string& firstValStr, const std::string& secValStr) {
    int firstVal{};
    if(std::from_chars(&firstValStr[0], &firstValStr[firstValStr.size()], firstVal).ec != std::errc()) {
        throw std::logic_error{"Failed to parse the first value"};
    }

    int secVal{};
    if(std::from_chars(&secValStr[0], &secValStr[secValStr.size()], secVal).ec != std::errc()) {
        throw std::logic_error{"Failed to parse the second value"};
    }

    return firstVal * secVal;
}

class PartTwo
{
public:
    int64_t sovle(const std::string& input) {
        int64_t result{};
        
        const std::regex regexPattern(R"(mul\(([0-9]{1,3}),([0-9]{1,3})\)|do\(\)|don\'t\(\))");
        auto matchesBegin = std::sregex_iterator(input.begin(), input.end(), regexPattern);
        auto matchesEnd = std::sregex_iterator();

        for (auto it = matchesBegin; it != matchesEnd; ++it) {
            std::smatch match = *it;
            const std::string cmd{match[0]};
            if(cmd == "do()") {
                mulEnabled = true;
            }
            else if(cmd == "don't()") {
                mulEnabled = false;
            }
            else if(mulEnabled) {                
                std::string firstValStr{match.str(1)};
                std::string secValStr{match.str(2)};
                result += doMul(firstValStr, secValStr);
            }
        }
        return result;
    }
private:
    bool mulEnabled{true};
};

class PartOne
{
public:
    [[nodiscard]] int64_t solve(const std::string& input) {
        int64_t result{};
        
        const std::regex regexPattern(R"(mul\(([0-9]{1,3}),([0-9]{1,3})\))");
        auto matchesBegin = std::sregex_iterator(input.begin(), input.end(), regexPattern);
        auto matchesEnd = std::sregex_iterator();

        for (auto it = matchesBegin; it != matchesEnd; ++it) {
            std::smatch match = *it;
            std::string firstValStr{match.str(1)};
            std::string secValStr{match.str(2)};
            result += doMul(firstValStr, secValStr);
        }
        return result;
    }
};

void printHelp()
{
    std::cerr << "\nUsage:\n"
    << "The program requires 2 args: (part1, part2) and the path to the file."
    << "\nFor example, ./day3 part1 data/day3.txt";
}

int main(int argc, char *argv[]) {
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
    PartOne part1;
    PartTwo part2;
    
    try {
        int64_t result{};
        std::string line;
        while((ifile >> line)) {
            if(task == "part1") {
                result += part1.solve(line);
            }
            else {
                result += part2.sovle(line);
            }
        }
        std::cout << result;
    }
    catch(const std::exception& ex) {
        std::cerr << "\nexception: " << ex.what();
        return 1;
    }
    return 0;
}