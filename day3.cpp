#include "common_headers.hpp"
#include <regex>

/*
The task is to multiply values. The values are stored in the following pattern mul(123, 321).
However, there might be other options of it such as mul(4*, mul(6,9!, ?(12,34), or mul ( 2 , 4 ),
which should be skipped.

The task becomes simpler when using regex.
*/


int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cerr << "\nUsage:\n"
        << "The program requires the path to the file.";
        return 1;
    }
    int64_t result{};
    std::ifstream ifile(argv[1]);
    if(!ifile) {
        std::cerr << "\nFile cannot be open";
        return 1;
    }

    const std::regex regexPattern(R"(mul\(([0-9]{1,3}),([0-9]{1,3})\))");

    std::string line;
    while((ifile >> line)) {
        auto matchesBegin = std::sregex_iterator(line.begin(), line.end(), regexPattern);
        auto matchesEnd = std::sregex_iterator();

        // Iterate through all matches
        for (auto it = matchesBegin; it != matchesEnd; ++it) {
            std::smatch match = *it;
            std::string firstValStr{match.str(1)};
            int firstVal{};
            if(std::from_chars(&firstValStr[0], &firstValStr[firstValStr.size()], firstVal).ec != std::errc()) {
                std::cerr << "\nFailed to parse the first value: " << match[0];
                return 1;
            }

            std::string secValStr{match.str(2)};
            int secVal{};
            if(std::from_chars(&secValStr[0], &secValStr[secValStr.size()], secVal).ec != std::errc()) {
                std::cerr << "\nFailed to parse the second value: " << match[0];
                return 1;
            }

            result += (firstVal * secVal);
        }
    }
    std::cout << result;
    return 0;
}