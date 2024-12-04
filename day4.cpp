#include "common_headers.hpp"

/*
Part 1.
The task is to count the word `XMAS` horizontally, vertically, diagonally.
More over, it can be written backwards.

*/


struct RowsReader final
{
    RowsReader(std::ifstream& ifile) : m_ifile{ifile} {
        m_rows.reserve(4);
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
        if(m_rows.size() < 4) {
            m_rows.emplace_back();
        }
        else {
            std::move_backward(m_rows.rbegin(), std::next(m_rows.rbegin(), 3), m_rows.rend());
        }
        return m_rows.back();
    }

private:
    std::ifstream& m_ifile;
    std::vector<std::string> m_rows;
};

size_t find(RowsReader& reader)
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
        for(size_t i = 0; i < rows.back().size(); ++i) {
            if(i + 3 < rows.back().size()) {
                // straightforward
                if(rows.back().compare(i, 4, "XMAS") == 0) ++count;

                // backward
                if(rows.back().compare(i, 4, "SAMX") == 0) ++count;
            }

            if(rows.size() == 4) {
                // vertical                
                std::array<char, 4> vertical{rows[3][i], rows[2][i], rows[1][i], rows[0][i]};
                count += countArrays(vertical);

                if(i + 3 < rows.back().size()) {
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


int main(int argc, char* argv[])
{
    if(argc != 2) {
        std::cerr << "\nUsage:\n"
        << "The programm requires the path to the file.";
        return 1;
    }
    std::ifstream ifile(argv[1]);
    if(!ifile) {
        std::cerr << "\nFile cannot be open";
        return 1;
    }

    RowsReader reader(ifile);
    std::cout << find(reader);
    return 0;
}