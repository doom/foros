/*
** Created by doom on 08/10/18.
*/

#include <string.h>
#include <type_traits>
#include <vga/vga.hpp>
#include <core/panic.hpp>

extern "C"
{

using namespace foros;
using namespace vga::literals;

void kmain()
{
    vga::screen::instance().clear(vga::background_color(vga::black));
    vga::printer(0_x, 0_y) << "ForOS";
}

}
