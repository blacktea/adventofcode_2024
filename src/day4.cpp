#include "common_headers.hpp"

/*
Part 1.
The task is to count the word `XMAS` horizontally, vertically, diagonally.
More over, it can be written backwards.

Part 2.
The task is to count two MASin the shape of an X. Each MAS can be written forwards or backwards.

*/


struct RowsReader final
{
    RowsReader(std::ifstream& ifile, size_t numOfRows) : m_ifile{ifile}, m_numOfRows{numOfRows} {
        m_rows.reserve(numOfRows);
    }

    [[nodiscard]] bool readNextLine() {
        auto& rowToBeRead = getNextRow();
        m_ifile >> rowToBeRead;
        if(m_ifile) return true;
        return false;
    }

    [[nodiscard]] const std::vector<std::string>& getRows() const {
        return m_rows;
    }

private:
    std::string& getNextRow() {
        if(m_rows.size() < m_numOfRows) {
            m_rows.emplace_back();
        }
        else {
            std::move_backward(m_rows.rbegin(), std::next(m_rows.rbegin(), m_numOfRows - 1), m_rows.rend());
        }
        return m_rows.back();
    }

private:
    std::ifstream& m_ifile;
    size_t m_numOfRows{};
    std::vector<std::string> m_rows;
};

[[nodiscard]] size_t firstPart(RowsReader& reader)
{
    constexpr auto countArrays = [] (const std::array<char, 4>& input) -> size_t {
        static constexpr std::array<char, 4> forwardArrays = {'X', 'M', 'A', 'S'};
        static constexpr std::array<char, 4> backwardArrays = {'S', 'A', 'M', 'X'};
        size_t count{};
        count += (forwardArrays == input);
        count += (backwardArrays == input);
        return count;
    };

    size_t count{};
    while(reader.readNextLine()) {
        const auto& rows = reader.getRows();
        const size_t RowSize = rows.back().size();
        for(size_t i = 0; i < RowSize; ++i) {
            if(i + 3 < RowSize) {
                // straightforward
                if(rows.back().compare(i, 4, "XMAS") == 0) ++count;

                // backward
                if(rows.back().compare(i, 4, "SAMX") == 0) ++count;
            }

            if(rows.size() == 4) {
                // vertical                
                std::array<char, 4> vertical{rows[3][i], rows[2][i], rows[1][i], rows[0][i]};
                count += countArrays(vertical);

                if(i + 3 < RowSize) {
                    std::array<char, 4> bottomUpForwardDiagonal{rows[3][i], rows[2][i + 1], rows[1][i + 2], rows[0][i + 3]};
                    count += countArrays(bottomUpForwardDiagonal);

                    std::array<char, 4> topDownForwardDiagonal{rows[0][i], rows[1][i + 1], rows[2][i + 2], rows[3][i + 3]};
                    count += countArrays(topDownForwardDiagonal);
                }
            }
        }
    }
    return count;
}


[[nodiscard]] size_t secondPart(RowsReader& reader)
{
    constexpr auto countArrays = [] (const std::array<char, 3>& input) -> size_t {
        static constexpr std::array forwardArrays = {'M', 'A', 'S'};
        static constexpr std::array backwardArrays = {'S', 'A', 'M'};
        size_t count{};
        count += (forwardArrays == input);
        count += (backwardArrays == input);
        return count;
    };

    size_t count{};
    while(reader.readNextLine()) {
        const auto& rows = reader.getRows();
        const size_t RowSize = rows.back().size();
        if(rows.size() < 3) continue;

        for(size_t i = 0; i + 2 < RowSize; ++i) {
            std::array<char, 3> forwardDiagonal{rows[2][i], rows[1][i + 1], rows[0][i + 2]};
            std::array<char, 3> backwardDiagonal{rows[2][i + 2], rows[1][i + 1], rows[0][i]};

            if(countArrays(forwardDiagonal) > 0 && countArrays(backwardDiagonal) > 0) {
                ++count;
            }
        }
    }
    return count;
}

void printHelp()
{
    std::cerr << "\nUsage:\n"
    << "The program requires 2 args: (part1, part2) and the path to the file."
    << "\nFor example, ./day4 part1 data/day4.txt";
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

    std::ifstream ifile(argv[2]);
    if(!ifile) {
        std::cerr << "\nFile cannot be open";
        return 1;
    }

    if(task == "part1") {
        RowsReader reader(ifile, 4);
        std::cout << firstPart(reader);
    }
    else {
        RowsReader reader(ifile, 3);
        std::cout << secondPart(reader);
    }
    return 0;
}