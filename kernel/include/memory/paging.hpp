/*
** Created by doom on 04/11/18.
*/

#ifndef FOROS_MEMORY_PAGING_HPP
#define FOROS_MEMORY_PAGING_HPP

#include <climits>
#include <cstdint>
#include <st/st.hpp>
#include <utils/bit_field_trait.hpp>
#include <utils/optional.hpp>
#include <arch/x86_64/instructions.hpp>
#include <memory/definitions.hpp>
#include <memory/physical_frame.hpp>

namespace foros::memory
{
    namespace arch = ::foros::x86_64;

    namespace details
    {
        struct page_entry_flags :
            public st::type_base<uint64_t>,
            public st::traits::bitwise_manipulable<page_entry_flags>
        {
            page_entry_flags(const page_entry_flags &flags) noexcept : st::type_base<uint64_t>::type_base(flags.value())
            {
            }

            using type_base<uint64_t>::type_base;

            static const page_entry_flags present;
            static const page_entry_flags writable;
            static const page_entry_flags user_accessible;
            static const page_entry_flags write_through;
            static const page_entry_flags no_cache;
            static const page_entry_flags accessed;
            static const page_entry_flags dirty;
            static const page_entry_flags huge_page;
            static const page_entry_flags global;
            static const page_entry_flags no_execute;

            constexpr bool has(page_entry_flags flag) const noexcept
            {
                return (value() & flag.value()) != 0;
            }
        };

        inline const page_entry_flags page_entry_flags::present{1 << 0};
        inline const page_entry_flags page_entry_flags::writable{1 << 1};
        inline const page_entry_flags page_entry_flags::user_accessible{1 << 2};
        inline const page_entry_flags page_entry_flags::write_through{1 << 3};
        inline const page_entry_flags page_entry_flags::no_cache{1 << 4};
        inline const page_entry_flags page_entry_flags::accessed{1 << 5};
        inline const page_entry_flags page_entry_flags::dirty{1 << 6};
        inline const page_entry_flags page_entry_flags::huge_page{1 << 7};
        inline const page_entry_flags page_entry_flags::global{1 << 8};
        inline const page_entry_flags page_entry_flags::no_execute{1UL << 63};
    }

    struct page :
        public st::type_base<std::size_t>,
        public st::traits::orderable<page>,
        public st::traits::addable<page, std::size_t>
    {
        using st::type_base<std::size_t>::type_base;

        static page for_address(virtual_address addr) noexcept
        {
            /**
             * On x86_64, addresses only really are 48-bit long. In order to still remain unique on
             * a 64-bit pointer, the bits above the 47th are just replicas of the 47th bit.
             *
             * Because of that, a virtual address is either above (or equal to) 0xffff800000000000
             * (if its 47th bit is set), or below 0x0000800000000000 (if its 47th bit if not set).
             * Addresses that don't match this requirement are invalid.
             */
            kassert(addr.value() >= 0xffff800000000000 || addr.value() < 0x0000800000000000,
                    "page::for_address: invalid virtual address");
            return page(addr.value() / page_size);
        }

        constexpr virtual_address start_address() const noexcept
        {
            return virtual_address(page_size * value());
        }

        /**
         * The 48-bit layout of an x84_64 virtual address is as follows:
         *
         *  |   P4   ||   P3   ||   P2   ||   P1   ||    Page   |
         *  vvvvvvvv vwwwwwww wwxxxxxx xxxyyyyy yyyyzzzz zzzzzzzz
         *
         *  We have 4 pieces of 9 bits each, and one piece of 12 bits
         *  The "address" of a page is thus only the 36 first bits (we don't need the extra 12
         *  of the address since they are the offset into the page)
         */
        constexpr size_t p4_index() const noexcept
        {
            return (value() >> 27) & 0x1ff;
        }

        constexpr size_t p3_index() const noexcept
        {
            return (value() >> 18) & 0x1ff;
        }

        constexpr size_t p2_index() const noexcept
        {
            return (value() >> 9) & 0x1ff;
        }

        constexpr size_t p1_index() const noexcept
        {
            return value() & 0x1ff;
        }
    };

    struct page_table_entry : public st::type_base<uint64_t>
    {
    public:
        using st::type_base<uint64_t>::type_base;

        using flags = details::page_entry_flags;

    private:
        static constexpr const uint64_t addr_only_mask = 0x000ffffffffff000;

    public:
        /**
         * Check whether this page table entry is unused
         *
         * @return              if the entry is unused, true
         *                      if the entry is used, false
         */
        constexpr bool is_unused() const noexcept
        {
            return value() == 0;
        }

        /**
         * Mark this page table entry as unused
         */
        constexpr void set_unused() noexcept
        {
            value() = 0;
        }

