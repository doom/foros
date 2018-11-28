/*
** Created by doom on 08/10/18.
*/

#include <string.h>
#include <type_traits>
#include <vga/vga.hpp>
#include <interrupts/idt.hpp>
#include <interrupts/handlers.hpp>
#include <multiboot2/multiboot2.hpp>
#include <interrupts/interrupts.hpp>
#include <memory/kernel_heap.hpp>

using namespace foros;
using namespace vga::literals;
namespace mb2 = multiboot2;

static void setup_idt() noexcept
{
    vga::scrolling_printer() << "Setting up the IDT... ";

    idt::instance().set_handler<division_by_zero>(division_by_zero_handler);
    idt::instance().set_handler<breakpoint>(breakpoint_handler);
    idt::instance().set_handler<invalid_opcode>(invalid_opcode_handler);
    idt::instance().set_handler<double_fault>(double_fault_handler);
    idt::instance().set_handler<page_fault>(page_fault_handler);
    idt::instance().set_handler<pit_interrupt>(pit_interrupt_handler);
    idt::instance().set_handler<keyboard_interrupt>(keyboard_interrupt_handler);
    idt::instance().set_default_handler(&handle_any_interrupt);
    idt::instance().load();

    pic_8259::instance().remap();

    enable_maskable_interrupts();

    vga::scrolling_printer() << "Done\n";
}

static void setup_memory(const mb2::boot_information &boot_info) noexcept
{
    vga::scrolling_printer() << "Setting up the kernel heap... ";
    memory::kernel_heap::instance().initialize(boot_info,
                                               memory::virtual_address(0x40000000),
                                               memory::virtual_address(0x40020000));
    vga::scrolling_printer() << "Done\n";
}

static void debug_infos(const mb2::boot_information &boot_info) noexcept
{
    auto boot_loader_name_tag = boot_info.tag<mb2::boot_loader_name_tag>();
    vga::scrolling_printer() << "Bootloader: " << boot_loader_name_tag.name() << '\n';

    auto cmd_line_tag = boot_info.tag<mb2::command_line_tag>();
    vga::scrolling_printer() << "Arguments: \"" << cmd_line_tag.arguments() << "\"\n";

    vga::scrolling_printer() << "Mapped memory regions:\n";
    auto memory_map = boot_info.tag<mb2::memory_map_tag>();
    for (auto area_it = memory_map.memory_areas_begin(); area_it != memory_map.memory_areas_end(); ++area_it) {
        vga::scrolling_printer() << area_it->start_address() << ", "
                                 << area_it->end_address() << ", "
                                 << area_it->size() << '\n';
    }

    auto elf_sect_it = boot_info.tag<mb2::elf_sections_tag>().sections_begin();
    auto elf_sect_end = boot_info.tag<mb2::elf_sections_tag>().sections_end();

    auto kernel_start = std::min_element(elf_sect_it, elf_sect_end, [](const mb2::elf_section &a,
                                                                       const mb2::elf_section &b) {
        return a.start_address() < b.start_address();
    });
    auto kernel_end = std::max_element(elf_sect_it, elf_sect_end, [](const mb2::elf_section &a,
                                                                     const mb2::elf_section &b) {
        return a.start_address() + a.size() < b.start_address() + b.size();
    });
    vga::scrolling_printer() << "Kernel start: " << kernel_start->start_address()
                             << ", end: " << kernel_end->end_address() << '\n';
}

void run_tests(const mb2::boot_information &);

void fake_init_main();

extern "C" void kmain(const void *ptr)
{
    vga::screen::instance().clear(vga::background_color(vga::black));
    mb2::boot_information boot_info((const std::byte *)ptr);
    debug_infos(boot_info);

    setup_idt();
    setup_memory(boot_info);

    run_tests(boot_info);

    fake_init_main();
}
