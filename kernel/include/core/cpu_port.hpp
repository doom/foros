/*
** Created by doom on 05/11/18.
*/

#ifndef FOROS_CORE_CPU_PORT_HPP
#define FOROS_CORE_CPU_PORT_HPP

#include <cstdint>
#include <arch/x86_64/instructions.hpp>
#include <st/st.hpp>

namespace foros
{
    namespace arch = foros::x86_64;

    template <typename ValueT>
    class cpu_port
    {
    public:
        static_assert(std::is_same_v<ValueT, uint8_t>
                      || std::is_same_v<ValueT, uint16_t>
                      || std::is_same_v<ValueT, uint32_t>,
                      "CPU ports only support values from types uint8_t, uint16_t or uint32_t");

        explicit constexpr cpu_port(std::uint16_t port_no) noexcept : _port_no(port_no)
        {
        }

        ValueT read_value() const noexcept
        {
            if constexpr (std::is_same_v<ValueT, uint8_t>) {
                return arch::instructions::inb(_port_no);
            } else if constexpr (std::is_same_v<ValueT, uint16_t>) {
                return arch::instructions::inw(_port_no);
            } else {
                return arch::instructions::inl(_port_no);
            }
        }

        void write_value(ValueT value) const noexcept
        {
            if constexpr (std::is_same_v<ValueT, uint8_t>) {
                arch::instructions::outb(value, _port_no);
            } else if constexpr (std::is_same_v<ValueT, uint16_t>) {
                arch::instructions::outw(value, _port_no);
            } else {
                arch::instructions::outl(value, _port_no);
            }
        }

    private:
        uint16_t _port_no;
    };
}

#endif /* !FOROS_CORE_CPU_PORT_HPP */
