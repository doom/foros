/*
** Created by doom on 12/10/18.
*/

#ifndef FOROS_COMPILER_HINTS_HPP
#define FOROS_COMPILER_HINTS_HPP

#define likely(x)       (__builtin_expect(static_cast<bool>(x), true))
#define unlikely(x)     (__builtin_expect(static_cast<bool>(x), false))

#define force_inline    __attribute__((always_inline)) inline
#define no_inline       __attribute__((noinline))

#endif /* !FOROS_COMPILER_HINTS_HPP */
