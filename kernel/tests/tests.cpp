/*
** Created by doom on 28/10/18.
*/

#include <vga/vga.hpp>
#include <multiboot2/multiboot2.hpp>
#include "tests_config.hpp"

void run_bit_field_tests();
void run_optional_tests();
void run_physical_frame_allocator_tests();

void run_tests(const multiboot2::boot_information &boot_info)
{
    foros::vga::scrolling_printer() << "Running tests...\n";

    tests_context::instance().set_boot_information(boot_info);

    run_bit_field_tests();
    run_optional_tests();
    run_physical_frame_allocator_tests();

    foros::vga::scrolling_printer() << "All tests passed\n";
}
