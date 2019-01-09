/*
** Created by doom on 03/11/18.
*/

#ifndef FOROS_MEMORY_PHYSICAL_FRAME_HPP
#define FOROS_MEMORY_PHYSICAL_FRAME_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <st/st.hpp>
#include <multiboot2/multiboot2.hpp>
#include <memory/definitions.hpp>

namespace mb2 = multiboot2;

namespace foros::memory
{
    /** Class representing a physical memory frame suitable to be used as a page or page table */
    struct physical_frame :
        public st::type_base<std::size_t>,
        public st::traits::orderable<physical_frame>,
        public st::traits::addable<physical_frame, std::size_t>
    {
        using st::type_base<std::size_t>::type_base;

        static physical_frame for_address(physical_address addr) noexcept
        {
            return physical_frame(addr.value() / page_size);
        }

        physical_address start_address() const noexcept
        {
            return physical_address(page_size * value());
        }
    };

    /**
     * The physical frame allocator allocates physical frames from the mapped areas given by the multiboot information.
     *
     * Its logic is simple: it takes the frames in order from an area, and moves to the next when none are left.
     * The only tricky part is that it has to avoid allocating frames that overlap with our kernel space or the
     * multiboot information structure, since we don't want to overwrite them.
     */
    class physical_frame_allocator
    {
    protected:
        physical_frame_allocator(mb2::memory_area_iterator begin_it, mb2::memory_area_iterator end_it,
                                 physical_address kernel_start, physical_address kernel_end,
                                 physical_address multiboot_start, physical_address multiboot_end) noexcept :
            _area_it(begin_it), _area_end_it(end_it),
            _kernel_start(physical_frame::for_address(kernel_start)),
            _kernel_end(physical_frame::for_address(kernel_end)),
            _multiboot_start(physical_frame::for_address(multiboot_start)),
            _multiboot_end(physical_frame::for_address(multiboot_end))
        {
        }

    public:
        /**
         * In order to ensure that the allocator is not constructed with incorrect data, the only way to
         * instantiate it is by providing a multiboot information structure to this factory function.
         */
        static auto create(const mb2::boot_information &boot_info) noexcept
        {
            auto memory_map = boot_info.tag<mb2::memory_map_tag>();

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

            return physical_frame_allocator(memory_map.memory_areas_begin(), memory_map.memory_areas_end(),
                                            physical_address(kernel_start->start_address()),
                                            physical_address(kernel_end->end_address()),
                                            physical_address(boot_info.start_address()),
                                            physical_address(boot_info.end_address())
            );
        }

    protected:
        void _increment_area() noexcept
        {
            for (auto cur_area_it = _area_it; cur_area_it != _area_end_it; ++cur_area_it) {
                if (_area_it->start_address() < cur_area_it->start_address()) {
                    /** If the area has not been used yet */
                    auto start_addr = physical_address(cur_area_it->start_address());

                    if (physical_frame::for_address(start_addr) >= _next_frame) {
                        /** If the area comes before the last seen area (we process areas in order, so we want the first) */
                        _area_it = cur_area_it;
                        _next_frame = physical_frame::for_address(physical_address(_area_it->start_address()));
                    }
                }

            }
            _area_it = _area_end_it;
        }

    public:
        std::optional<physical_frame> allocate_frame() noexcept
        {
            while (true) {
                /** Return nullopt if we cannot find any area to allocate from */
                if (_area_it == _area_end_it)
                    return std::nullopt;

                auto candidate_frame = _next_frame;
                auto last_byte_in_area_addr = physical_address(_area_it->start_address() + _area_it->size() - 1);
                auto last_frame_in_current_area = physical_frame::for_address(last_byte_in_area_addr);

                if (candidate_frame > last_frame_in_current_area) {
                    /**
                     * Our next frame is greater than the last one of the current area.
                     * This means it would fall out of the area, so we need to find a new area to allocate from.
                     */
                    _increment_area();
                    return allocate_frame();
                } else if (_kernel_start <= candidate_frame && candidate_frame <= _kernel_end) {
                    /** The frame is inside the kernel memory, skip it */
                    _next_frame = _kernel_start + 1;
                } else if (_multiboot_start <= candidate_frame && candidate_frame <= _multiboot_end) {
                    /** The frame is inside the multiboot info memory, skip it */
                    _next_frame = _multiboot_end + 1;
                } else {
                    _next_frame = _next_frame + 1;
                    return candidate_frame;
                }
            }
        }

        void deallocate_frame(physical_frame) noexcept
        {
            /** No-op */
        }

    protected:
        mb2::memory_area_iterator _area_it;
        const mb2::memory_area_iterator _area_end_it;
        physical_frame _next_frame{0};
        const physical_frame _kernel_start;
        const physical_frame _kernel_end;
        const physical_frame _multiboot_start;
        const physical_frame _multiboot_end;
    };
}

#endif /* !FOROS_MEMORY_PHYSICAL_FRAME_HPP */
