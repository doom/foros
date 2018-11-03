/*
** Created by doom on 03/11/18.
*/

#ifndef FOROS_UTILS_POINTER_LIKE_HPP
#define FOROS_UTILS_POINTER_LIKE_HPP

#include <utility>

namespace utils
{
    template <typename T>
    class pointer_like
    {
    public:
        template <typename ...Args>
        explicit constexpr pointer_like(Args &&...args) noexcept : _value{std::forward<Args>(args)...}
        {
        }

        constexpr const T &operator*() const noexcept
        {
            return _value;
        }

        constexpr T &operator*() noexcept
        {
            return _value;
        }

        constexpr const T *operator->() const noexcept
        {
            return &_value;
        }

        constexpr T *operator->() noexcept
        {
            return &_value;
        }

    private:
        T _value;
    };
}

#endif /* !FOROS_UTILS_POINTER_LIKE_HPP */
