/*
** Created by doom on 06/11/18.
*/

#ifndef FOROS_MASKABLE_INTERRUPTS_HPP
#define FOROS_MASKABLE_INTERRUPTS_HPP

#include <arch/x86_64/instructions.hpp>

namespace foros
{
    namespace arch = x86_64;

    inline void enable_maskable_interrupts() noexcept
    {
        arch::instructions::sti();
    }

    inline void ignore_maskable_interrupts() noexcept
    {
        arch::instructions::cli();
    }
}

#endif /* !FOROS_MASKABLE_INTERRUPTS_HPP */
