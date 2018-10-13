/*
** Created by doom on 12/10/18.
*/

#include <core/panic.hpp>
#include <vga/vga.hpp>

namespace foros
{
    void panic(const char *reason) noexcept
    {
        using namespace vga::literals;

        auto printer = vga::printer(0_x, 0_y);
        printer << "[" << vga::text_color(vga::red) << "PANIC" << vga::text_color(vga::white) << "] ";
        printer << reason;
        while (1) {
        }
    }
}
