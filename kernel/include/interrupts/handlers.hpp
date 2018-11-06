/*
** Created by doom on 28/10/18.
*/

#ifndef FOROS_HANDLERS_HPP
#define FOROS_HANDLERS_HPP

#include <interrupts/idt.hpp>
#include <interrupts/exceptions.hpp>

extern "C" void handle_division_by_zero(const foros::exception_stack_frame *);
extern "C" void handle_breakpoint(const foros::exception_stack_frame *);
extern "C" void handle_invalid_opcode(const foros::exception_stack_frame *);
extern "C" void handle_double_fault(const foros::exception_stack_frame *, uint64_t);
extern "C" void handle_page_fault(const foros::exception_stack_frame *, uint64_t);

extern "C" void handle_any_interrupt(const foros::exception_stack_frame *);

namespace foros
{
    inline constexpr division_by_zero_handler_t division_by_zero_handler(&handle_division_by_zero);
    inline constexpr breakpoint_handler_t breakpoint_handler(&handle_breakpoint);
    inline constexpr invalid_opcode_handler_t invalid_opcode_handler(&handle_invalid_opcode);
    inline constexpr double_fault_handler_t double_fault_handler(&handle_double_fault);
    inline constexpr page_fault_handler_t page_fault_handler(&handle_page_fault);
}

#endif /* !FOROS_HANDLERS_HPP */
