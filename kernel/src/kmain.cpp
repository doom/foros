/*
** Created by doom on 08/10/18.
*/

#include <string.h>
#include <type_traits>
#include <vga/vga.hpp>

using namespace foros;
using namespace vga::literals;

void run_tests();

extern "C" void kmain(const void *)
{
    vga::screen::instance().clear(vga::background_color(vga::black));

    run_tests();
}
