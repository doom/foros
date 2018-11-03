/*
** Created by doom on 02/11/18.
*/

#ifndef FOROS_PRINTER_BASE_HPP
#define FOROS_PRINTER_BASE_HPP

#include <climits>
#include <type_traits>
#include <charconv>
#include <string_view>
#include <vga/screen.hpp>

namespace foros::vga::details
{
    /**
     * This class template provides common formatting utilities for inheriting printer classes.
     * Thus, the concrete printers only have to deal with character-by-character printing and leave
     * the formatting to this base class.
     *
     * The ConcretePrinter class should provide the _write_char(char) member function.
     */
    template <typename ConcretePrinter>
    class printer_base
    {
    public:
        constexpr printer_base(vga::x x, vga::y y) noexcept : _x(x), _y(y)
        {
        }

        printer_base &operator<<(char c) noexcept
        {
            static_cast<ConcretePrinter *>(this)->_write_char(c);
            return *this;
        }

    public:
        printer_base &operator<<(const char *str) noexcept
        {
            while (*str != '\0') {
                *this << *str++;
            }
            return *this;
        }

        printer_base &operator<<(std::string_view sv) noexcept
        {
            for (const auto &c : sv) {
                *this << c;
            }
            return *this;
        }

    private:
        template <typename T>
        void _write_number(T value, int base = 10) noexcept
        {
            char buf[sizeof(T) * 3]; /** shameful approximation */

            auto res = std::to_chars(buf, buf + sizeof(buf), value, base);
            kassert(res.ec != std::errc::value_too_large, "vga::printer_base::operator<<(Integral): implementation bug");
            *this << std::string_view(buf, res.ptr - buf);
        }

    public:
        template <typename T, typename = std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, char>>>
        printer_base &operator<<(T value) noexcept
        {
            _write_number(value);
            return *this;
        }

        template <typename T, typename = std::enable_if_t<!std::is_same_v<char, std::remove_cv_t<T>>>>
        printer_base &operator<<(T *ptr) noexcept
        {
            *this << "0x";
            _write_number((uintptr_t)ptr, 16);
            return *this;
        }

        printer_base &operator<<(background_color bkgd) noexcept
        {
            _bkgd = bkgd;
            return *this;
        }

        printer_base &operator<<(text_color text) noexcept
        {
            _text = text;
            return *this;
        }

    protected:
        vga::x _x;
        vga::y _y;
        background_color _bkgd{vga::black};
        text_color _text{vga::white};
    };
}

#endif /* !FOROS_PRINTER_BASE_HPP */
