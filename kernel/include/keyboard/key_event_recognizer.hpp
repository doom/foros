/*
** Created by doom on 20/11/18.
*/

#ifndef FOROS_KEYBOARD_EVENT_PRODUCER_HPP
#define FOROS_KEYBOARD_EVENT_PRODUCER_HPP

#include <cstdint>
#include <utils/singleton.hpp>
#include <utils/optional.hpp>
#include <keyboard/keys.hpp>
#include <keyboard/scancode_set.hpp>

namespace foros::kbd
{
    /**
     * Class processing keyboard scan codes as signaled by the PIC and producing key events
     *
     * This class is not aware of the keyboard layout, only of the physical keys.
     * The decoding is handled by a dedicated scan code set recognizer
     */
    class key_event_recognizer : public utils::singleton<key_event_recognizer>
    {
    public:
        using recognizer = scan_code_set1_recognizer;

        /**
         * Add a byte to the current decoding process
         *
         * @param byte          the byte to add
         */
        void add_byte(uint8_t byte) noexcept
        {
            _rec.add_input(byte);
        }

        /**
         * Get the next keyboard event if one is available
         *
         * @return              if an event is available, an optional containing that event
         *                      otherwise, nullopt
         */
        utils::optional<key_event> get_next_event() noexcept
        {
            if (_rec.state() & recognizer::state::ready) {
                key_event ev;

                ev.code = _rec.get_keycode();
                if (_rec.state() & recognizer::state::released) {
                    ev.state = key_state::up;
                } else {
                    ev.state = key_state::down;
                }
                _rec.reset();
                return {ev};
            }
            return std::nullopt;
        }

    private:
        recognizer _rec;
    };
}

#endif /* !FOROS_KEYBOARD_EVENT_PRODUCER_HPP */
