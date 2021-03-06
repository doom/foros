/*
** Created by doom on 28/10/18.
*/

#include <arch/x86_64/registers.hpp>
#include <vga/vga.hpp>
#include <core/panic.hpp>
#include <interrupts/exceptions.hpp>
#include <interrupts/pic.hpp>
#include <keyboard/key_event_recognizer.hpp>
#include <stdarg.h>

/**
 * Below are the implementations of default exception handlers.
 *
 * Builtin interrupt mode (enabled if FOROS_USE_BUILTIN_INTERRUPT is defined):
 * We use the builtin interrupt function attribute to handle the work for us.
 * As a result, the stack is properly aligned, and the pointer to the stack frame (and the
 * error code if any) are passed directly to our handler wrapper.
 * All we have to do is call the real handler.
 *
 * Manual mode:
 * Since the calling convention for interrupt handlers is different from the default x86_64
 * convention, intermediate naked functions are used to handle the interrupt.
 * These intermediate functions are required in order to ensure the stack is properly aligned
 * and to fetch the exception stack frame and the optional error code and pass them to
 * our real handlers.
 */


/** Save the general purpose registers (pushes 9 8-bytes registers to the stack) */
#define save_general_registers()                                                                    \
    asm volatile(                                                                                   \
    "push %rax;"                                                                                    \
    "push %rdi;"                                                                                    \
    "push %rsi;"                                                                                    \
    "push %rdx;"                                                                                    \
    "push %rcx;"                                                                                    \
    "push %r8;"                                                                                     \
    "push %r9;"                                                                                     \
    "push %r10;"                                                                                    \
    "push %r11;"                                                                                    \
    );

/** Restore the general purpose registers (pops 9 8-bytes registers to the stack) */
#define restore_general_registers()                                                                 \
    asm volatile(                                                                                   \
    "pop %r11;"                                                                                     \
    "pop %r10;"                                                                                     \
    "pop %r9;"                                                                                      \
    "pop %r8;"                                                                                      \
    "pop %rcx;"                                                                                     \
    "pop %rdx;"                                                                                     \
    "pop %rsi;"                                                                                     \
    "pop %rdi;"                                                                                     \
    "pop %rax;"                                                                                     \
    );

#ifdef FOROS_USE_BUILTIN_INTERRUPT
#define define_handler(name)                                                                        \
    extern "C" void name##_next(const foros::exception_stack_frame *esf);                           \
                                                                                                    \
    extern "C" __attribute__((interrupt)) void name(const foros::exception_stack_frame *esf)        \
    {                                                                                               \
        name##_next(esf);                                                                           \
    }                                                                                               \
                                                                                                    \
    extern "C" void name##_next

#define define_handler_with_error_code(name)                                                        \
    extern "C" void name##_next(const foros::exception_stack_frame *esf, uint64_t err_code);        \
                                                                                                    \
    extern "C" __attribute__((interrupt)) void name(const foros::exception_stack_frame *esf,        \
                                                    uint64_t err_code)                              \
    {                                                                                               \
        name##_next(esf, err_code);                                                                 \
    }                                                                                               \
                                                                                                    \
    extern "C" void name##_next

#else /* FOROS_USE_BUILTIN_INTERRUPT is not defined, define handlers manually */
#define define_handler(name)                                                                        \
    extern "C" __attribute__((naked)) void name()                                                   \
    {                                                                                               \
        save_general_registers();                                                                   \
        asm volatile(                                                                               \
        "mov %%rsp, %%rdi;"                                                                         \
        "add $72, %%rdi;" /* we pushed 9 8-byte registers */                                        \
        "call " #name "_next;"                                                                      \
        : /* no output registers */                                                                 \
        : /* no input registers */                                                                  \
        : "%rdi" /* rdi is modified */                                                              \
        );                                                                                          \
        restore_general_registers();                                                                \
        asm volatile(                                                                               \
        "iretq;"                                                                                    \
        );                                                                                          \
    }                                                                                               \
                                                                                                    \
    extern "C" void name##_next

#define define_handler_with_error_code(name)                                                        \
    extern "C" __attribute__((naked)) void name()                                                   \
    {                                                                                               \
        save_general_registers();                                                                   \
        asm volatile(                                                                               \
        "mov 72(%%rsp), %%rsi;" /* rsp + 72, since we pushed 9 8-byte registers */                  \
        "mov %%rsp, %%rdi;"                                                                         \
        "add $80, %%rdi;"                                                                           \
        "sub $8, %%rsp;" /* we align the stack pointer on a 16-byte boundary */                     \
        "call " #name "_next;"                                                                      \
        "add $8, %%rsp;" /* remove the extra 8-byte alignment so we can pop saved registers */      \
        : /* no output registers */                                                                 \
        : /* no input registers */                                                                  \
        : "%rsi", "%rdi" /* rsi and rdi are modified */                                             \
        );                                                                                          \
        restore_general_registers();                                                                \
        asm volatile(                                                                               \
        "add $8, %rsp;"                                                                             \
        "iretq;"                                                                                    \
        );                                                                                          \
    }                                                                                               \
                                                                                                    \
    extern "C" void name##_next

