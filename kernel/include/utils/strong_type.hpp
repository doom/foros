/*
** Created by doom on 09/10/18.
*/

#ifndef FOROS_STRONG_TYPE_HPP
#define FOROS_STRONG_TYPE_HPP

#include <type_traits>

namespace utils
{
    template <typename T, typename Tag>
    class strong_type
    {
    public:
        using value_type = T;
        using tag_type = Tag;

        explicit strong_type(const T &t) : _t(t)
        {
        }

        explicit strong_type(T &&t) noexcept(std::is_nothrow_move_constructible_v<T>) : _t(t)
        {
        }

        strong_type(const strong_type &) = default;

        strong_type(strong_type &&) noexcept(std::is_nothrow_move_constructible_v<T>) = default;

        constexpr const T &value() const noexcept
        {
            return _t;
        }

        constexpr T &value() noexcept
        {
            return _t;
        }

    private:
        T _t;
    };
}

#endif /* !FOROS_STRONG_TYPE_HPP */
