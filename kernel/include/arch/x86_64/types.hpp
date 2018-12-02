/*
** Created by doom on 01/12/18.
*/

#ifndef FOROS_X86_64_TYPES_HPP
#define FOROS_X86_64_TYPES_HPP

#include <cstdint>

namespace foros::x86_64::types
{
    struct [[gnu::packed]] descriptor_table
    {
        std::uint16_t limit;
        std::uintptr_t base_ptr;
    };
}

#endif /* !FOROS_X86_64_TYPES_HPP */
