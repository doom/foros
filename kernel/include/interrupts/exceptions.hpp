/*
** Created by doom on 01/11/18.
*/

#ifndef FOROS_EXCEPTIONS_HPP
#define FOROS_EXCEPTIONS_HPP

#include <cstdint>

namespace foros
{
    struct exception_stack_frame
    {
        void *instruction_pointer;
        uint64_t code_segment;
        uint64_t rflags;
        void *stack_pointer;
        uint64_t stack_segment;
    };

    enum class page_fault_error_code : uint64_t
    {
        none = 0,
        protection_violation = 1,
        caused_by_write = 1 << 1,
        user_mode = 1 << 2,
        malformed_table = 1 << 3,
        instruction_fetch = 1 << 4,
    };
}

#endif /* !FOROS_EXCEPTIONS_HPP */
