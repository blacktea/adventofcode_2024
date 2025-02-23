#include <algorithm>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char* argv[])
{
    if(argc != 2) {
        std::cerr << "\nUsage:\n"
        << "The programm requires the path to the file.";
        return 1;
    }
    std::string_view filename{argv[1]};
    std::ifstream ifile(filename.data());
    if(!ifile) {
        std::cerr << "\nFile cannot be open";
        return 1;
    }
    std::vector<int> leftNums, rightNums;
    int leftNum{}, rightNum{};
    std::string line;
    while((ifile >> leftNum >> rightNum)) {
        leftNums.push_back(leftNum);
        rightNums.push_back(rightNum);
    }
    if(leftNums.size() != rightNums.size()) {
        std::cerr << "\nThe size columns must be equal";
        return 1;
    }
    std::sort(leftNums.begin(), leftNums.end());
    std::sort(rightNums.begin(), rightNums.end());
 
    uint64_t sum{};
    for(size_t i = 0; i < leftNums.size(); ++i) {
        sum += abs(leftNums[i] - rightNums[i]);
    }
    std::cout << sum;
    return 0;
}