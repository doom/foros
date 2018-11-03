/*
** Created by doom on 28/10/18.
*/

#ifndef FOROS_X86_64_REGISTERS_HPP
#define FOROS_X86_64_REGISTERS_HPP

#include <cstdint>

namespace foros::x86_64::registers
{
    inline std::uint16_t cs() noexcept
    {
        std::uint16_t ret;

        asm("mov %%cs, %0"
        : "=r"(ret)
        );
        return ret;
    }

    inline std::uintptr_t cr2() noexcept
    {
        std::uintptr_t ret;

        asm("mov %%cr2, %0"
        : "=r"(ret)
        );
        return ret;
    }
}

#endif /* !FOROS_X86_64_REGISTERS_HPP */
