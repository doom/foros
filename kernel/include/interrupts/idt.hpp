/*
** Created by doom on 18/10/18.
*/

#ifndef FOROS_IDT_HPP
#define FOROS_IDT_HPP

#include <cstdint>
#include <type_traits>
#include <st/type.hpp>
#include <utils/bit_field.hpp>
#include <utils/singleton.hpp>
#include <arch/x86_64/registers.hpp>
#include <arch/x86_64/instructions.hpp>
#include <core/panic.hpp>
#include <interrupts/exceptions.hpp>

namespace arch = foros::x86_64;

namespace foros
{
    /**
     * The IDT entry options are packed in a 16-bit bitfield with the following meaning:
     *
     * 0-2      if 0, don't switch stacks otherwise switch to the n-th stack
     * 3-7      reserved
     * 8        if not set, interrupts are disabled when this handler is called.
     * 9-11     reserved, must have all bits set
     * 12       reserved, must be zero
     * 13‑14    DPL (Descriptor Privilege Level), the minimal privilege level required to use the handler
     * 15       if set, the entry is present, otherwise it is not
     */
    struct [[gnu::packed]] idt_entry_options
    {
    public:
        constexpr idt_entry_options() noexcept
        {
            value.set_bit_range<9, 12, 0b111>();
        }

        constexpr idt_entry_options &set_present() noexcept
        {
            value.set_bit<15>();
            return *this;
        }

        constexpr bool is_present() const noexcept
        {
            return value.get_bit<15>();
        }

        constexpr idt_entry_options &disable_interrupts(bool disable = true) noexcept
        {
            if (disable) {
                value.unset_bit<8>();
            } else {
                value.set_bit<8>();
            }
            return *this;
        }

        constexpr idt_entry_options &set_privilege_level(std::uint16_t lvl) noexcept
        {
            (void)lvl;
            return *this;
        }

        constexpr idt_entry_options &set_stack_index(std::uint16_t index) noexcept
        {
            (void)index;
            return *this;
        }

        utils::bit_field<std::uint16_t> value;
    };

    struct [[gnu::packed]] idt_entry
    {
        constexpr idt_entry() noexcept : pointer_low_bits(0), gdt_selector(0), entry_options(),
                                         pointer_middle_bits(0), pointer_high_bits(0), reserved(0)
        {
        }

        std::uint16_t pointer_low_bits;
        std::uint16_t gdt_selector;
        idt_entry_options entry_options;
        std::uint16_t pointer_middle_bits;
        std::uint32_t pointer_high_bits;
        std::uint32_t reserved;
    };
}

namespace foros
{
    using division_by_zero = std::integral_constant<std::size_t, 0>;
    using breakpoint = std::integral_constant<std::size_t, 3>;
    using invalid_opcode = std::integral_constant<std::size_t, 6>;
    using double_fault = std::integral_constant<std::size_t, 8>;
    using page_fault = std::integral_constant<std::size_t, 14>;

    using division_by_zero_handler_t = st::type<void (*)(const exception_stack_frame *), division_by_zero>;
    using breakpoint_handler_t = st::type<void (*)(const exception_stack_frame *), breakpoint>;
    using invalid_opcode_handler_t = st::type<void (*)(const exception_stack_frame *), invalid_opcode>;
    using double_fault_handler_t = st::type<void (*)(const exception_stack_frame *, uint64_t), double_fault>;
    using page_fault_handler_t = st::type<void (*)(const exception_stack_frame *, uint64_t), page_fault>;

    struct idt : public utils::singleton<idt>
    {
        static constexpr const std::size_t nb_entries = 16;

        idt_entry entries[nb_entries];

        template <typename ExceptionType, typename HandlerType>
        constexpr void set_handler(HandlerType handler) noexcept
        {
            using handler_exception_type = typename HandlerType::tag_type;
            static_assert(std::is_same_v<handler_exception_type, ExceptionType>, "Exception types mismatch");
            static_assert(std::is_same_v<decltype(handler_exception_type::value), decltype(nb_entries)>,
                          "Invalid exception type");
            static_assert(handler_exception_type::value < nb_entries, "Invalid exception type");

            idt_entry &entry = (*this)[handler_exception_type::value];
            auto handler_ptr = reinterpret_cast<uintptr_t>(handler.value());

            entry.gdt_selector = arch::registers::cs();
            entry.pointer_low_bits = static_cast<uint16_t>(handler_ptr);
            entry.pointer_middle_bits = static_cast<uint16_t>(handler_ptr >> 16);
            entry.pointer_high_bits = static_cast<uint16_t>(handler_ptr >> 32);
            entry.entry_options
                .set_present()
                .disable_interrupts();
        }

        void set_default_handler(void (*handler)(const exception_stack_frame *)) noexcept
        {
            std::size_t entry_idx = 0;
            auto handler_ptr = reinterpret_cast<uintptr_t>(handler);

            for (auto &entry : entries) {
                if (!entry.entry_options.is_present()) {

                    entry.gdt_selector = arch::registers::cs();
                    entry.pointer_low_bits = static_cast<uint16_t>(handler_ptr);
                    entry.pointer_middle_bits = static_cast<uint16_t>(handler_ptr >> 16);
                    entry.pointer_high_bits = static_cast<uint16_t>(handler_ptr >> 32);
                    entry.entry_options
                        .set_present()
                        .disable_interrupts();
                }
                ++entry_idx;
            }
        }

        constexpr idt_entry &operator[](std::size_t idx) noexcept
        {
            kassert(idx < nb_entries, "idt::operator[]: out of bounds access");
            return entries[idx];
        }

        constexpr const idt_entry &operator[](std::size_t idx) const noexcept
        {
            kassert(idx < nb_entries, "idt::operator[]: out of bounds access");
            return entries[idx];
        }

        void load() noexcept
        {
            arch::instructions::descriptor_table desc;
            desc.limit = sizeof(idt) - 1; //limit is actually the last allowed index
            desc.base_ptr = reinterpret_cast<uintptr_t>(this);
            arch::instructions::lidt(desc);
        }
    };
}

#endif /* !FOROS_IDT_HPP */
