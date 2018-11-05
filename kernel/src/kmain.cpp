/*
** Created by doom on 08/10/18.
*/

#include <string.h>
#include <type_traits>
#include <vga/vga.hpp>
#include <interrupts/idt.hpp>
#include <interrupts/handlers.hpp>
#include <multiboot2/multiboot2.hpp>

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
    idt::instance().load();

    vga::scrolling_printer() << "Done\n";
}

static void debug_infos(const mb2::boot_information &boot_info) noexcept
{
    vga::scrolling_printer() << "Mapped memory regions:\n";
    for (auto it = boot_info.tags_begin<mb2::memory_map_tag>();
         it != boot_info.tags_end<mb2::memory_map_tag>();
         ++it) {
        for (auto area_it = it->memory_areas_begin(); area_it != it->memory_areas_end(); ++area_it) {
            vga::scrolling_printer() << area_it->start_address() << ", "
                                     << area_it->end_address() << ", "
                                     << area_it->size() << '\n';
        }
    }

    auto elf_sect_it = boot_info.elf_sections_tag().sections_begin();
    auto elf_sect_end = boot_info.elf_sections_tag().sections_end();

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

void run_tests();

extern "C" void kmain(const void *ptr)
{
    vga::screen::instance().clear(vga::background_color(vga::black));

    setup_idt();

    run_tests();

    mb2::boot_information boot_info((const std::byte *)ptr);

    auto boot_loader_name_tag = boot_info.boot_loader_name_tag();
    vga::scrolling_printer() << "Bootloader: " << boot_loader_name_tag.name() << '\n';

    auto cmd_line_tag = boot_info.command_line_tag();
    vga::scrolling_printer() << "Arguments: \"" << cmd_line_tag.arguments() << "\"\n";

    debug_infos(boot_info);
}
