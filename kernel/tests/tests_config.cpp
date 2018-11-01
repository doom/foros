/*
** Created by doom on 28/10/18.
*/

#include <vga/vga.hpp>
#include <stdio.h>

using namespace foros;

int vga_printf_impl(const char *fmt, ...)
{
    static auto printer = vga::printer(vga::x(0), vga::y(0));
    char buf[512];
    va_list ap;

    va_start(ap, fmt);
    int ret = vsnprintf(buf, sizeof(buf), fmt, ap);
    printer << buf;
    va_end(ap);
    return ret;
}
