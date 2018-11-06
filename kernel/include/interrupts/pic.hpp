/*
** Created by doom on 05/11/18.
*/

#ifndef FOROS_INTERRUPTS_PIC_HPP
#define FOROS_INTERRUPTS_PIC_HPP

#include <cstddef>
#include <utils/singleton.hpp>
#include <core/cpu_port.hpp>

namespace foros
{
    class pic
    {
    public:
        explicit constexpr pic(uint8_t command_port_no, uint8_t data_port_no, uint8_t first_interrupt_number) noexcept :
            _command_port(command_port_no), _data_port(data_port_no), _first_interrupt_number(first_interrupt_number)
        {
        }

        const cpu_port<uint8_t> &command_port() const noexcept
        {
            return _command_port;
        }

        const cpu_port<uint8_t> &data_port() const
        {
            return _data_port;
        }

        constexpr bool handles_interrupt(uint8_t interrupt_number) const noexcept
        {
            return _first_interrupt_number <= interrupt_number && interrupt_number < _first_interrupt_number + 8;
        }

        void send_end_of_interrupt() const noexcept
        {
            constexpr uint8_t end_of_interrupt_cmd = 0x20;

            _command_port.write_value(end_of_interrupt_cmd);
        }

    private:
        cpu_port<uint8_t> _command_port;
        cpu_port<uint8_t> _data_port;
        uint8_t _first_interrupt_number;
    };

    namespace details
    {
        /**
         * According to OSDev Wiki, the Linux kernel uses the following trick to ensure
         * that we wait long enough for an I/O operation to complete
         *
         * See https://wiki.osdev.org/Inline_Assembly/Examples#IO_WAIT
         */
        inline void wait_for_cpu_io_completion() noexcept
        {
            constexpr cpu_port<uint8_t> port(0x80);

            port.write_value(0);
        }
    }

    /**
     * This is a coupled PIC, it actually chains a master (here PIC 1) and a slave (PIC 2)
     *
     * Most values here are hardcoded because they are the appropriate ones to talk to the
     * 8259 PIC.
     *
     * See https://wiki.osdev.org/PIC#How_does_the_8259_PIC_chip_work.3F
     */
    class pic_8259 : public utils::singleton<pic_8259>
    {
    private:
        static constexpr uint16_t pic1_command_port_number = 0x20;
        static constexpr uint16_t pic1_data_port_number = pic1_command_port_number + 1;

        static constexpr uint16_t pic2_command_port_number = 0xA0;
        static constexpr uint16_t pic2_data_port_number = pic2_command_port_number + 1;

        /** Interrupt numbers on which we want to map the PICs interrupts (will be used as index into the IDT) */
        static constexpr uint8_t pic1_first_interrupt_number = 0x20;
        static constexpr uint8_t pic2_first_interrupt_number = 0x28;

    public:
        explicit constexpr pic_8259() noexcept :
            _pic1(pic1_command_port_number, pic1_data_port_number, pic1_first_interrupt_number),
            _pic2(pic2_command_port_number, pic2_data_port_number, pic2_first_interrupt_number)
        {
        }

        /**
         * By default, the IDT offsets of the interrupts handled by the 8259 PIC are overlapping
         * with some CPU exceptions, which means the same handlers would be called for both.
         * Thus we have to "remap" the PIC so it can have its own handlers.
         *
         * FIXME: extremely unsafe code
         */
        void remap() const noexcept
        {
            /**
             * ICW stands for Initialization Command Word, and the following number (1, 2, 3 or 4)
             * is the position of this word in the initialization sequence (starting from 1)
             */
            constexpr uint8_t icw1_init = 0x10;
            constexpr uint8_t icw1_icw4 = 0x01;
            constexpr uint8_t icw4_8086 = 0x01;

            /** Back up the current PIC masks */
            auto mask1 = _pic1.data_port().read_value();
            auto mask2 = _pic2.data_port().read_value();

            /** Tell the PICs we are starting an initialization process */
            _pic1.command_port().write_value(icw1_init | icw1_icw4);
            details::wait_for_cpu_io_completion();
            _pic2.command_port().write_value(icw1_init | icw1_icw4);
            details::wait_for_cpu_io_completion();

            /** Send the initialization settings */
            _pic1.data_port().write_value(0x20);
            details::wait_for_cpu_io_completion();
            _pic2.data_port().write_value(0x28);
            details::wait_for_cpu_io_completion();

            /** Tell the master PIC (PIC 1) that there is a slave at IRQ 2 */
            _pic1.data_port().write_value(0x4);
            details::wait_for_cpu_io_completion();

            /** Tell the slave PIC (PIC 2) that it has cascade identity 2 */
            _pic2.data_port().write_value(0x2);
            details::wait_for_cpu_io_completion();

            /** Tell the PICs to run in 8086 mode */
            _pic1.data_port().write_value(icw4_8086);
            details::wait_for_cpu_io_completion();
            _pic2.data_port().write_value(icw4_8086);
            details::wait_for_cpu_io_completion();

            /** Restore the previously backed-up masks */
            _pic1.data_port().write_value(mask1);
            _pic2.data_port().write_value(mask2);
        }

        void send_end_of_interrupt(uint8_t interrupt_number) noexcept
        {
            if (_pic1.handles_interrupt(interrupt_number)) {
                _pic1.send_end_of_interrupt();
            } else if (_pic2.handles_interrupt(interrupt_number)) {
                _pic2.send_end_of_interrupt();
            }
        }

    private:
        pic _pic1;
        pic _pic2;
    };
}

#endif /* !FOROS_INTERRUPTS_PIC_HPP */
