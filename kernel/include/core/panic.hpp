/*
** Created by doom on 11/10/18.
*/

#ifndef FOROS_PANIC_HPP
#define FOROS_PANIC_HPP

#include <core/compiler_hints.hpp>

namespace foros
{
    [[noreturn]] void panic(const char *reason) noexcept;

#define kassert(cond, reason)                                               \
    do {                                                                    \
        if unlikely(!(cond))                                                \
            panic(reason);                                                 \
    } while (0);
}

#endif /* !FOROS_PANIC_HPP */
