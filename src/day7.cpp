#include "common_headers.hpp"
#include "utils/numeric_algorithm.hpp"

#include <chrono>
#include <concepts>
#include <limits>
#include <functional>
#include <sstream>

constexpr std::string_view gInput {
    #include "../data/2024/day7_constexpr.txt"
};

constexpr size_t gArgumentsSizeMax{100};
using ArgumentsArray = std::array<int64_t, gArgumentsSizeMax>;
struct Equation
{
    int64_t result{};
    size_t argumentsCount{};
    ArgumentsArray arguments;
};

template<typename T>
concept IsEquation = std::is_same_v<T, Equation>;

template<typename T>
concept EquationArrayContainer = requires(T a)
{
    requires IsEquation<std::iter_value_t<decltype(std::begin(a))>>;
    requires std::forward_iterator<decltype(std::begin(a))>;
    requires std::forward_iterator<decltype(std::end(a))>;
};

template<typename T>
concept EquationPointerArray = std::is_pointer_v<T> && IsEquation<std::remove_pointer_t<T>>;

template<typename T>
concept EquationBoundedArray = std::is_bounded_array_v<T> && requires(T a)
{
    requires IsEquation<std::remove_cvref_t<decltype(a[0])>>;
};

template<typename T>
concept EquationArray = EquationArrayContainer<T> || EquationPointerArray<T> || EquationBoundedArray<T>;


// TODO: replace the function with std::from_chairs, that becomes constexpr in C++23.
[[nodiscard]] constexpr std::pair<int64_t, size_t> parseNumber(std::string_view str) noexcept {
    constexpr auto char_to_int = [](char c) {
        return c - '0';
    };
    
    size_t index{};
    int64_t result{};
    while (index < str.size() && std::isdigit(str[index])) {
        result = result * 10 + char_to_int(str[index]);
        ++index;
    };
    return std::pair{result, index};
}

constexpr void skipNonDigits(std::string_view& str) noexcept {
    const auto it = std::find_if(str.begin(), str.end(), [](const char ch) {
        return std::isdigit(ch);
    });

    if(it != str.end()) {
        str.remove_prefix(std::distance(str.begin(), it));
    }
    else {
        str = {};
    }
}

[[nodiscard]] constexpr std::pair<ArgumentsArray, size_t> parseArgumentsEquation(std::string_view& data)
{
    size_t id{};
    ArgumentsArray arguments{};
    while(id < gArgumentsSizeMax && !data.empty() && data.front() != '\n') {
        skipNonDigits(data);
        if(data.empty()) break;

        const auto [argument, parsedNums] = parseNumber(data);

        arguments[id++] = argument;
        data.remove_prefix(parsedNums);
    }
    if (id == gArgumentsSizeMax) {
        throw std::logic_error{"Number of equation arguments exceeds the threshold"};
    }

    return std::pair{arguments, id};
}

[[nodiscard]] constexpr size_t numOfEquastions(std::string_view testData) noexcept
{
    return std::accumulate(testData.begin(), testData.end(), size_t{}, [](size_t value, char ch) {
        return value + (ch == ':');
    });
}

[[nodiscard]] constexpr Equation parseEquation(std::string_view& testData) {
    // Equestion pattern looks like: "1234: 32 12 12".
    // The first number is result followed by comma.
    // The rest numbers are arguments separated by whitespace.
    const auto [result, parsedNums] = parseNumber(testData);
    if(parsedNums + 2 >= testData.size() || testData[parsedNums] != ':') {
        throw std::runtime_error{"invalid input data"};
    }
    testData.remove_prefix(parsedNums + 2);
    auto [arguments, count] = parseArgumentsEquation(testData);

    Equation equation{
        .result = result,
        .argumentsCount = count,
        .arguments = arguments
    };
    return equation;
}

[[nodiscard]] constexpr auto parseEquations(std::string_view testData, EquationArray auto& equations) {
    size_t index{};
    const auto N{equations.size()};
    while(index < N && !testData.empty()) {
        skipNonDigits(testData);
        if(testData.empty()) break;

        equations[index] = parseEquation(testData);
        ++index;
    }    
}

[[nodiscard]] constexpr int64_t cat(int64_t a, int64_t b) noexcept
{
    const int multiplier = std::pow(10, numOfDigits(b));
    return a * multiplier + b;
};

template<typename T>
concept OperatorInvocable = std::regular_invocable<T, int64_t, int64_t>;

