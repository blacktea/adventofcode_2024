#include <algorithm>
#include <iostream>
#include <fstream>
#include <charconv>
#include <vector>
#include <stdexcept>
#include <span>

namespace {
void parseLevels(const std::string& report, std::vector<int>& out)
{
    if(report.empty()) return;

    const char *begin = &report[0];
    const char * const end = &report[report.size()];
    std::from_chars_result result{};
    int val{};
    while(begin != end) {
        result = std::from_chars(begin, end, val);
        if(result.ec != std::errc()) {
            std::cerr << "\nreport: " << report;
            throw std::logic_error{"Invalid report input string"};
        }
        begin = std::min(result.ptr + 1, end);
        out.push_back(val);
    }
}

template<typename S>
[[nodiscard]] bool safe(std::span<const int> levels, const unsigned tolerate, const S& safer) noexcept
{
    unsigned violations = 0;
    for (size_t i = 1; i < levels.size() && violations <= tolerate; ++i) {
        if (!safer(levels[i-1], levels[i])) {
            ++violations;
            
            if(i + 1 == levels.size()) break;

            
            // We have to figure out which value(i - 1 or i) have to be *removed.
            // This results in the next index position.
            
            // Edge cases:
            // 59 62 65 64 65 <- 65(2nd) element should be removed
            // 36 34 36 33 30 <- 34(2nd) element should be removed

            if(safer(levels[i - 1], levels[i + 1])) {
                ++i;
            }
            // Prev should be deleted because current and next elements are safe.
            else if(safer(levels[i], levels[i + 1])) {
                // However, we must check that current element and prevous elements build safe consequence.
                if(i > 1) {
                    // 1 2 3 10 12. 2 and 10 are not safe. This results in increase in violations.
                    violations += !safer(levels[i - 2], levels[i]);
                }
            }
        }
    }
    return violations <= tolerate;
}

[[nodiscard]] constexpr bool isLevelsSafe(int largelLevel, int smallerLevel) noexcept
{
    const auto diff{largelLevel - smallerLevel};
    return 1 <= diff && diff <= 3;
}

[[nodiscard]] bool isAscendingSafe(std::span<const int> levels, unsigned tolerate) noexcept
{
    return safe(levels, tolerate, [](int smaller, int larger) {
        return isLevelsSafe(larger, smaller);
    });
}

[[nodiscard]] bool isDescendingSafe(std::span<const int> levels, unsigned tolerate) noexcept
{
    return safe(levels, tolerate, [](int larger, int smaller) {
        return isLevelsSafe(larger, smaller);
    });
}

[[nodiscard]] bool isSafe(std::span<const int> levels, unsigned tolerateLevel) noexcept
{
    return isAscendingSafe(levels, tolerateLevel) || isDescendingSafe(levels, tolerateLevel);
}

void printHelp()
{
    std::cerr << "\nUsage:\n"
    << "The program requires 2 args: (part1, part2) and the path to the file."
    << "\nFor example, ./day2 part1 data/day2.txt";
}

} //< anonymous namespace

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

    const unsigned tolerateLevel = task == "part1" ? 0 : 1;

    size_t count{};
    std::string report;
    std::vector<int> levels;
    while(std::getline(ifile, report)) {
        levels.clear();
        parseLevels(report, levels);
        count += isSafe(levels, tolerateLevel);
    }
    std::cout << "\nanswer: " << count;

    return 0;
}