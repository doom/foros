/*
** Created by doom on 03/11/18.
*/

#ifndef FOROS_MULTIBOOT2_COMMAND_LINE_HPP
#define FOROS_MULTIBOOT2_COMMAND_LINE_HPP

#include <string_view>
#include <multiboot2/details/raw_types.hpp>

namespace multiboot2
{
    class command_line_tag
    {
    public:
        static constexpr const auto tag_id = details::command_line_tag_id;
        using raw_type = details::tag_string_raw;

        explicit constexpr command_line_tag(const raw_type *str) noexcept : _str(str)
        {
        }

        constexpr std::string_view arguments() const noexcept
        {
            return {_str->string};
        }

    private:
        const raw_type *_str;
    };
}

#endif /* !FOROS_MULTIBOOT2_COMMAND_LINE_HPP */
