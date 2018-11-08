/*
** Created by doom on 07/11/18.
*/

#ifndef FOROS_UTILS_OPTIONAL_HPP
#define FOROS_UTILS_OPTIONAL_HPP

#include <optional>

namespace utils
{
    namespace details
    {
        template <typename F, typename ...Args>
        using result_t = std::remove_reference_t<decltype(std::declval<F>()(std::declval<Args>()...))>;
    }

    /** Optional class with functional-style-ish features */
    template <typename T>
    class optional : public std::optional<T>
    {
    public:
        using std::optional<T>::optional;

        constexpr optional(std::nullopt_t) noexcept : std::optional<T>::optional(std::nullopt)
        {
        }

        template <typename Func, typename U = details::result_t<Func, T &&>>
        utils::optional<U> map(Func &&f) &&
        {
            if (this->has_value()) {
                return utils::optional<U>(f(*std::move(*this)));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = details::result_t<Func, const T &>>
        utils::optional<U> map(Func &&f) const &
        {
            if (this->has_value()) {
                return utils::optional<U>(f(**this));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = details::result_t<Func, T &&>>
        U and_then(Func &&f) &&
        {
            if (this->has_value()) {
                return f(*std::move(*this));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = details::result_t<Func, const T &>>
        U and_then(Func &&f) const &
        {
            if (this->has_value()) {
                return f(**this);
            }
            return std::nullopt;
        }

    };
}

#endif /* !FOROS_UTILS_OPTIONAL_HPP */
