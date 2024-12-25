#include "common_headers.hpp"

#include <stack>
#include <string>
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>


enum class Operation {AND, OR, XOR};

struct LogicExpression final {
    std::string operand1;
    Operation operation;
    std::string operand2;
    std::string output;
};

struct Program final
{
    std::unordered_map<std::string, bool> gates;
    std::vector<LogicExpression> exprs;
};

[[nodiscard]] bool executeOperation(bool operand1, bool operand2, Operation operation) {
    switch (operation)
    {
    case Operation::AND:
        return operand1 && operand2;
    case Operation::OR:
        return operand1 || operand2;
    case Operation::XOR:
        return operand1 != operand2;
    default:
        throw std::runtime_error{"unknown operation type"};
    }
    return false;
}

[[nodiscard]] Operation toOperation(const std::string_view op) {
    if(op == "AND") return Operation::AND;
    if(op == "OR") return Operation::OR;
    if(op == "XOR") return Operation::XOR;
    throw std::runtime_error{"unknown operation type"};
} 

[[nodiscard]] std::pair<std::string, bool> parseInitialGate(const std::string& line)
{
    const auto pos = line.find(":");
    if(pos == std::string::npos) {
        throw std::logic_error{"input invalid"};
    }
    std::string gate{line.substr(0, pos)};
    bool value{line.at(pos + 2) == '1'};
    return {std::move(gate), value}; 
}

[[nodiscard]] LogicExpression parseLogicExpression(const std::string& line)
{
    std::regex pattern(R"((\w+) (\w+) (\w+) -> (\w+))");
    std::smatch match;
    if(!std::regex_match(line, match, pattern)) {
        throw std::logic_error{"invalid logic expression"};
    }

    LogicExpression expr{
        .operand1 = match[1].str(), // operand1
        .operation = toOperation(match[2].str()), // operation
        .operand2 = match[3].str(), // operand2
        .output = match[4].str()  // result
    };

    return expr;
}

[[nodiscard]] Program parseInput(const std::vector<std::string>& inputVec)
{
    Program program;
    size_t id{};
    for(; id < inputVec.size() && !inputVec[id].empty(); ++id) {
        const auto [name, val] = parseInitialGate(inputVec[id]);
        program.gates.emplace(name, val);
    }
    ++id;
    for(; id < inputVec.size(); ++id) {
        program.exprs.push_back(parseLogicExpression(inputVec[id]));
    }
    return program;
}

[[nodiscard]] size_t solveFirstPart(Program& program)
{
    std::unordered_map<std::string, int> preordering;
    std::unordered_map<std::string, std::vector<size_t>> operandToExpressionId;

    for(size_t id = 0; id < program.exprs.size(); ++id) {
        const auto& expr = program.exprs[id];
        operandToExpressionId[expr.operand1].push_back(id);
        operandToExpressionId[expr.operand2].push_back(id);

        preordering[expr.output]++;
    }

    std::queue<size_t> bfs;
    for(size_t id = 0; id < program.exprs.size(); ++id) {
        const auto& expr = program.exprs[id];
        if(preordering[expr.operand1] == 0 && preordering[expr.operand2] == 0) {
            bfs.push(id);
        }
    }

    while(!bfs.empty()) {
        const auto exprId = bfs.front();
        bfs.pop();

        const LogicExpression& expr = program.exprs[exprId];
        const bool operationResult = executeOperation(program.gates.at(expr.operand1), program.gates.at(expr.operand2), expr.operation);
        program.gates[expr.output] = operationResult;

        if(--preordering[expr.output] == 0) {
            for(const size_t otherExprId : operandToExpressionId[expr.output]) {
                if(otherExprId == exprId) continue;
                const LogicExpression& otherExpr = program.exprs[otherExprId];
                
                if(preordering.at(otherExpr.operand1) == 0 && preordering.at(otherExpr.operand2) == 0) {
                    bfs.emplace(otherExprId);
                }
            }
            operandToExpressionId[expr.output].clear();
        }
    }

    size_t zGateOutput{0};
    for(const auto& [gate, result] : program.gates) {
        if(gate[0] != 'z' || result == false) continue;
        const auto gateNumber = std::stoul(gate.substr(1));
        zGateOutput |= (size_t{1} << gateNumber);
    }
    return zGateOutput;
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
    
    std::vector<std::string> inputVec;
    readInput(argv[2], std::back_inserter(inputVec));

    auto program = parseInput(inputVec);

    if(task == "part1") {
        std::cout << solveFirstPart(program);
    }
    else {
        // std::cout << solveSecondPart(program);       
    }

    return 0;
}