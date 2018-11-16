/*
** Created by doom on 13/10/18.
*/

#ifndef FOROS_MULTIBOOT2_HPP
#define FOROS_MULTIBOOT2_HPP

#include <cstdint>
#include <cstddef>
#include <iterator>
#include <utils/pointer_like.hpp>
#include <core/panic.hpp>
#include <multiboot2/details/raw_types.hpp>
#include <multiboot2/command_line_tag.hpp>
#include <multiboot2/boot_loader_name_tag.hpp>
#include <multiboot2/elf_sections_tag.hpp>
#include <multiboot2/memory_map_tag.hpp>

namespace multiboot2
{
    class tag_iterator
    {
    public:
        constexpr tag_iterator() noexcept : _raw(nullptr)
        {
        }

        explicit tag_iterator(const details::tag_raw *raw) noexcept : _raw(raw)
        {
        }

        const details::tag_raw &operator*() const noexcept
        {
            return *_raw;
        }

        const details::tag_raw *operator->() const noexcept
        {
            return _raw;
        }

        tag_iterator &operator++() noexcept
        {
            _raw = details::next_tag(_raw);
            if (_raw->type == MULTIBOOT_TAG_TYPE_END)
                _raw = nullptr;
            return *this;
        }

        const tag_iterator operator++(int) noexcept
        {
            auto tmp = *this;

            ++*this;
            return tmp;
        }

        constexpr bool operator==(const tag_iterator &other) const noexcept
        {
            return _raw == other._raw;
        }

        constexpr bool operator!=(const tag_iterator &other) const noexcept
        {
            return !(*this == other);
        }

    private:
        const details::tag_raw *_raw;
    };

    template <typename ValueType>
    class typed_tag_iterator
    {
    public:
        using raw_type = typename ValueType::raw_type;
        using value_type = ValueType;
        using pointer = utils::pointer_like<value_type>;
        using reference = value_type;
        using iterator_category = std::input_iterator_tag;

        typed_tag_iterator(const tag_iterator &it) noexcept : _it(it)
        {
            tag_iterator end_it;

            while (_it != end_it && _it->type != ValueType::tag_id) {
                ++_it;
            }
        }

        reference operator*() const noexcept
        {
            return value_type(reinterpret_cast<const raw_type *>(&*_it));
        }

        const pointer operator->() const noexcept
        {
            return pointer{**this};
        }

        typed_tag_iterator operator++() noexcept
        {
            tag_iterator end_it;

            do {
                ++_it;
            } while (_it != end_it && _it->type != ValueType::tag_id);
            return *this;
        }

        const typed_tag_iterator operator++(int) noexcept
        {
            auto tmp = *this;

            ++*this;
            return tmp;
        }

        constexpr bool operator==(const typed_tag_iterator &other) const noexcept
        {
            return _it == other._it;
        }

        constexpr bool operator!=(const typed_tag_iterator &other) const noexcept
        {
            return _it != other._it;
        }

    private:
        tag_iterator _it;
    };

    class boot_information
    {
    public:
        boot_information(const std::byte *start_address) noexcept :
            _raw(reinterpret_cast<const details::boot_information_raw *>(start_address))
        {
        }

        size_t total_size() const noexcept
        {
            return _raw->total_size;
        }

        const std::byte *start_address() const noexcept
        {
            return reinterpret_cast<const std::byte *>(_raw);
        }

        const std::byte *end_address() const noexcept
        {
            return start_address() + total_size();
        }

        tag_iterator tags_begin() const noexcept
        {
            return tag_iterator(reinterpret_cast<const details::tag_raw *>(_raw + 1));
        }

        tag_iterator tags_end() const noexcept
        {
            return tag_iterator();
        }

        template <typename T>
        auto tags_begin() const noexcept
        {
            return typed_tag_iterator<T>(tags_begin());
        }

        template <typename T>
        auto tags_end() const noexcept
        {
            return typed_tag_iterator<T>(tags_end());
        }

        template <typename Tag>
        auto tag() const noexcept
        {
            auto tag_it = tags_begin<Tag>();

            kassert(tag_it != tags_end<Tag>(), "boot_information::tag: unable to find the requested tag");
            return *tag_it;
        }

    private:
        const details::boot_information_raw *_raw;
    };
}

#endif /* !FOROS_MULTIBOOT2_HPP */
