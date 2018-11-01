/*
** Created by doom on 27/10/18.
*/

#ifndef FOROS_UTILS_BIT_FIELD_HPP
#define FOROS_UTILS_BIT_FIELD_HPP

#include <cstddef>
#include <climits>

namespace utils
{
    template <typename T>
    struct bit_field
    {
    public:
        static_assert(T(0 - 1) > 0, "T should be an unsigned integer type");

        static constexpr const size_t bits = sizeof(T) * CHAR_BIT;

    private:
        template <size_t from, size_t to>
        static constexpr T mask_range() noexcept
        {
            T all_set = ~0;
            T high_mask = all_set << from;
            T low_mask = all_set >> (bits - to);
            return high_mask & low_mask;
        }

    public:
        template <std::size_t index>
        constexpr bool get_bit() const noexcept
        {
            static_assert(index < bits, "index out of bounds");
            return value & (1 << index);
        }

        template <std::size_t index>
        constexpr void set_bit() noexcept
        {
            static_assert(index < bits, "index out of bounds");
            value |= (1 << index);
        }

        template <std::size_t index>
        constexpr void unset_bit() noexcept
        {
            static_assert(index < bits, "index out of bounds");
            value &= ~(1 << index);
        }

        template <std::size_t from, std::size_t to, T to_set = mask_range<from, to>()>
        constexpr void set_bit_range() noexcept
        {
            static_assert(from < to && to <= bits);
            static_assert(mask_range<from, to>() >= to_set, "the given value cannot fit in the given range");
            constexpr T erase_mask = ~mask_range<from, to>();

            value = (value & erase_mask) | (to_set << from);
        }

        template <std::size_t from, std::size_t to>
        constexpr void unset_bit_range() noexcept
        {
            set_bit_range<from, to, 0>();
        }

        T value{0};
    };

    template <typename T>
    auto make_bit_field(T value) noexcept
    {
        return bit_field<T>{value};
    }
}

#endif /* !FOROS_UTILS_BIT_FIELD_HPP */
