#include "common_headers.hpp"
#include "utils/numeric_algorithm.hpp"

#include <sstream>
#include <limits>
#include <cmath>


struct Equation final
{
    int64_t result{};
    std::vector<int64_t> values;
};

[[nodiscard]] constexpr int64_t cat(int64_t a, int64_t b) noexcept {
    const int multiplier = std::pow(10, numOfDigits(b));
    return a * multiplier + b;
};

template<typename ...Operation>
class Combiner final
{
public:
    constexpr explicit Combiner(Operation&& ...operations) : m_functions{std::forward<Operation>(operations)...} {}

    [[nodiscard]] constexpr auto compute(const Equation& equation, size_t id, int64_t curValue) const {
        if(curValue >= equation.result) {
            return curValue == equation.result;
        }
        if(id >= equation.values.size()) {
            return false;
        }
        
        return std::apply(
            [&, this](const auto& ...op) {
                return (op.operate(equation, id, curValue, *this) || ...);
            },
            m_functions);
    }

private:
    std::tuple<Operation...> m_functions;
};

template<typename ...Operation>
[[nodiscard]] constexpr auto makeCombiner(Operation&& ...ops) {
    return Combiner<Operation...>{std::forward<Operation>(ops)...};
}

class CatOperation final
{
public:
    template<typename C>
    constexpr auto operate(const Equation& equation, size_t id, int64_t curValue, C context) const noexcept {
        return context.compute(equation, id + 1, cat(curValue, equation.values[id]));
    }
};

class AddOperation final
{
public:
    template<typename C>
    constexpr auto operate(const Equation& equation, size_t id, int64_t curValue, C context) const noexcept {
        return context.compute(equation, id + 1, curValue + equation.values[id]);
    }
};

class MultOperation final
{
public:
    template<typename C>
    constexpr auto operate(const Equation& equation, size_t id, int64_t curValue, C context) const noexcept {
        return context.compute(equation, id + 1, curValue * equation.values[id]);
    }
};

[[nodiscard]] constexpr bool canBeCombinedBy2Operations(const Equation& equation, size_t id, int64_t curValue) {
    auto combiner{makeCombiner(AddOperation{}, MultOperation{})};
    return combiner.compute(equation, id, curValue);
}

[[nodiscard]] constexpr bool canBeCombinedBy3Operations(const Equation& equation, size_t id, int64_t curValue) {
    auto combiner{makeCombiner(CatOperation{}, AddOperation{}, MultOperation{})};
    return combiner.compute(equation, id, curValue);
}

template<typename CombineFun>
[[nodiscard]] size_t solveImpl(const std::vector<Equation>& equations, CombineFun canBeCombined) noexcept
{
    size_t totalSum{};
    for(const auto& equation : equations) {
        if(canBeCombined(equation, 0, 0)) {
            totalSum += equation.result;
        }
    }
    return totalSum;
}

[[nodiscard]] size_t solveFirstPart(const std::vector<Equation>& equations) noexcept
{
    return solveImpl(equations, canBeCombinedBy2Operations);
}

[[nodiscard]] size_t solveSecondPart(const std::vector<Equation>& equations) noexcept
{
    return solveImpl(equations, canBeCombinedBy3Operations);
}

void printHelp()
{
    std::cerr << "\nUsage:\n"
    << "The program requires 2 args: (part1, part2) and the path to the file."
    << "\nFor example, ./day7 part1 data/day7.txt";
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

    std::vector<Equation> equations;
    std::string line;
    while(std::getline(ifile, line)) {
        equations.emplace_back();
        Equation& eq = equations.back();
        
        std::istringstream ss(line);
        char colon{};
        ss >> eq.result >> colon;
        int num2{};
        while((ss >> num2)) {
            eq.values.push_back(num2);
        }
    }

    if(task == "part1") {
        std::cout << solveFirstPart(equations);
    }
    else {
        std::cout << solveSecondPart(equations);
    }

    return 0;
}