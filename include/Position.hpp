#pragma once

struct Position final
{
    int64_t x{};
    int64_t y{};

    explicit constexpr Position() = default;
    explicit constexpr Position(int64_t arg_x, int64_t arg_y) noexcept : x{arg_x}, y{arg_y} {}

    [[nodiscard]] constexpr bool inRange(int64_t rows, int64_t cols) const noexcept {
        return x >= 0 && x < rows && y >= 0 && y < cols;
    }
};
