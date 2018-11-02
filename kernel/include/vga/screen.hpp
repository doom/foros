/*
** Created by doom on 08/10/18.
*/

#ifndef FOROS_VGA_SCREEN_HPP
#define FOROS_VGA_SCREEN_HPP

#include <stdint.h>
#include <st/st.hpp>
#include <utils/singleton.hpp>
#include <core/panic.hpp>

namespace foros::vga
{
    using x = st::type<unsigned int, struct x_coord_tag, st::arithmetic>;
    using y = st::type<unsigned int, struct y_coord_tag, st::arithmetic>;

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

    /**
     * Class representing the computer screen as usable through the VGA text buffer
     */
    class screen : public utils::singleton<screen>
    {
    public:
        constexpr vga::x width() const noexcept
        {
            return vga::x{80};
        }

        constexpr vga::y height() const noexcept
        {
            return vga::y{25};
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
            constexpr screen_line(screen &buf, vga::y line) noexcept : _buf(buf), _line(line)
            {
            }

            auto operator[](vga::x x) noexcept
            {
                kassert(x < _buf.width(), "VGA: out of bounds access on X axis");
                return screen_slot(_buf.raw()[_line.value() * _buf.width().value() + x.value()]);
            }

        private:
            screen &_buf;
            vga::y _line;
        };

        auto operator[](vga::y y) noexcept
        {
            kassert(y < height(), "VGA: out of bounds access on Y axis");
            return screen_line(*this, y);
        }

        void clear(background_color bkgd = background_color(black)) noexcept
        {
            using namespace vga::literals;
            const auto value = screen_character(' ', bkgd, text_color(white));

            for (auto y = 0_y; y < height(); ++y) {
                for (auto x = 0_x; x < width(); ++x) {
                    (*this)[y][x] = value;
                }
            }
        }

        void scroll() noexcept
        {
            using namespace vga::literals;

            volatile uint16_t *dest = raw();
            volatile uint16_t *src = dest + width().value();
            while (src < raw() + width().value() * height().value()) {
                *dest++ = *src++;
            }

            constexpr auto value = screen_character(' ', background_color(black), text_color(white));
            for (auto x = 0_x; x < width(); ++x) {
                (*this)[height() - 1_y][x] = value;
            }
        }
    };
}

#endif /* !FOROS_VGA_SCREEN_HPP */
