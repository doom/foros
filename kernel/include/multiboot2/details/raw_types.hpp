/*
** Created by doom on 03/11/18.
*/

#ifndef FOROS_MULTIBOOT2_RAW_TYPES_HPP
#define FOROS_MULTIBOOT2_RAW_TYPES_HPP

#include <cstdint>
#include <cstddef>
#include <meta/utils/enum_bitwise_ops.hpp>
#include <multiboot2/details/multiboot2.h>

namespace multiboot2::details
{
    struct boot_information_raw
    {
        uint32_t total_size;
        uint32_t reserved;
    };

    static constexpr auto command_line_tag_id = MULTIBOOT_TAG_TYPE_CMDLINE;
    static constexpr auto boot_loader_name_tag_id = MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME;
    static constexpr auto memory_map_tag_id = MULTIBOOT_TAG_TYPE_MMAP;
    static constexpr auto elf_sections_tag_id = MULTIBOOT_TAG_TYPE_ELF_SECTIONS;

    using tag_raw = multiboot_tag;

    using tag_string_raw = multiboot_tag_string;

    using tag_memory_map_raw = multiboot_tag_mmap;

    using memory_map_entry_raw = multiboot_mmap_entry;


    enum class memory_area_type_raw
    {
        memory_available = MULTIBOOT_MEMORY_AVAILABLE,
        reserved = MULTIBOOT_MEMORY_RESERVED,
        acpi_reclaimable = MULTIBOOT_MEMORY_ACPI_RECLAIMABLE,
        nvs = MULTIBOOT_MEMORY_NVS,
        bad_ram = MULTIBOOT_MEMORY_BADRAM,
    };

    using tag_elf_sections_raw = multiboot_tag_elf_sections;

    struct elf_section_header_raw
    {
        uint32_t sh_name;
        uint32_t sh_type;
        uint64_t sh_flags;
        uint64_t sh_addr;
        uint64_t sh_offset;
        uint64_t sh_size;
        uint32_t sh_link;
        uint32_t sh_info;
        uint64_t sh_addralign;
        uint64_t sh_entsize;
    };

    static inline constexpr uintptr_t align_up(uintptr_t ptr, size_t boundary) noexcept
    {
        return (ptr + (boundary - 1)) & ~(boundary - 1);
    }

    static inline const tag_raw *next_tag(const tag_raw *tag) noexcept
    {
        return reinterpret_cast<const tag_raw *>(align_up((uintptr_t)tag + tag->size, MULTIBOOT_TAG_ALIGN));
    }

    static inline const memory_map_entry_raw *memory_map_end_area(const tag_memory_map_raw *tag) noexcept
    {
        auto ptr = reinterpret_cast<const std::byte *>(tag) + (tag->size - tag->entry_size);

        return reinterpret_cast<const multiboot_memory_map_t *>(ptr);
    }

    static inline const memory_map_entry_raw *memory_map_next_entry(const memory_map_entry_raw *entry,
                                                      size_t entry_size) noexcept
    {
        auto ptr = reinterpret_cast<const std::byte *>(entry) + entry_size;

        return reinterpret_cast<const multiboot_memory_map_t *>(ptr);
    }

    static inline const elf_section_header_raw *elf_sections_next(const elf_section_header_raw *entry, size_t entry_size)
    {
        auto ptr = reinterpret_cast<const std::byte *>(entry) + entry_size;

        return reinterpret_cast<const elf_section_header_raw *>(ptr);
    }
}

ENABLE_BITWISE_OPS_FOR(multiboot2::details::memory_area_type_raw);

#endif /* !FOROS_MULTIBOOT2_RAW_TYPES_HPP */
