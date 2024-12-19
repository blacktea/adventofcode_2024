#pragma once

// #include <space

class Cursor final
{
public:
    enum class Direction{Up = 0, Right = 1, Down = 2, Left=3, Sentinel=4};

    Cursor() = default;
    
    explicit constexpr Cursor(Direction initDirection, int initX, int initY)
    : m_direction{initDirection}
    , m_x{initX}
    , m_y{initY}
    {}

    [[nodiscard]] constexpr int x() const noexcept {
        return m_x;
    }

    [[nodiscard]] constexpr int y() const noexcept {
        return m_y;
    }

    [[nodiscard]] constexpr Direction direction() const noexcept {
        return m_direction;
    }

    constexpr void stepUp() noexcept {
        --m_x;
    }

    constexpr void stepDown() noexcept {
        ++m_x;
    }

    constexpr void stepLeft() noexcept {
        --m_y;
    }

    constexpr void stepRight() noexcept {
        ++m_y;
    }


    constexpr void move() noexcept {
        switch (m_direction)
        {
        case Direction::Up:
            stepUp();
            break;
        case Direction::Down:
            stepDown();
            break;
        case Direction::Left:
            stepLeft();
            break;
        case Direction::Right:
            stepRight();
            break;
        }
    }

    constexpr void stepBack() noexcept {
        switch (m_direction)
        {
        case Direction::Up:
            stepDown();
            break;
        case Direction::Left:
            stepRight();
            break;
        case Direction::Down:
            stepUp();
            break;
        case Direction::Right:
            stepLeft();
            break;
        }
    }

    constexpr void turnRight() noexcept {
       constexpr auto sentinelInt = static_cast<int>(Direction::Sentinel);
       m_direction = static_cast<Direction>((static_cast<int>(m_direction) + 1) % sentinelInt);
    }

    constexpr void setDirection(Direction dir) noexcept {
        m_direction = dir;
    }

    [[nodiscard]] constexpr bool operator==(const Cursor&) const = default;

    [[nodiscard]] constexpr bool operator!=(const Cursor&) const = default;

private:
    Direction m_direction{Direction::Up};
    int m_x{}, m_y{};
};