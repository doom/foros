/*
** Created by doom on 03/11/18.
*/

#ifndef FOROS_MULTIBOOT2_MEMORY_MAP_TAG_HPP
#define FOROS_MULTIBOOT2_MEMORY_MAP_TAG_HPP

#include <iterator>
#include <utils/pointer_like.hpp>
#include <multiboot2/details/raw_types.hpp>

namespace multiboot2
{
    class memory_area
    {
    public:
        constexpr explicit memory_area(const details::memory_map_entry_raw *_entry) noexcept : _entry(_entry)
        {
        }

        memory_area(const memory_area &) noexcept = default;

        const std::byte *start_address() const noexcept
        {
            return reinterpret_cast<const std::byte *>(_entry->addr);
        }

        const std::byte *end_address() const noexcept
        {
            return reinterpret_cast<const std::byte *>(_entry->addr) + _entry->len;
        }

        std::size_t size() const noexcept
        {
            return _entry->len;
        }

        using memory_type = details::memory_area_type_raw;

        memory_type type() const noexcept
        {
            return static_cast<memory_type>(_entry->type);
        }

    private:
        const details::memory_map_entry_raw *_entry;
    };

    class memory_area_iterator
    {
    public:
        using value_type = const memory_area;
        using reference = value_type;
        using pointer = utils::pointer_like<memory_area>;
        using iterator_category = std::input_iterator_tag;

        constexpr memory_area_iterator() noexcept : _entry(nullptr), _end_area(nullptr), _entry_size(0)
        {
        }

        explicit constexpr memory_area_iterator(const details::memory_map_entry_raw *entry,
                                                const details::memory_map_entry_raw *end_area,
                                                std::size_t entry_size) noexcept :
            _entry(entry), _end_area(end_area), _entry_size(entry_size)
        {
        }

        reference operator*() const noexcept
        {
            return memory_area(_entry);
        }

        pointer operator->() const noexcept
        {
            return pointer(memory_area(_entry));
        }

        memory_area_iterator &operator++() noexcept
        {
            _entry = details::memory_map_next_entry(_entry, _entry_size);

            if (_entry == _end_area) {
                _entry = nullptr;
            }
            return *this;
        }

        const memory_area_iterator operator++(int) noexcept
        {
            auto tmp = *this;

            ++*this;
            return tmp;
        }

        constexpr bool operator==(const memory_area_iterator &other) const noexcept
        {
            return _entry == other._entry;
        }

        constexpr bool operator!=(const memory_area_iterator &other) const noexcept
        {
            return !(*this == other);
        }

    private:
        const details::memory_map_entry_raw *_entry;
        const details::memory_map_entry_raw *_end_area;
        std::size_t _entry_size;
    };

    class memory_map_tag
    {
    public:
        static constexpr const auto tag_id = details::memory_map_tag_id;
        using raw_type = details::tag_memory_map_raw;

        explicit constexpr memory_map_tag(const raw_type *mmap) noexcept : _mmap(mmap)
        {
        }

        auto memory_areas_begin() const noexcept
        {
            return memory_area_iterator(_mmap->entries, details::memory_map_end_area(_mmap), _mmap->entry_size);
        }

        auto memory_areas_end() const noexcept
        {
            return memory_area_iterator();
        }

    private:
        const raw_type *_mmap;
    };
}

#endif /* !FOROS_MULTIBOOT2_MEMORY_MAP_TAG_HPP */
