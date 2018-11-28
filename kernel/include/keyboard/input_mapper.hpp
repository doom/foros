/*
** Created by doom on 21/11/18.
*/

#ifndef FOROS_INPUT_MAPPER_HPP
#define FOROS_INPUT_MAPPER_HPP

#include <utils/optional.hpp>
#include <utils/singleton.hpp>
#include <utils/map.hpp>
#include <keyboard/keys.hpp>
#include <keyboard/layout.hpp>

namespace foros::kbd
{
    /**
     * Class responsible to mapping key events to characters
     *
     * This class is aware of the keyboard layout, and uses it to convert the raw key events
     * to characters as specified in the layout
     */
    class input_mapper : public utils::singleton<input_mapper>
    {
    public:
        /**
         * Add an event and get the resulting character, if any
         *
         * @param ev            the event to add
         *
         * @return              if the event produces a character, an optional containing that character
         *                      otherwise, nullopt
         */
        utils::optional<char> add_event(key_event ev) noexcept
        {
            switch (ev.code) {
                case key_code::left_shift:
                    _left_shifted = ev.state == key_state::down;
                    break;
                case key_code::right_shift:
                    _right_shifted = ev.state == key_state::down;
                    break;
                case key_code::caps_lock:
                    _caps_locked = !_caps_locked;
                    break;
                default:
                    if (ev.state == key_state::down) {
                        if (_layout.contains(ev.code)) {
                            return {_layout[ev.code]};
                        }
                    }
                    break;
            }
            return std::nullopt;
        }

        void set_layout(layout lay)
        {
            _layout = lay;
        }

    private:
        bool _left_shifted{false};
        bool _right_shifted{false};
        bool _caps_locked{false};
        layout _layout{azerty_layout};
    };
}

#endif /* !FOROS_INPUT_MAPPER_HPP */
