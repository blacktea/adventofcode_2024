#pragma once

#include <fstream>
#include <iostream>
#include <string_view>
#include <stdexcept>

struct DummyLineParser
{
    const std::string& operator()(const std::string&s) const { return s; } 
};

// TODO: Add concepts once I update compiler.
template<typename Iterator, typename Parser = DummyLineParser>
void readInput(const std::string_view filename, Iterator inserter, Parser lineParser = DummyLineParser{}) {
    std::ifstream ifile(filename.data());
    if(!ifile) {
        std::cerr << "\nFile cannot be open: " << filename;
        std::exit(1);
    }

    std::string line;
    while(std::getline(ifile, line)) {
        *inserter++ = lineParser(line);
    }
}