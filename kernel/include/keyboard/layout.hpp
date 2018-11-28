/*
** Created by doom on 20/11/18.
*/

#ifndef FOROS_KEYBOARD_LAYOUT_HPP
#define FOROS_KEYBOARD_LAYOUT_HPP

#include <utils/map.hpp>
#include <keyboard/keys.hpp>

namespace foros::kbd
{
    /** Type representing a keyboard layout */
    using layout = utils::map<key_code, char, 27>;

    inline constexpr auto azerty_layout = layout{
        {key_code::q, 'a'},
        {key_code::w, 'z'},
        {key_code::e, 'e'},
        {key_code::r, 'r'},
        {key_code::t, 't'},
        {key_code::y, 'y'},
        {key_code::u, 'u'},
        {key_code::i, 'i'},
        {key_code::o, 'o'},
        {key_code::p, 'p'},
        {key_code::a, 'q'},
        {key_code::s, 's'},
        {key_code::d, 'd'},
        {key_code::f, 'f'},
        {key_code::g, 'g'},
        {key_code::h, 'h'},
        {key_code::j, 'j'},
        {key_code::k, 'k'},
        {key_code::l, 'l'},
        {key_code::semi_colon, 'm'},
        {key_code::z, 'w'},
        {key_code::x, 'x'},
        {key_code::c, 'c'},
        {key_code::v, 'v'},
        {key_code::b, 'b'},
        {key_code::n, 'n'},
        {key_code::spacebar, ' '},
    };
}

#endif /* !FOROS_KEYBOARD_LAYOUT_HPP */
