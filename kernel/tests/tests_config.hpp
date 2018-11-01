/*
** Created by doom on 28/10/18.
*/

#ifndef FOROS_TESTS_CONFIG_HPP
#define FOROS_TESTS_CONFIG_HPP

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

#endif /* !FOROS_TESTS_CONFIG_HPP */
