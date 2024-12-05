#include "common_headers.hpp"
#include <unordered_map>
#include <unordered_set>
#include <sstream>


using Graph = std::unordered_map<int, std::unordered_set<int>>;

struct Comparator final
{
    explicit Comparator(const Graph& graph) : m_graph{graph}
    {}

    [[nodiscard]] bool operator()(int lhs, int rhs) const noexcept {
        return m_graph.find(lhs) != m_graph.end() && m_graph.at(lhs).count(rhs) > 0;
    }
    
private:
    const Graph& m_graph;
};


[[nodiscard]] bool isCorrectPages(const Graph& graph, const std::vector<int>& pages) noexcept
{
    Comparator cmp{graph};
    return std::is_sorted(pages.begin(), pages.end(), cmp);
}

void correctPages(const Graph& graph, std::vector<int>& pages) noexcept
{
    Comparator cmp{graph};
    std::sort(pages.begin(), pages.end(), cmp);
}

[[nodiscard]] std::pair<int, int> parsePageRule(const std::string& rule) noexcept
{
    constexpr auto convertCharsToInt = [](char tensDigit, char unitsDigit) {
        return static_cast<int>(tensDigit-'0') * 10 +
            static_cast<int>(unitsDigit-'0');
    };
    return {convertCharsToInt(rule[0], rule[1]), convertCharsToInt(rule[3], rule[4])};
}


void printHelp()
{
    std::cerr << "\nUsage:\n"
    << "The program requires 2 args: (part1, part2) and the path to the file."
    << "\nFor example, ./day5 part1 data/day5.txt";
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

    Graph graph;

    std::string line;
    while(std::getline(ifile, line, '\n')) {
        if(line.empty()) {
            break;
        }
        
        const auto [beforeNum, afterNum] = parsePageRule(line);
        graph[beforeNum].emplace(afterNum);
    }

    size_t ans{};
    std::vector<int> pages;
    while((ifile >> line)) {
        std::stringstream ss(line);
        std::string numStr;
        pages.clear();
        while(std::getline(ss, numStr, ',')) {
            pages.push_back(std::stoi(numStr));
        }

        if(task == "part1") {
            if(isCorrectPages(graph, pages)) {
                ans += pages[pages.size() / 2];
            }
        }
        else {
            if(!isCorrectPages(graph, pages)) {
                correctPages(graph, pages);
                ans += pages[pages.size() / 2];
            }
        }

    }

    std::cout << ans;
    
    return 0;
}