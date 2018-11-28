/*
** Created by doom on 27/11/18.
*/

#ifndef FOROS_SCANCODE_SET_HPP
#define FOROS_SCANCODE_SET_HPP

#include <cstdint>
#include <utils/map.hpp>
#include <keyboard/keys.hpp>

namespace foros::kbd
{
    /**
     * Class to recognize scan codes for the first Scan Code Set
     *
     * See https://wiki.osdev.org/PS/2_Keyboard#Scan_Code_Set_1
     */
    class scan_code_set1_recognizer
    {
    public:
        enum state
        {
            clear = 0,
            released = 1 << 0,
            extended = 1 << 1,
            ready = 1 << 2,
        };

    private:
        constexpr bool is_released_code(uint8_t byte) const noexcept
        {
            return (byte & (1 << 7)) != 0;
        }

        constexpr bool is_extended_sequence_prefix(uint8_t byte) const noexcept
        {
            return byte == 0xe0;
        }

    public:
        void reset() noexcept
        {
            _state = clear;
        }

        void add_input(uint8_t byte) noexcept
        {
            if (!is_extended_sequence_prefix(byte)) {
                _state = (enum state)(_state | state::ready);
            } else {
                _state = (enum state)(_state | state::extended);
            }
            if (is_released_code(byte)) {
                _state = (enum state)(_state | state::released);
                byte &= 0x7f;
            }
            _last_byte = byte;
        }

        enum state state() const noexcept
        {
            return _state;
        }

        key_code get_keycode() const noexcept
        {
            const uint8_t code = _last_byte & 0x7f;

            if (!(state() & state::extended)) {
                return _scan_to_keycode[code];
            } else {
                return _extended_scan_to_keycode[code];
            }
        }

    private:
        uint8_t _last_byte{0};
        enum state _state{clear};

        static constexpr utils::map<uint8_t, key_code, 67> _scan_to_keycode{
            {0x01, key_code::escape},
            {0x02, key_code::key1},
            {0x03, key_code::key2},
            {0x04, key_code::key3},
            {0x05, key_code::key4},
            {0x06, key_code::key5},
            {0x07, key_code::key6},
            {0x08, key_code::key7},
            {0x09, key_code::key8},
            {0x0a, key_code::key9},
            {0x0b, key_code::key0},
            {0x0c, key_code::minus},
            {0x0d, key_code::equals},
            {0x0e, key_code::backspace},
            {0x0f, key_code::tab},
            {0x10, key_code::q},
            {0x11, key_code::w},
            {0x12, key_code::e},
            {0x13, key_code::r},
            {0x14, key_code::t},
            {0x15, key_code::y},
            {0x16, key_code::u},
            {0x17, key_code::i},
            {0x18, key_code::o},
            {0x19, key_code::p},
            {0x1a, key_code::left_square_bracket},
            {0x1b, key_code::right_square_bracket},
            {0x1c, key_code::enter},
            {0x1d, key_code::left_control},
            {0x1e, key_code::a},
            {0x1f, key_code::s},
            {0x20, key_code::d},
            {0x21, key_code::f},
            {0x22, key_code::g},
            {0x23, key_code::h},
            {0x24, key_code::j},
            {0x25, key_code::k},
            {0x26, key_code::l},
            {0x27, key_code::semi_colon},
            {0x28, key_code::single_quote},
            {0x29, key_code::back_quote},
            {0x2a, key_code::left_shift},
            {0x2b, key_code::backslash},
            {0x2c, key_code::z},
            {0x2d, key_code::x},
            {0x2e, key_code::c},
            {0x2f, key_code::v},
            {0x30, key_code::b},
            {0x31, key_code::n},
            {0x32, key_code::m},
            {0x33, key_code::comma},
            {0x34, key_code::dot},
            {0x35, key_code::slash},
            {0x36, key_code::right_shift},
            {0x38, key_code::left_alt},
            {0x39, key_code::spacebar},
            {0x3a, key_code::caps_lock},
            {0x3b, key_code::F1},
            {0x3c, key_code::F2},
            {0x3d, key_code::F3},
            {0x3e, key_code::F4},
            {0x3f, key_code::F5},
            {0x40, key_code::F6},
            {0x41, key_code::F7},
            {0x42, key_code::F8},
            {0x43, key_code::F9},
            {0x44, key_code::F10},
            {0x57, key_code::F11},
            {0x58, key_code::F12},
        };

        static constexpr utils::map<uint8_t, key_code, 13> _extended_scan_to_keycode{
            {0x1d, key_code::right_control},
            {0x38, key_code::right_alt},
            {0x47, key_code::home},
            {0x48, key_code::up_arrow},
            {0x49, key_code::page_up},
            {0x4b, key_code::left_arrow},
            {0x4d, key_code::right_arrow},
            {0x4f, key_code::end},
            {0x50, key_code::down_arrow},
            {0x51, key_code::page_down},
            {0x52, key_code::insert},
            {0x5b, key_code::_delete},
            {0x38, key_code::right_alt},
        };
    };
}

#endif /* !FOROS_SCANCODE_SET_HPP */
