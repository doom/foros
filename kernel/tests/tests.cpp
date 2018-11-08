/*
** Created by doom on 28/10/18.
*/

#include <vga/vga.hpp>
#include "tests_config.hpp"

void run_bit_field_tests();
void run_optional_tests();

void run_tests()
{
    foros::vga::scrolling_printer() << "Running tests...\n";
    run_bit_field_tests();
    run_optional_tests();
    foros::vga::scrolling_printer() << "All tests passed\n";
}
