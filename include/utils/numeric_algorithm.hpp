#pragma once

#include <cstdint>
#include <cmath>

[[nodiscard]] constexpr size_t numOfDigits(uint64_t num) noexcept {
    return static_cast<uint64_t>(std::log10(num) + 1);
}

[[nodiscard]] constexpr std::pair<uint64_t, uint64_t> splitNum(uint64_t num, size_t countOfDigits) noexcept {
    // Calculate the power of 10 to split the number
    const uint64_t splitPoint = countOfDigits / 2;
    const uint64_t divisor = static_cast<uint64_t>(std::pow(10, splitPoint));

    // Split the number
    const uint64_t firstPart = num / divisor;
    const uint64_t secondPart = num % divisor;

    return {firstPart, secondPart};
}