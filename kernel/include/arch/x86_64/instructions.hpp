/*
** Created by doom on 28/10/18.
*/

#ifndef FOROS_X86_64_INSTRUCTIONS_HPP
#define FOROS_X86_64_INSTRUCTIONS_HPP

#include <cstdint>

namespace foros::x86_64::instructions
{
    struct [[gnu::packed]] descriptor_table
    {
        std::uint16_t limit;
        std::uintptr_t base_ptr;
    };

    inline void lidt(const descriptor_table &desc) noexcept
    {
        asm("lidt (%0)"
        :
        : "r"(&desc)
        : "memory"
        );
    }
}

#endif /* !FOROS_X86_64_INSTRUCTIONS_HPP */
