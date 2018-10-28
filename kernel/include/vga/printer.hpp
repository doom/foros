/*
** Created by doom on 11/10/18.
*/

#ifndef FOROS_VGA_PRINTER_HPP
#define FOROS_VGA_PRINTER_HPP

#include <vga/screen.hpp>

namespace foros::vga
{
    namespace details
    {
        class printer
        {
        public:
            printer(vga::x x, vga::y y) noexcept : _x(x), _y(y)
            {
            }

            printer &operator<<(char c) noexcept
            {
                switch (c) {
                    case '\n':
                        _x.value() = 0;
                        ++_y;
                        break;
                    case '\r':
                        _x = vga::x(0);
                        break;
                    case '\b':
                        if (_x > vga::x(0))
                            --_x;
                        else {
                            --_y;
                            _x = screen::instance().width() - vga::x(1);
                        }
                        break;
                    default:
                        screen::instance()[vga::y(_y)][vga::x(_x)] = screen_character(c, _bkgd, _text);
                        if (_x == screen::instance().width() - vga::x(1)) {
                            _x = vga::x(0);
                            ++_y;
                        } else
                            ++_x;
                        break;
                }
                return *this;
            }

            printer &operator<<(const char *str) noexcept
            {
                while (*str != '\0') {
                    *this << *str++;
                }
                return *this;
            }

            printer &operator<<(background_color bkgd) noexcept
            {
                _bkgd = bkgd;
                return *this;
            }

            printer &operator<<(text_color text) noexcept
            {
                _text = text;
                return *this;
            }

        private:
            vga::x _x;
            vga::y _y;
            background_color _bkgd{vga::black};
            text_color _text{vga::white};
        };
    }

    static inline details::printer printer(vga::x x, vga::y y) noexcept
    {
        return details::printer{x, y};
    }
}

#endif /* !FOROS_VGA_PRINTER_HPP */
