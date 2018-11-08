/*
** Created by doom on 03/11/18.
*/

#ifndef FOROS_MEMORY_DEFINITIONS_HPP
#define FOROS_MEMORY_DEFINITIONS_HPP

#include <cstddef>
#include <cstdint>
#include <st/st.hpp>

namespace foros::memory
{
    static constexpr const std::size_t page_size = 4096;
    static constexpr const std::size_t page_entries_count = 512;

    using physical_address = st::type<uintptr_t, st::arithmetic>;
    using virtual_address = st::type<uintptr_t, st::arithmetic>;

    static_assert(sizeof(physical_address) == sizeof(uintptr_t));
    static_assert(sizeof(virtual_address) == sizeof(uintptr_t));
}

#endif /* !FOROS_MEMORY_DEFINITIONS_HPP */
