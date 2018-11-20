/*
** Created by doom on 10/11/18.
*/

#include <memory/kernel_heap.hpp>
#include <memory/paging.hpp>
#include <vga/scrolling_printer.hpp>

namespace foros::memory
{
    void kernel_heap::initialize(const multiboot2::boot_information &boot_info,
                                 virtual_address start_address,
                                 virtual_address end_address) noexcept
    {
        _start_addr = start_address;
        _end_addr = end_address;
        _current_addr = start_address;
        _frame_allocator.emplace(physical_frame_allocator::create(boot_info));

        if (start_address != end_address) {
            auto end_page = page::for_address(virtual_address(end_address.value() - 1));
            auto next_page = [](page p) {
                return page::for_address(virtual_address(p.start_address() + page_size));
            };

            for (auto page = page::for_address(start_address); page != end_page; page = next_page(page)) {
                mapper::map_page(page, page_table_entry::flags::writable, *_frame_allocator);
            }
        }
    }

    static virtual_address align_up(virtual_address base, size_t align)
    {
        return virtual_address((base.value() + (align - 1)) & ~(align - 1));
    }

    void *kernel_heap::allocate(size_t size, size_t align) noexcept
    {
        virtual_address ret = align_up(_current_addr, align);

        _current_addr = ret + size;
        if (_current_addr >= _end_addr) {
            return nullptr;
        }
        return (void *)ret.value();
    }

    void kernel_heap::deallocate(void *ptr, std::size_t size, std::size_t) noexcept
    {
        auto addr = virtual_address((uintptr_t)ptr);

        if (addr + size == _current_addr) {
            _current_addr = virtual_address(_current_addr.value() - size);
        }
    }
}