template<typename ...Operations>
requires (OperatorInvocable<Operations> && ...)
class Combiner final
{
public:
    constexpr explicit Combiner(Operations&& ...operations) : m_operations{std::forward<Operations>(operations)...} {}

    [[nodiscard]] constexpr bool compute(const Equation& equation, size_t id, int64_t curValue) const {
        if(curValue >= equation.result) {
            return curValue == equation.result;
        }
        if(id >= equation.argumentsCount) {
            return false;
        }

        return std::apply(
            [&, this](const auto& ...op) {
                const auto arg{equation.arguments[id++]};
                return ((compute(equation, id, op(curValue, arg))) || ...);
            },
            m_operations);
    }

    [[nodiscard]] constexpr bool compute(const Equation& equation) const {
        constexpr size_t id{};
        constexpr size_t initValue{};
        return compute(equation, id, initValue);
    }

private:
    std::tuple<Operations...> m_operations;
};

template<typename ...Operation> requires (OperatorInvocable<Operation> && ...)
[[nodiscard]] constexpr auto makeCombiner(Operation&& ...ops) noexcept {
    return Combiner<Operation...>{std::forward<Operation>(ops)...};
}

class CatOperation final
{
public:
    constexpr auto operator()(int64_t initValue, int64_t curValue) const noexcept {
        return cat(initValue, curValue);
    }
};

[[nodiscard]] constexpr bool canBeCombinedBy2Operations(const Equation& equation) {
    constexpr auto combiner{makeCombiner(std::multiplies<>{}, std::plus<>{})};
    return combiner.compute(equation);
}

[[nodiscard]] constexpr bool canBeCombinedBy3Operations(const Equation& equation) {
    auto combiner{makeCombiner(std::plus<>{}, std::multiplies<>{}, CatOperation{})};
    return combiner.compute(equation);
}

template<typename CombineFun>
[[nodiscard]] constexpr size_t solveImpl(const EquationArray auto& equations, const CombineFun& canBeCombined) noexcept
{
    return std::accumulate(std::begin(equations), std::end(equations), size_t{}, [&canBeCombined](size_t value, const Equation& equation) {
        return canBeCombined(equation) ? value + equation.result : value;
    });
}

[[nodiscard]] constexpr size_t solveFirstPart(const EquationArray auto& equations) noexcept
{
    return solveImpl(equations, canBeCombinedBy2Operations);
}

[[nodiscard]] constexpr size_t solveSecondPart(const EquationArray auto& equations) noexcept
{
    return solveImpl(equations, canBeCombinedBy3Operations);
}

[[nodiscard]] std::string readFile(std::string_view filename) {
    std::ifstream ifile(filename.data());
    if(!ifile) {
        throw std::runtime_error{"Failed to open file"};
    }
    std::string str(std::istreambuf_iterator<char>{ifile}, {});
    return str;
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

    if(task == "part1" || task == "part2") {
        using SolutionImplFunction = size_t(*)(const std::vector<Equation>&);
        std::unordered_map<std::string_view, SolutionImplFunction> handlers{
            {"part1", solveFirstPart<std::vector<Equation>>},
            {"part2", solveSecondPart<std::vector<Equation>>},
        };

        const auto start = std::chrono::high_resolution_clock::now();
        const auto fileContent{readFile(argv[2])};
        std::string_view fileContentView{fileContent};
        const size_t N{numOfEquastions(fileContentView)};
        std::vector<Equation> equations(N);
        parseEquations(fileContentView, equations);

        const auto res = handlers.at(task)(equations);
        const auto end = std::chrono::high_resolution_clock::now();

        std::cout << res
        << " elapsed " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    }
    else if(task == "part1_constexpr") {
        
    #if defined(DAY7_CONSTEXPR)
        const auto constexpr_context = [] {
            constexpr size_t N{numOfEquastions(gInput)};
            std::array<Equation, N> equations{};
            parseEquations(gInput, equations);

            return equations;
        };
        constexpr auto equations{constexpr_context()};
        constexpr auto firstPartAns{solveFirstPart(equations)};
        std::cout << firstPartAns;

        // TODO: solving second part takes a lot of time and consumes lots of memory.
        // constexpr auto secondPartAns{solveSecondPart(equations)};
        // std::cout << "\n" << secondPartAns;
    #else
        std::cout << "\ndisabled. DAY7_CONSTEXPR compile variable should be defined";
    #endif
    }
    else {
        std::cerr << "\nfirst arg can be either `part1`, `part2`, or `part1_constexpr`\n";
        printHelp();
        return 1;
    }

    return 0;
}