        /**
         * Get the flags associated with this page table entry
         *
         * @return              the entry flags
         */
        flags entry_flags() const noexcept
        {
            const flags flags_only_mask = flags::present | flags::writable | flags::user_accessible |
                                          flags::write_through | flags::no_cache | flags::accessed |
                                          flags::dirty | flags::huge_page | flags::global | flags::no_execute;
            return flags(value() & (flags_only_mask.value()));
        }

        /**
         * Get the frame to which this page is mapped
         *
         * @return              on success, an optional containing the frame
         *                      on failure, nullopt
         */
        utils::optional<physical_frame> get_frame() const noexcept
        {
            if (entry_flags().has(flags::present)) {
                const uintptr_t addr = value() & addr_only_mask;

                return {physical_frame::for_address(addr)};
            }
            return std::nullopt;
        }

        /**
         * Set a physical frame as mapped, applying given flags
         *
         * @param f             the frame to set
         * @param fl            the flags to apply
         */
        void set_frame(physical_frame f, flags fl) noexcept
        {
            const flags flags_only_mask = flags::present | flags::writable | flags::user_accessible |
                                          flags::write_through | flags::no_cache | flags::accessed |
                                          flags::dirty | flags::huge_page | flags::global | flags::no_execute;
            kassert((f.start_address() & ~addr_only_mask) == 0,
                    "page_table_entry::set_frame: invalid address for physical_frame");
            kassert((~flags_only_mask & fl).value() == 0, "page_table_entry::set_frame: invalid flags");
            value() = f.value() | fl.value();
        }

        /**
         * Set a physical frame as mapped
         *
         * @param f             the frame to set
         */
        void set_frame(physical_frame f) noexcept
        {
            set_frame(f, entry_flags());
        }
    };

    static_assert(sizeof(page_table_entry) == sizeof(uint64_t));

    template <std::size_t Level>
    struct table_level
    {
        using next_level = table_level<Level - 1>;
    };

    template <>
    struct table_level<1>
    {
    };

    namespace details
    {
        struct page_table_base
        {
        public:
            page_table_base(const page_table_base &) = delete;

            page_table_base &operator=(const page_table_base &) = delete;

            /**
             * Mark all entries of this page table as unused
             */
            void clear() noexcept
            {
                for (auto &entry : entries) {
                    entry.set_unused();
                }
            }

            /**
             * Get the entry at a given index
             *
             * @param index         the index of the entry
             *
             * @return              a const-reference to the entry
             */
            constexpr const page_table_entry &operator[](std::size_t index) const noexcept
            {
                return entries[index];
            }

            /**
             * Get the entry at a given index
             *
             * @param index         the index of the entry
             *
             * @return              a reference to the entry
             */
            constexpr page_table_entry &operator[](std::size_t index) noexcept
            {
                return entries[index];
            }

        protected:
            page_table_entry entries[page_entries_count];
        };
    }

    template <typename LevelT>
    struct page_table : public details::page_table_base
    {
    public:
        using level_type = LevelT;
        using next_level_type = typename level_type::next_level;

        using details::page_table_base::page_table_base;

    private:
        constexpr utils::optional<uintptr_t> _next_table_addr(std::size_t index) const noexcept
        {
            auto entry_flags = (*this)[index].entry_flags();

            /** Huge pages are stored in upper levels of the page tables, but we still should not use them as tables */
            if (entry_flags.has(page_table_entry::flags::present) &&
                !entry_flags.has(page_table_entry::flags::huge_page)) {
                auto table_addr = reinterpret_cast<uintptr_t>(entries);
                auto next_table_addr = (table_addr << 9) | (index << 12);
                return {next_table_addr};
            }
            return std::nullopt;
        }

    public:
        /**
         * Get the next table at a given index
         *
         * @param index         the index at which the table entry is stored in the current table
         *
         * @return              on success, an optional containing a reference to the table
         *                      on failure, nullopt
         */
        constexpr utils::optional<page_table<next_level_type> &> next_table(std::size_t index) noexcept
        {
            return _next_table_addr(index).map([](auto &&addr) -> page_table<next_level_type> & {
                return *reinterpret_cast<page_table<next_level_type> *>(addr);
            });
        }

        /**
         * Get the next table at a given index
         *
         * @param index         the index at which the table entry is stored in the current table
         *
         * @return              on success, an optional containing a reference to the table
         *                      on failure, nullopt
         */
        constexpr utils::optional<const page_table<next_level_type> &> next_table(std::size_t index) const noexcept
        {
            return _next_table_addr(index).map([](auto &&addr) -> const page_table<next_level_type> & {
                return *reinterpret_cast<page_table<next_level_type> *>(addr);
            });
        }

