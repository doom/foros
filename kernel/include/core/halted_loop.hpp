/*
** Created by doom on 21/11/18.
*/

#ifndef FOROS_HALTED_LOOP_HPP
#define FOROS_HALTED_LOOP_HPP

namespace foros
{
    template <typename Cond, typename Func>
    void halted_loop(Cond &cond, Func &&f) noexcept
    {
        while (cond) {
            f(cond);
            asm volatile("hlt");
        }
    }

    template <typename Func>
    void halted_loop(Func &&f) noexcept
    {
        while (1) {
            f();
            asm volatile("hlt");
        }
    }
}

#endif /* !FOROS_HALTED_LOOP_HPP */
