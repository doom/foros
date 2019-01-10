/*
** Created by doom on 07/11/18.
*/

#ifndef FOROS_UTILS_OPTIONAL_HPP
#define FOROS_UTILS_OPTIONAL_HPP

#include <optional>
#include <functional>
#include <core/panic.hpp>

namespace utils
{
    /** Optional class with functional-style-ish features */
    template <typename T>
    class optional : private std::optional<T>
    {
    protected:
        using parent = std::optional<T>;

    public:
        using std::optional<T>::optional;
        using value_type = typename parent::value_type;

        constexpr optional(std::nullopt_t) noexcept : std::optional<T>::optional(std::nullopt)
        {
        }

        using parent::operator bool;
        using parent::has_value;

        constexpr const T &unwrap() const & noexcept
        {
            return parent::operator*();
        }

        constexpr T &unwrap() & noexcept
        {
            return parent::operator*();
        }

        constexpr const T &&unwrap() const && noexcept
        {
            return std::move(parent::operator*());
        }

        constexpr T &&unwrap() && noexcept
        {
            return std::move(parent::operator*());
        }

        constexpr const T &unwrap_or_panic(const char *message) const & noexcept
        {
            kassert(has_value(), message);
            return parent::operator*();
        }

        constexpr T &unwrap_or_panic(const char *message) & noexcept
        {
            kassert(has_value(), message);
            return parent::operator*();
        }

        constexpr const T &&unwrap_or_panic(const char *message) const && noexcept
        {
            kassert(has_value(), message);
            return std::move(parent::operator*());
        }

        constexpr T &&unwrap_or_panic(const char *message) && noexcept
        {
            kassert(has_value(), message);
            return std::move(parent::operator*());
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
    class optional<T &>
    {
    private:
        T *_ptr;

    public:
        using value_type = T &;

        constexpr optional() : _ptr(nullptr)
        {
        }

        constexpr optional(T &t) : _ptr(&t)
        {
        }

        constexpr optional(std::nullopt_t) noexcept : _ptr(nullptr)
        {
        }

        constexpr bool has_value() const noexcept
        {
            return _ptr != nullptr;
        }

        constexpr operator bool() const noexcept
        {
            return has_value();
        }

        constexpr const T &unwrap() const noexcept
        {
            return *_ptr;
        }

        constexpr T &unwrap() noexcept
        {
            return *_ptr;
        }

        constexpr const T &unwrap_or_panic(const char *message) const noexcept
        {
            kassert(has_value(), message);
            return *_ptr;
        }

        constexpr T &unwrap_or_panic(const char *message) noexcept
        {
            kassert(has_value(), message);
            return *_ptr;
        }

        template <typename U>
        constexpr T &value_or(U &&default_value) const
        {
            if (has_value())
                return **this;
            return std::forward<U>(default_value);
        }

        template <typename Func, typename U = std::invoke_result_t<Func, T &>>
        utils::optional<U> map(Func &&f)
        {
            if (has_value()) {
                return utils::optional<U>(f(unwrap()));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, const T &>>
        utils::optional<U> map(Func &&f) const
        {
            if (has_value()) {
                return utils::optional<U>(f(**this));
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, T &>>
        U and_then(Func &&f)
        {
            if (has_value()) {
                return std::invoke(std::forward<Func>(f), unwrap());
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func, const T &>>
        U and_then(Func &&f) const
        {
            if (has_value()) {
                return std::invoke(std::forward<Func>(f), unwrap());
            }
            return std::nullopt;
        }

        template <typename Func, typename U = std::invoke_result_t<Func>>
        U or_else(Func &&f)
        {
            if (has_value()) {
                return *this;
            }
            return f();
        }

        template <typename Func, typename U = std::invoke_result_t<Func>>
        U or_else(Func &&f) const
        {
            if (has_value()) {
                return *this;
            }
            return f();
        }
    };
}

#endif /* !FOROS_UTILS_OPTIONAL_HPP */
