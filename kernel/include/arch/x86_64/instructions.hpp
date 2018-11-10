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

    inline void outb(uint8_t value, uint16_t port) noexcept
    {
        asm volatile(
        "outb %0, %1"
        : /* no output operands */
        :
        "a"(value), /* send value through the al register */
        "dN"(port)  /* send port through the dx register */
        );
    }

    inline uint8_t inb(uint16_t port) noexcept
    {
        uint8_t value;

        asm volatile(
        "inb %1, %0"
        : "=a"(value) /* receive value through the al register */
        : "dN"(port)  /* send port through the dx register */
        );
        return value;
    }

    inline void outw(uint16_t value, uint16_t port) noexcept
    {
        asm volatile(
        "outw %0, %1"
        : /* no output operands */
        :
        "a"(value), /* send value through the ax register */
        "dN"(port)  /* send port through the dx register */
        );
    }

    inline uint16_t inw(uint16_t port) noexcept
    {
        uint16_t value;

        asm volatile(
        "inw %1, %0"
        : "=a"(value) /* receive value through the ax register */
        : "dN"(port)  /* send port through the dx register */
        );
        return value;
    }

    inline void outl(uint32_t value, uint16_t port) noexcept
    {
        asm volatile(
        "outl %0, %1"
        : /* no output operands */
        :
        "a"(value), /* send value through the eax register */
        "dN"(port)  /* send port through the dx register */
        );
    }

    inline uint32_t inl(uint16_t port) noexcept
    {
        uint32_t value;

        asm volatile(
        "inl %1, %0"
        : "=a"(value) /* receive value through the eax register */
        : "dN"(port)  /* send port through the dx register */
        );
        return value;
    }

    /** Set interrupt flag */
    inline void sti() noexcept
    {
        asm volatile(
        "sti"
        );
    }

    /** Clear interrupt flag */
    inline void cli() noexcept
    {
        asm volatile(
        "cli"
        );
    }

    inline void invlpg(uintptr_t addr) noexcept
    {
        asm volatile(
        "invlpg (%0)"
        : /* no output registers */
        : "r"(addr)
        : "memory"
        );
    }
}

#endif /* !FOROS_X86_64_INSTRUCTIONS_HPP */
