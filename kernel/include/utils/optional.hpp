/*
** Created by doom on 07/11/18.
*/

#ifndef FOROS_UTILS_OPTIONAL_HPP
#define FOROS_UTILS_OPTIONAL_HPP

#include <optional>
#include <functional>

namespace utils
{
    /** Optional class with functional-style-ish features */
    template <typename T>
    class optional : public std::optional<T>
    {
    protected:
        using parent = std::optional<T>;

    public:
        using std::optional<T>::optional;
        using value_type = typename parent::value_type;

        constexpr optional(std::nullopt_t) noexcept : std::optional<T>::optional(std::nullopt)
        {
        }

        template <typename Func, typename U = std::invoke_result_t<Func, T &&>>
        utils::optional<U> map(Func &&f) &&
        {
            if (this->has_value()) {
                return utils::optional<U>(f(*std::move(*this)));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, const T &&>>
        utils::optional<U> map(Func &&f) const &&
        {
            if (this->has_value()) {
                return utils::optional<U>(f(*std::move(*this)));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, T &>>
        utils::optional<U> map(Func &&f) &
        {
            if (this->has_value()) {
                return utils::optional<U>(f(**this));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, const T &>>
        utils::optional<U> map(Func &&f) const &
        {
            if (this->has_value()) {
                return utils::optional<U>(f(**this));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, T &&>>
        U and_then(Func &&f) &&
        {
            if (this->has_value()) {
                return std::invoke(std::forward<Func>(f), *std::move(*this));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, const T &&>>
        U and_then(Func &&f) const &&
        {
            if (this->has_value()) {
                return std::invoke(std::forward<Func>(f), *std::move(*this));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, T &>>
        U and_then(Func &&f) &
        {
            if (this->has_value()) {
                return std::invoke(std::forward<Func>(f), **this);
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, const T &>>
        U and_then(Func &&f) const &
        {
            if (this->has_value()) {
                return std::invoke(std::forward<Func>(f), **this);
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func>>
        U or_else(Func &&f) &&
        {
            if (this->has_value()) {
                return std::move(*this);
            }
            return f();
        }

        template <typename Func, typename U = std::invoke_result_t<Func>>
        U or_else(Func &&f) const &&
        {
            if (this->has_value()) {
                return std::move(*this);
            }
            return f();
        }

        template <typename Func, typename U = std::invoke_result_t<Func>>
        U or_else(Func &&f) &
        {
            if (this->has_value()) {
                return *this;
            }
            return f();
        }

        template <typename Func, typename U = std::invoke_result_t<Func>>
        U or_else(Func &&f) const &
        {
            if (this->has_value()) {
                return *this;
            }
            return f();
        }
    };

    template <typename T>
    class optional<T &> : protected std::optional<T *>
    {
    protected:
        using parent = std::optional<T *>;

    public:
        using value_type = T &;

        constexpr optional() : parent()
        {
        }

        template <typename ...Args>
        constexpr optional(T &t) : parent(&t)
        {
        }

        constexpr optional(std::nullopt_t) noexcept : parent::optional(std::nullopt)
        {
        }

        using parent::operator bool;

        constexpr const T &operator*() const & noexcept
        {
            return *parent::operator*();
        }

        constexpr T &operator*() & noexcept
        {
            return *parent::operator*();
        }

        constexpr const T &&operator*() const && noexcept
        {
            return std::move(*parent::operator*());
        }

        constexpr T &&operator*() && noexcept
        {
            return std::move(*parent::operator*());
        }

        constexpr const T *operator->() const noexcept
        {
            return *parent::operator->();
        }

        constexpr T *operator->() noexcept
        {
            return *parent::operator->();
        }

        constexpr const T &value() const &
        {
            return *parent::value();
        }

        constexpr T &value() &
        {
            return *parent::value();
        }

        constexpr const T &&value() const &&
        {
            return std::move(*parent::value());
        }

        constexpr T &&value() &&
        {
            return std::move(*parent::value());
        }

        template <typename U>
        constexpr T &value_or(U &&default_value) const &
        {
            if (parent::has_value())
                return *parent::operator*();
            return std::forward<U>(default_value);
        }

        template <typename U>
        constexpr T &value_or(U &&default_value) const &&
        {
            if (parent::has_value())
                return std::move(*parent::operator*());
            return std::forward<U>(default_value);
        }

        template <typename Func, typename U = std::invoke_result_t<Func, T &&>>
        utils::optional<U> map(Func &&f) &&
        {
            if (this->has_value()) {
                return utils::optional<U>(f(*std::move(*this)));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, const T &&>>
        utils::optional<U> map(Func &&f) const &&
        {
            if (this->has_value()) {
                return utils::optional<U>(f(*std::move(*this)));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, T &>>
        utils::optional<U> map(Func &&f) &
        {
            if (this->has_value()) {
                return utils::optional<U>(f(**this));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, const T &>>
        utils::optional<U> map(Func &&f) const &
        {
            if (this->has_value()) {
                return utils::optional<U>(f(**this));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, T &&>>
        U and_then(Func &&f) &&
        {
            if (this->has_value()) {
                return std::invoke(std::forward<Func>(f), *std::move(*this));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, const T &&>>
        U and_then(Func &&f) const &&
        {
            if (this->has_value()) {
                return std::invoke(std::forward<Func>(f), *std::move(*this));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, T &>>
        U and_then(Func &&f) &
        {
            if (this->has_value()) {
                return std::invoke(std::forward<Func>(f), **this);
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, const T &>>
        U and_then(Func &&f) const &
        {
            if (this->has_value()) {
                return std::invoke(std::forward<Func>(f), **this);
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func>>
        U or_else(Func &&f) &&
        {
            if (this->has_value()) {
                return std::move(*this);
            }
            return f();
        }

        template <typename Func, typename U = std::invoke_result_t<Func>>
        U or_else(Func &&f) const &&
        {
            if (this->has_value()) {
                return std::move(*this);
            }
            return f();
        }

        template <typename Func, typename U = std::invoke_result_t<Func>>
        U or_else(Func &&f) &
        {
            if (this->has_value()) {
                return *this;
            }
            return f();
        }

        template <typename Func, typename U = std::invoke_result_t<Func>>
        U or_else(Func &&f) const &
        {
            if (this->has_value()) {
                return *this;
            }
            return f();
        }
    };
}

#endif /* !FOROS_UTILS_OPTIONAL_HPP */
