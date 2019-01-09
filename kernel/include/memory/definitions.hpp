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

    struct physical_address :
        public st::type_base<uintptr_t>,
        public st::traits::arithmetic<physical_address>,
        public st::traits::addable<physical_address, std::size_t>
    {
        using st::type_base<uintptr_t>::type_base;

        explicit physical_address(const void *ptr) noexcept : physical_address((uintptr_t)ptr)
        {
        }

        explicit physical_address(const std::byte *ptr) noexcept : physical_address((uintptr_t)ptr)
        {
        }
    };

    using virtual_address = st::type<
        uintptr_t,
        struct virtual_address_tag,
        st::arithmetic,
        st::addable_with<std::size_t>
    >;

    static_assert(sizeof(physical_address) == sizeof(uintptr_t));
    static_assert(sizeof(virtual_address) == sizeof(uintptr_t));
}

#endif /* !FOROS_MEMORY_DEFINITIONS_HPP */