#endif /* !FOROS_USE_BUILTIN_INTERRUPT */

using namespace foros;
namespace arch = foros::x86_64;

template <typename StreamT>
static inline StreamT &operator<<(StreamT &&s, const exception_stack_frame &esf) noexcept
{
    s << "{\n";
    s << "  instruction pointer: " << vga::text_color(vga::cyan)
      << esf.instruction_pointer << vga::text_color(vga::white) << '\n';
    s << "  code segment: " << vga::text_color(vga::cyan)
      << esf.code_segment << vga::text_color(vga::white) << '\n';
    s << "  rflags: " << vga::text_color(vga::cyan)
      << esf.rflags << vga::text_color(vga::white) << '\n';
    s << "  stack pointer: " << vga::text_color(vga::cyan)
      << esf.stack_pointer << vga::text_color(vga::white) << '\n';
    s << "  stack segment: " << vga::text_color(vga::cyan)
      << esf.stack_segment << vga::text_color(vga::white) << '\n';
    s << "}";
    return s;
}

inline constexpr page_fault_error_code operator&(page_fault_error_code a, page_fault_error_code b)
{
    return static_cast<page_fault_error_code>(static_cast<uint64_t>(a) & static_cast<uint64_t >(b));
}

template <typename StreamT>
static inline StreamT &operator<<(StreamT &&s, page_fault_error_code pfec) noexcept
{
    s << vga::text_color(vga::cyan);
    if ((pfec & page_fault_error_code::protection_violation) != page_fault_error_code::none) {
        s << "protection_violation";
    }
    if ((pfec & page_fault_error_code::caused_by_write) != page_fault_error_code::none) {
        s << "caused_by_write";
    } else {
        s << "caused_by_read";
    }
    if ((pfec & page_fault_error_code::user_mode) != page_fault_error_code::none) {
        s << "user_mode";
    }
    if ((pfec & page_fault_error_code::malformed_table) != page_fault_error_code::none) {
        s << "malformed_table";
    }
    if ((pfec & page_fault_error_code::instruction_fetch) != page_fault_error_code::none) {
        s << "instruction_fetch";
    }
    s << vga::text_color(vga::white);
    return s;
}

define_handler(handle_division_by_zero)(const exception_stack_frame *)
{
    panic("Division by zero detected");
}

define_handler(handle_breakpoint)(const exception_stack_frame *)
{
}

define_handler(handle_invalid_opcode)(const exception_stack_frame *)
{
    panic("Invalid opcode detected");
}

define_handler_with_error_code(handle_double_fault)(const exception_stack_frame *, uint64_t)
{
    panic("Double fault");
}

define_handler_with_error_code(handle_page_fault)(const exception_stack_frame *stack_frame, uint64_t err_code)
{
    const auto error_code = page_fault_error_code(err_code);

    vga::scrolling_printer() << "Page fault when accessing address: " << vga::text_color(vga::cyan)
                             << (void *)arch::registers::cr2() << vga::text_color(vga::white) << '\n';
    vga::scrolling_printer() << "error code: " << error_code << '\n';
    vga::scrolling_printer() << "stack frame: " << *stack_frame << '\n';
    panic("Page fault detected");
}

define_handler(handle_pit_interrupt)(const exception_stack_frame *)
{
    /** Ignore the timer for now */
    pic_8259::instance().send_end_of_interrupt(0x20);
}

define_handler(handle_keyboard_interrupt)(const exception_stack_frame *)
{
    pic_8259::instance().send_end_of_interrupt(0x21);
    constexpr cpu_port<uint8_t> in_port(0x60);

    kbd::key_event_recognizer::instance().add_byte(in_port.read_value());
}

struct syscall_registers
{
    uint64_t r9;
    uint64_t r8;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rax;
};

extern "C" void handle_syscall(syscall_registers r)
{
    if (r.rax == 0) {
        vga::scrolling_printer() << std::string_view{(const char *)r.rdi, r.rsi};
    }
}

// This doesn't use the "interrupt" attribute because we want to control what happens on the stack. Meh.
extern "C" __attribute__((naked)) void handle_syscall_interrupt()
{
    save_general_registers();

    asm volatile(
    "push %rax;"
    "push %rdi;"
    "push %rsi;"
    "push %rdx;"
    "push %rcx;"
    "push %r8;"
    "push %r9;"
    "call handle_syscall;"
    "pop %rdx;"
    "pop %rdx;"
    "pop %rdx;"
    "pop %rdx;"
    "pop %rdx;"
    "pop %rdx;"
    "pop %rdx;"
    );

    restore_general_registers();

    asm volatile(
    "iretq;"
    );
}

define_handler(handle_any_interrupt)(const exception_stack_frame *)
{
    panic("Unhandled interrupt");
}
