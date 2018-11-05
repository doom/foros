/*
** Created by doom on 03/11/18.
*/

#ifndef FOROS_MULTIBOOT2_ELF_SECTIONS_TAG_HPP
#define FOROS_MULTIBOOT2_ELF_SECTIONS_TAG_HPP

#include <iterator>
#include <string_view>
#include <utils/pointer_like.hpp>
#include <multiboot2/details/raw_types.hpp>

namespace multiboot2
{
    class elf_section
    {
    public:
        explicit constexpr elf_section(const details::elf_section_header_raw *raw,
                                       const details::elf_section_header_raw *str_sect) noexcept :
            _raw(raw), _str_sect(str_sect)
        {
        }

        std::string_view name() const noexcept
        {
            return {reinterpret_cast<const char *>(_str_sect->sh_addr + _raw->sh_name)};
        }

        constexpr std::size_t size() const noexcept
        {
            return _raw->sh_size;
        }

        const std::byte *start_address() const noexcept
        {
            return reinterpret_cast<const std::byte *>(_raw->sh_addr);
        }

        const std::byte *end_address() const noexcept
        {
            return reinterpret_cast<const std::byte *>(_raw->sh_addr) + size();
        }

    private:
        const details::elf_section_header_raw *_raw;
        const details::elf_section_header_raw *_str_sect;
    };

    class elf_sections_iterator
    {
    public:
        using value_type = const elf_section;
        using reference = value_type;
        using pointer = utils::pointer_like<elf_section>;
        using iterator_category = std::input_iterator_tag;

        constexpr elf_sections_iterator() noexcept : _sect(nullptr), _str_sect(nullptr), _nb_sections(0), _entry_size(0)
        {
        }

        explicit constexpr elf_sections_iterator(const details::elf_section_header_raw *sect,
                                                 const details::elf_section_header_raw *str_sect,
                                                 std::size_t nb_sections, std::size_t entry_size) noexcept :
            _sect(sect), _str_sect(str_sect), _nb_sections(nb_sections), _entry_size(entry_size)
        {
        }

        reference operator*() const noexcept
        {
            return elf_section(_sect, _str_sect);
        }

        pointer operator->() const noexcept
        {
            return pointer(elf_section(_sect, _str_sect));
        }

        elf_sections_iterator &operator++() noexcept
        {
            _sect = details::elf_sections_next(_sect, _entry_size);

            --_nb_sections;
            if (_nb_sections == 0) {
                _sect = nullptr;
            }
            return *this;
        }

        const elf_sections_iterator operator++(int) noexcept
        {
            auto tmp = *this;

            ++*this;
            return tmp;
        }

        constexpr bool operator==(const elf_sections_iterator &other) const noexcept
        {
            return _sect == other._sect;
        }

        constexpr bool operator!=(const elf_sections_iterator &other) const noexcept
        {
            return !(*this == other);
        }

    private:
        const details::elf_section_header_raw *_sect;
        const details::elf_section_header_raw *_str_sect;
        std::size_t _nb_sections;
        std::size_t _entry_size;
    };

    class elf_sections_tag
    {
    public:
        static constexpr const auto tag_id = details::elf_sections_tag_id;
        using raw_type = details::tag_elf_sections_raw;

        explicit constexpr elf_sections_tag(const raw_type *sections) noexcept : _sections(sections)
        {
        }

        auto sections_begin() const noexcept
        {
            auto string_section = _sections->sections + (_sections->shndx * _sections->entsize);
            return elf_sections_iterator(
                reinterpret_cast<const details::elf_section_header_raw *>(_sections->sections + _sections->entsize),
                reinterpret_cast<const details::elf_section_header_raw *>(string_section),
                _sections->num - 1, _sections->entsize);
        }

        auto sections_end() const noexcept
        {
            return elf_sections_iterator();
        }

    private:
        const raw_type *_sections;
    };
}

#endif /* !FOROS_MULTIBOOT2_ELF_SECTIONS_TAG_HPP */
