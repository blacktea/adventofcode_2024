#include "common_headers.hpp"
#include <cmath>
#include <regex>

void printHelp()
{
    std::cerr << "\nUsage:\n"
    << "The program requires 2 args: (part1, part2) and the path to the file."
    << "\nFor example, ./day7 part1 data/day7.txt";
}

using Matrix = std::vector<std::vector<long double>>;
using Vector = std::vector<long double>;

struct EquationParams final
{
    Matrix coords;
    Vector result;

    void appendXY(std::pair<int64_t, int64_t> xy) {
        coords.resize(2);
        coords[0].push_back(xy.first);
        coords[1].push_back(xy.second);
    }

    void setResult(std::pair<int64_t, int64_t> resultXY) {
        result.push_back(resultXY.first);
        result.push_back(resultXY.second);
    }
};


void gaussianElimination(Matrix& A, Vector& b) {
    int n = A.size();

    for (int i = 0; i < n; ++i) {
        // Partial pivoting
        int maxRow = i;
        for (int k = i + 1; k < n; ++k) {
            if (std::abs(A[k][i]) > std::abs(A[maxRow][i])) {
                maxRow = k;
            }
        }

        // Swap rows in A and b
        std::swap(A[i], A[maxRow]);
        std::swap(b[i], b[maxRow]);

        // Check for singularity
        if (std::abs(A[i][i]) < 1e-10) {
            throw std::runtime_error("Matrix is singular or nearly singular");
        }

        // Eliminate column i below the pivot
        for (int k = i + 1; k < n; ++k) {
            double factor = A[k][i] / A[i][i];
            for (int j = i; j < n; ++j) {
                A[k][j] -= factor * A[i][j];
            }
            b[k] -= factor * b[i];
        }
    }
}

Vector backSubstitution(const Matrix& A, const Vector& b) {
    int n = A.size();
    Vector x(n);

    for (int i = n - 1; i >= 0; --i) {
        x[i] = b[i];
        for (int j = i + 1; j < n; ++j) {
            x[i] -= A[i][j] * x[j];
        }
        x[i] /= A[i][i];
    }

    return x;
}

Vector solveLinearSystem(Matrix A, Vector b) {
    gaussianElimination(A, b);
    return backSubstitution(A, b);
}

bool checkResultCorrectness(const Vector& result, double upperBoundValue) {
    for(auto r : result) {
        if(!std::isfinite(r) || std::abs(std::trunc(std::round(r)) - r) > 0.0003 || r > upperBoundValue) {
            if(r > 100) {
            }
            std::cerr << "\nsolution incorrect: " << r ;
            return false;
        }
    }
    return true;
}

size_t solveFirstPart(const std::vector<EquationParams>& equationParams) {
    constexpr size_t costButtonA{3};
    constexpr size_t costButtonB{1};
    size_t tokens{};
    
    for(const auto& equation : equationParams) {
        Vector result = solveLinearSystem(equation.coords, equation.result);
        if(checkResultCorrectness(result, 100)) {
            tokens += static_cast<size_t>(std::round(result.at(0))) * costButtonA + static_cast<size_t>(std::round(result.at(1)) * costButtonB);
        }
    }
    return tokens;
}

size_t solveSecondPart(const std::vector<EquationParams>& equationParams) {
    constexpr size_t costButtonA{3};
    constexpr size_t costButtonB{1};
    size_t tokens{};
    
    for(const auto& equation : equationParams) {
        Vector result = solveLinearSystem(equation.coords, equation.result);
        if(checkResultCorrectness(result, 10000000000000)) {
            tokens += static_cast<size_t>(std::round(result.at(0))) * costButtonA + static_cast<size_t>(std::round(result.at(1)) * costButtonB);
        }
    }
    return tokens;
}


[[nodiscard]] std::pair<int64_t, int64_t> parseXY(const std::string& input) {
    std::pair<int64_t, int64_t> values{};

    // Regular expression to match the input pattern
    std::regex pattern(R"((\w+ \w+|\w+): X[+=](\-?\d+), Y[+=](\-?\d+))");
    std::smatch matches;

    std::string::const_iterator searchStart(input.cbegin());
    std::regex_search(searchStart, input.cend(), matches, pattern);
    // std::cerr << "\nx " << matches[2] << " y " << matches[3];
    return std::pair{std::stoll(matches[2]), std::stoll(matches[3])};
}

[[nodiscard]] EquationParams parse(const std::string& buttonA, const std::string& buttonB, const std::string& result) {
    EquationParams params;
    params.appendXY(parseXY(buttonA));
    params.appendXY(parseXY(buttonB));
    params.setResult(parseXY(result));
    return params;
}

[[nodiscard]] std::vector<EquationParams> parseEquationParams(const std::vector<std::string>& inputLines) {
    std::vector<EquationParams> equationParams;
    for(size_t i = 0; i + 3 <= inputLines.size();) {
        if(inputLines[i].empty()) {
            ++i;
        }
        else {
            equationParams.emplace_back(parse(inputLines[i], inputLines[i + 1], inputLines[i + 2]));
            i += 3;
        }
    }
    return equationParams;
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
    
    std::vector<std::string> inputVec;
    readInput(argv[2], std::back_inserter(inputVec));
    std::vector<EquationParams> equationParams = parseEquationParams(inputVec);
    

    if(task == "part1") {
        std::cout << solveFirstPart(equationParams);
    }
    else {
        for(auto& param : equationParams) {
            param.result[0] += 10000000000000;
            param.result[1] += 10000000000000;
        }
        std::cout << solveSecondPart(equationParams);
    }

    return 0;
}