        /**
         * Allocate the next table and add in a given entry slot
         *
         * @param index         the index at which to create the entry
         * @param al            the physical frame allocator to use
         *
         * @return              a reference to the newly created table
         */
        page_table<next_level_type> &allocate_next_table(size_t index, physical_frame_allocator &al) noexcept
        {
            auto next_opt = next_table(index);

            if (next_opt) {
                return *next_opt;
            }
            kassert(!entries[index].entry_flags().has(page_table_entry::flags::huge_page),
                    "page_table::allocate_next_table: huge pages are not supported");
            auto frame = al.allocate_frame();
            kassert(frame, "page_table::allocate_next_table: unable to allocate a physical_frame");
            entries[index].set_frame(*frame, page_table_entry::flags::present | page_table_entry::flags::writable);
            auto next = next_table(index);
            next->clear();
            return *next;
        }
    };

    template <>
    struct page_table<table_level<1>> : public details::page_table_base
    {
        using details::page_table_base::page_table_base;
    };

    using p4_table = page_table<table_level<4>>;
    using p3_table = page_table<table_level<3>>;
    using p2_table = page_table<table_level<2>>;
    using p1_table = page_table<table_level<1>>;

    class mapper
    {
    private:
        static p4_table &root_p4_table() noexcept
        {
            constexpr uintptr_t p4_table_addr = 0xfffffffffffff000;
            auto *p4 = reinterpret_cast<p4_table *>(p4_table_addr);

            return *p4;
        }

    public:
        /**
         * Obtain the physical frame to which a page is mapped
         *
         * @param p             the page
         *
         * @return              on success, an optional containing the physical frame
         *                      on failure, nullopt
         */
        static utils::optional<physical_frame> get_frame_for_page(page p) noexcept
        {
            auto p3opt = root_p4_table().next_table(p.p4_index());

            auto r = p3opt.and_then([&p](auto &&p3) {
                return p3.next_table(p.p3_index());
            }).and_then([&p](auto &&p2) {
                return p2.next_table(p.p2_index());
            }).and_then([&p](auto &&p1) {
                return p1[p.p1_index()].get_frame();
            });

            //TODO: support huge pages

            return r;
        }

        /**
         * Obtain the physical frame to which a virtual address is mapped
         *
         * @param addr
         *
         * @return              on success, an optional containing the physical frame
         *                      on failure, nullopt
         */
        static utils::optional<physical_frame> get_frame_for_address(virtual_address addr) noexcept
        {
            return get_frame_for_page(page::for_address(addr));
        }

        /**
         * Map a given page to a given frame
         *
         * @param frame         the frame to map to
         * @param p             the page to map
         * @param entry_flags   the flags to apply to the page
         * @param al            the physical allocator used to allocate physical frames
         */
        static void map_page_to_frame(physical_frame frame,
                                      page p, page_table_entry::flags entry_flags,
                                      physical_frame_allocator &al) noexcept
        {
            auto &p3 = root_p4_table().allocate_next_table(p.p4_index(), al);
            auto &p2 = p3.allocate_next_table(p.p3_index(), al);
            auto &p1 = p2.allocate_next_table(p.p2_index(), al);

            kassert(p1[p.p1_index()].is_unused(), "map_page_to_frame: page already in use");
            p1[p.p1_index()].set_frame(frame, entry_flags | page_table_entry::flags::present);
        }

        /**
         * Map a given page to the next free frame
         *
         * @param p             the page to map
         * @param entry_flags   the flags to apply to the page
         * @param al            the physical allocator used to allocate physical frames
         */
        static void map_page(page p, page_table_entry::flags entry_flags,
                             physical_frame_allocator &al) noexcept
        {
            auto frame_opt = al.allocate_frame();

            map_page_to_frame(*frame_opt, p, entry_flags, al);
        }

        /**
         * Map a physical frame to the equivalent virtual address
         *
         * @param frame         the frame to map to
         * @param entry_flags   the flags to apply to the page
         * @param al            the physical allocator used to allocate physical frames
         */
        static void identity_map_frame(physical_frame frame,
                                       page_table_entry::flags entry_flags,
                                       physical_frame_allocator &al) noexcept
        {
            auto page = page::for_address(virtual_address(frame.start_address()));

            map_page_to_frame(frame, page, entry_flags, al);
        }

        /**
         * Unmap a previously mapped memory page.
         * After unmapping, any access to the memory in that page will trigger a page fault
         *
         * @param p             the page to unmap
         * @param al            the physical allocator used to allocate physical frames
         */
        static void unmap(page p, physical_frame_allocator &al)
        {
            auto p3opt = root_p4_table().next_table(p.p4_index());

            auto p1opt = p3opt.and_then([&p](auto &&p3) {
                return p3.next_table(p.p3_index());
            }).and_then([&p](auto &&p2) {
                return p2.next_table(p.p2_index());
            });
            kassert(p1opt, "memory::manager::unmap: attempted to unmap an already unmapped frame");

            auto frame = *(*p1opt)[p.p1_index()].get_frame();
            (*p1opt)[p.p1_index()].set_unused();
            al.deallocate_frame(frame);
            arch::instructions::invlpg(p.start_address().value());
        }
    };
}

#endif /* !FOROS_MEMORY_PAGING_HPP */
