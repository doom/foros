/*
** Created by doom on 08/10/18.
*/

#include <string.h>
#include <type_traits>
#include <vga/vga.hpp>
#include <interrupts/idt.hpp>
#include <interrupts/handlers.hpp>

using namespace foros;
using namespace vga::literals;

static void setup_idt() noexcept
{
    vga::scrolling_printer() << "Setting up the IDT... ";

    idt::instance().set_handler<division_by_zero>(division_by_zero_handler);
    idt::instance().set_handler<breakpoint>(breakpoint_handler);
    idt::instance().set_handler<invalid_opcode>(invalid_opcode_handler);
    idt::instance().set_handler<double_fault>(double_fault_handler);
    idt::instance().set_handler<page_fault>(page_fault_handler);
    idt::instance().load();

    vga::scrolling_printer() << "Done\n";
}

void run_tests();

extern "C" void kmain(const void *)
{
    vga::screen::instance().clear(vga::background_color(vga::black));

    setup_idt();

    run_tests();
}
