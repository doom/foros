/*
** Created by doom on 08/10/18.
*/

#ifndef FOROS_VGA_SCREEN_HPP
#define FOROS_VGA_SCREEN_HPP

#include <stdint.h>
#include <utils/singleton.hpp>
#include <utils/strong_type.hpp>
#include <core/panic.hpp>

namespace foros::vga
{
    using x = utils::strong_type<unsigned int, struct x_coord_tag>;
    using y = utils::strong_type<unsigned int, struct y_coord_tag>;

    namespace literals
    {
        static inline vga::x operator ""_x(unsigned long long value) noexcept
        {
            return vga::x{static_cast<unsigned int>(value)};
        }

        static inline vga::y operator ""_y(unsigned long long value) noexcept
        {
            return vga::y{static_cast<unsigned int>(value)};
        }
    }

#define VGA_ADD_BRIGHT(c)   ((c) | (1u << 3))

    enum color
    {
        black = 0,
        blue = 1,
        green = 2,
        cyan = 3,
        red = 4,
        magenta = 5,
        brown = 6,
        light_gray = 7,
        dark_gray = VGA_ADD_BRIGHT(black),
        light_blue = VGA_ADD_BRIGHT(blue),
        light_green = VGA_ADD_BRIGHT(green),
        light_cyan = VGA_ADD_BRIGHT(cyan),
        light_red = VGA_ADD_BRIGHT(red),
        pink = VGA_ADD_BRIGHT(magenta),
        yellow = VGA_ADD_BRIGHT(brown),
        white = VGA_ADD_BRIGHT(light_gray),
    };

#undef VGA_ADD_BRIGHT

    namespace details
    {
        template <uint16_t Shift>
        class color_attribute
        {
        private:
            static constexpr const uint16_t shift = Shift;

        public:
            constexpr explicit color_attribute(color c) noexcept: _color(c)
            {
            }

            constexpr explicit operator uint16_t() const noexcept
            {
                return static_cast<uint16_t>(_color) << shift;
            }

        private:
            color _color;
        };
    }

    using text_color = details::color_attribute<8>;
    using background_color = details::color_attribute<12>;

    class screen_character
    {
    public:
        constexpr screen_character(char character, background_color bkgd_color, text_color txt_color) noexcept :
            _character(character), _background(bkgd_color), _text(txt_color)
        {
        }

        constexpr uint16_t raw_value() const noexcept
        {
            return _character | static_cast<uint16_t>(_background) | static_cast<uint16_t>(_text);
        }

    private:
        char _character;
        background_color _background;
        text_color _text;
    };

    class screen : public utils::singleton<screen>
    {
    public:
        constexpr unsigned int width() const noexcept
        {
            return 80;
        }

        constexpr unsigned int height() const noexcept
        {
            return 25;
        }

        volatile uint16_t *raw() noexcept
        {
            return (volatile uint16_t *)0xb8000;
        }

        class screen_slot
        {
        public:
            explicit screen_slot(volatile uint16_t &slot) noexcept : _slot(slot)
            {
            }

            screen_slot &operator=(const screen_character &c) noexcept
            {
                _slot = c.raw_value();
                return *this;
            }

        private:
            volatile uint16_t &_slot;
        };

        class screen_line
        {
        public:
            constexpr screen_line(screen &buf, unsigned int line) noexcept : _buf(buf), _line(line)
            {
            }

            auto operator[](vga::x column) noexcept
            {
                kassert(column.value() < _buf.width(), "VGA: out of bounds access on X axis");
                return screen_slot(_buf.raw()[_line * _buf.width() + column.value()]);
            }

        private:
            screen &_buf;
            unsigned int _line;
        };

        auto operator[](vga::y line) noexcept
        {
            kassert(line.value() < height(), "VGA: out of bounds access on Y axis");
            return screen_line(*this, line.value());
        }

        void clear(background_color bkgd = background_color(black)) noexcept
        {
            const auto value = screen_character(' ', bkgd, text_color(white));

            for (unsigned int line = 0; line < height(); ++line) {
                for (unsigned int col = 0; col < width(); ++col) {
                    (*this)[vga::y(line)][vga::x(col)] = value;
                }
            }
        }
    };
}

#endif /* !FOROS_VGA_SCREEN_HPP */
