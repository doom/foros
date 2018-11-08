/*
** Created by doom on 28/10/18.
*/

#ifndef FOROS_TESTS_CONFIG_HPP
#define FOROS_TESTS_CONFIG_HPP

#include <optional>
#include <multiboot2/multiboot2.hpp>
#include <utils/singleton.hpp>
#include <core/panic.hpp>

int vga_printf_impl(const char *fmt, ...);

#define ut_printf(fmt, ...)         vga_printf_impl(fmt, ##__VA_ARGS__)

#define PRINT_RED
#define PRINT_GREEN
#define PRINT_CYAN
#define PRINT_WHITE

#define ut_set_color(color)

#define ut_on_fail(x)               foros::panic("Tests failed, aborting");

#include <ut_tests.h>

class tests_context : public utils::singleton<tests_context>
{
public:
    void set_boot_information(const multiboot2::boot_information &info) noexcept
    {
        _boot_info = info;
    }

    const multiboot2::boot_information &boot_information() noexcept
    {
        return *_boot_info;
    }

private:
    std::optional<multiboot2::boot_information> _boot_info;
};

#endif /* !FOROS_TESTS_CONFIG_HPP */
