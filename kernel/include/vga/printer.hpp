/*
** Created by doom on 11/10/18.
*/

#ifndef FOROS_VGA_PRINTER_HPP
#define FOROS_VGA_PRINTER_HPP

#include <vga/details/printer_base.hpp>

namespace foros::vga
{
    namespace details
    {
        /**
         * Basic printer class
         *
         * This class is meant to be used to create "one-shot" printers with a custom cursor position.
         */
        class printer : public printer_base<printer>
        {
        public:
            using printer_base<printer>::printer_base;

        private:
            friend printer_base<printer>;

            void _write_char(char c) noexcept
            {
                using namespace vga::literals;

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

    static inline details::printer printer(vga::x x, vga::y y) noexcept
    {
        return details::printer{x, y};
    }
}

#endif /* !FOROS_VGA_PRINTER_HPP */
