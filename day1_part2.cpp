#include <algorithm>
#include <iostream>
#include <fstream>
#include <unordered_map>

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
    uint64_t score{};
    std::unordered_map<int, int> leftNumToFrequency, rightNumToFrequency; 

    int leftNum{}, rightNum{};
    std::string line;
    while((ifile >> leftNum >> rightNum)) {
        ++rightNumToFrequency[rightNum];
        score += (rightNum * leftNumToFrequency[rightNum]);
        score += (leftNum * rightNumToFrequency[leftNum]);
        ++leftNumToFrequency[leftNum];
    }
    std::cout << score;
    return 0;
}