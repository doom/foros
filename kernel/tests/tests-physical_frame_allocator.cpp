/*
** Created by doom on 04/11/18.
*/

#include "tests_config.hpp"
#include <memory/physical_frame.hpp>

ut_test(usage)
{
    using namespace foros::memory;

    auto allocator = physical_frame_allocator::create(tests_context::instance().boot_information());

    auto elf_sect_it = tests_context::instance().boot_information().tag<mb2::elf_sections_tag>().sections_begin();
    auto elf_sect_end = tests_context::instance().boot_information().tag<mb2::elf_sections_tag>().sections_end();

    auto kern_start_frame = std::min_element(elf_sect_it, elf_sect_end, [](const mb2::elf_section &a,
                                                                           const mb2::elf_section &b) {
        return a.start_address() < b.start_address();
    });
    auto kern_end_frame = std::max_element(elf_sect_it, elf_sect_end, [](const mb2::elf_section &a,
                                                                         const mb2::elf_section &b) {
        return a.start_address() + a.size() < b.start_address() + b.size();
    });

    auto kern_start = physical_address(kern_start_frame->start_address());
    auto kern_end = physical_address(kern_end_frame->end_address());

    auto multiboot_start = physical_address(tests_context::instance().boot_information().start_address());
    auto multiboot_end = physical_address(tests_context::instance().boot_information().end_address());

    while (auto opt = allocator.allocate_frame()) {
        ut_assert_false(physical_frame::for_address(kern_start) < opt.unwrap()
                        && opt.unwrap() < physical_frame::for_address(kern_end));
        ut_assert_false(physical_frame::for_address(multiboot_start) < opt.unwrap()
                        && opt.unwrap() < physical_frame::for_address(multiboot_end));
    }
}

ut_group(physical_frame_allocator,
         ut_get_test(usage)
);

void run_physical_frame_allocator_tests()
{
    ut_run_group(ut_get_group(physical_frame_allocator));
}
