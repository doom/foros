/*
** Created by doom on 02/11/18.
*/

#ifndef FOROS_SCROLLING_PRINTER_HPP
#define FOROS_SCROLLING_PRINTER_HPP

#include <vga/details/printer_base.hpp>

namespace foros::vga
{
    namespace details
    {
        /**
         * Printer providing automatic scrolling
         *
         * When it reaches the lower end of the screen, all visible content is pushed up in order
         * to make room for a new empty line.
         *
         * This printer is a shared instance meant to be used to print streams of text without
         * having to manage cursor positions.
         */
        class scrolling_printer : public details::printer_base<scrolling_printer>,
                                  public utils::singleton<scrolling_printer>
        {
        private:
            friend utils::singleton<scrolling_printer>;
            friend printer_base<scrolling_printer>;

            constexpr scrolling_printer() noexcept : printer_base(vga::x(0), vga::y(0))
            {
            }

            void _write_char(char c) noexcept
            {
                using namespace vga::literals;

                if (_y == screen::instance().height()) {
                    screen::instance().scroll();
                    --_y;
                }

                switch (c) {
                    case '\n':
                        _x = 0_x;
                        ++_y;
                        break;
                    case '\r':
                        _x = 0_x;
                        break;
                    case '\b':
                        if (_x > 0_x)
                            --_x;
                        else if (_y > 0_y) {
                            --_y;
                            _x = screen::instance().width() - 1_x;
                        }
                        break;
                    default:
                        screen::instance()[_y][_x] = screen_character(c, _bkgd, _text);
                        if (_x == screen::instance().width() - 1_x) {
                            _x = 0_x;
                            ++_y;
                        } else
                            ++_x;
                        break;
                }
            }
        };
    }

    static inline details::scrolling_printer &scrolling_printer() noexcept
    {
        return details::scrolling_printer::instance();
    }
}

#endif /* !FOROS_SCROLLING_PRINTER_HPP */
