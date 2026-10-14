#ifndef BCDCONFIG_H
#define BCDCONFIG_H

#include "common_types.h"
#include <array>
#include <cstddef>

namespace BcdConfig {
constexpr int GroupCount = 4;
constexpr int BitsPerGroup = 4;

constexpr bool isBcd(int mode)
{
    return mode >= BCD1_BIT0_GND && mode <= BCD4_BIT3_VCC;
}

struct Group {
    std::array<int, BitsPerGroup> assignments{};
    int bitCount = 0;

    int buttonCount() const { return bitCount ? 1 << bitCount : 0; }
    bool valid() const
    {
        for (int bit = 0; bit < bitCount; ++bit)
            if (assignments[bit] != 1) return false;
        return true;
    }
};

struct Layout {
    std::array<Group, GroupCount> groups{};
    int buttonCount() const
    {
        int count = 0;
        for (const auto &group : groups) count += group.buttonCount();
        return count;
    }
    bool valid() const
    {
        for (const auto &group : groups)
            if (!group.valid()) return false;
        return true;
    }
};

inline Layout inspect(const pin_t *pins, std::size_t count)
{
    Layout layout;
    for (std::size_t pin = 0; pin < count; ++pin) {
        if (!isBcd(pins[pin])) continue;
        const int mode = pins[pin] - BCD1_BIT0_GND;
        auto &group = layout.groups[mode / 8];
        const int bit = mode % BitsPerGroup;
        ++group.assignments[bit];
        if (bit + 1 > group.bitCount) group.bitCount = bit + 1;
    }
    return layout;
}
}

#endif
