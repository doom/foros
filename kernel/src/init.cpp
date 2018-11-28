/*
** Created by doom on 27/11/18.
*/

#include <core/halted_loop.hpp>
#include <vga/vga.hpp>
#include <keyboard/key_event_recognizer.hpp>
#include <keyboard/input_mapper.hpp>

using namespace foros;

void fake_init_main()
{
    halted_loop([]() {
        auto ev_opt = kbd::key_event_recognizer::instance().get_next_event();

        while (ev_opt) {
            auto[ev_code, ev_state] = *ev_opt;

            switch (ev_code) {
                case kbd::key_code::enter:
                    if (ev_state == kbd::key_state::down) {
                        vga::scrolling_printer() << "\n";
                    }
                    break;
                case kbd::key_code::backspace:
                    if (ev_state == kbd::key_state::down) {
                        vga::scrolling_printer() << "\b \b";
                    }
                    break;
                default: {
                    auto chr_opt = kbd::input_mapper::instance().add_event(*ev_opt);

                    if (chr_opt) {

                        vga::scrolling_printer() << *chr_opt;
                    }
                }
                    break;
            }
            ev_opt = kbd::key_event_recognizer::instance().get_next_event();
        }
    });
}
