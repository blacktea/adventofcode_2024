#include <algorithm>
#include <iostream>
#include <fstream>
#include <charconv>
#include <vector>
#include <stdexcept>

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

[[nodiscard]] constexpr bool isLevelsSafe(int largelLevel, int smallerLevel) noexcept
{
    const auto diff{largelLevel - smallerLevel};
    return 1 <= diff && diff <= 3;
}


[[nodiscard]] bool isAscendingSafe(const std::vector<int>& levels) noexcept
{
    return std::adjacent_find(levels.begin(), levels.end(), [](int smallerLevel, int largerLevel) {
        return !isLevelsSafe(largerLevel, smallerLevel);
    }) == levels.end();
}

[[nodiscard]] bool isDescendingSafe(const std::vector<int>& levels) noexcept
{
    return std::adjacent_find(levels.rbegin(), levels.rend(), [](int smallerLevel, int largerLevel) {
        return !isLevelsSafe(largerLevel, smallerLevel);
    }) == levels.rend();
}

[[nodiscard]] bool isSafe(const std::vector<int>& levels) noexcept
{
    if(levels.front() < levels.back()) {
        return isAscendingSafe(levels);
    }
    return isDescendingSafe(levels);
}
} //< anonymous namespace

int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cerr << "\nUsage:\n"
        << "The program requires the path to the file.";
        return 1;
    }

    std::ifstream ifile(argv[1]);
    if(!ifile) {
        std::cerr << "\nFile cannot be open";
        return 1;
    }

    size_t count{};
    std::string report;
    std::vector<int> levels;
    while(std::getline(ifile, report)) {
        // std::cerr << "\nline  " << report;
        levels.clear();
        parseLevels(report, levels);
        count += isSafe(levels);
    }
    std::cout << count;

    return 0;
}