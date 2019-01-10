/*
** Created by doom on 27/11/18.
*/

#include <tuple>
#include <core/halted_loop.hpp>
#include <vga/vga.hpp>
#include <keyboard/key_event_recognizer.hpp>
#include <keyboard/input_mapper.hpp>

using namespace foros;

enum syscall_id
{
    write = 0,
};

template <typename ...Args>
static long syscall(syscall_id id, std::tuple<Args ...> args)
{
    constexpr auto nb_args = sizeof...(Args);

    if constexpr (nb_args == 6) {
        asm volatile("mov %0, %%r9"::"r"((long)(std::get<5>(args))));
    }

    if constexpr (nb_args >= 5) {
        asm volatile("mov %0, %%r8"::"r"((long)(std::get<4>(args))));
    }

    if constexpr (nb_args >= 4) {
        asm volatile("mov %0, %%rcx"::"r"((long)(std::get<3>(args))));
    }

    if constexpr (nb_args >= 3) {
        asm volatile("mov %0, %%rdx"::"r"((long)(std::get<2>(args))));
    }

    if constexpr (nb_args >= 2) {
        asm volatile("mov %0, %%rsi"::"r"((long)(std::get<1>(args))));
    }

    if constexpr (nb_args >= 1) {
        asm volatile("mov %0, %%rdi"::"r"((long)(std::get<0>(args))));
    }

    long ret;

    asm volatile(
    "mov %1, %%rax;"
    "int $0x80;"
    "mov %%rax, %0"
    : "=a"(ret)
    : "r"((long)id)
    );
    return ret;
}

static long monitor_write(const char *buf, size_t size)
{
    return syscall(syscall_id::write, std::make_tuple(buf, size));
}

void fake_init_main()
{
    monitor_write("hello\n", 6);

    halted_loop([]() {
        auto ev_opt = kbd::key_event_recognizer::instance().get_next_event();

        while (ev_opt) {
            auto[ev_code, ev_state] = ev_opt.unwrap();

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
                    auto chr_opt = kbd::input_mapper::instance().add_event(ev_opt.unwrap());

                    if (chr_opt) {
                        vga::scrolling_printer() << chr_opt.unwrap();
                    }
                    break;
                }
            }
            ev_opt = kbd::key_event_recognizer::instance().get_next_event();
        }
    });
}
