/*
** Created by doom on 20/11/18.
*/

#ifndef FOROS_KEY_CODES_HPP
#define FOROS_KEY_CODES_HPP

namespace foros::kbd
{
    /**
     * Enumeration describing a physical key
     *
     * The names are from a QWERTY physical keyboard, thus they do not correspond to actual letters
     * (and might now even match keys on your physical keyboard)
     */
    enum key_code
    {
        escape, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, insert, _delete,

        back_quote, key1, key2, key3, key4, key5, key6, key7, key8, key9, key0, minus, equals, backspace,

        tab, q, w, e, r, t, y, u, i, o, p, left_square_bracket, right_square_bracket, backslash, enter, page_up,

        caps_lock, a, s, d, f, g, h, j, k, l, semi_colon, single_quote, page_down,

        left_shift, z, x, c, v, b, n, m, comma, dot, slash, right_shift, end,

        left_control, left_alt, spacebar, right_alt, right_control, left_arrow, up_arrow, right_arrow, down_arrow,

        home,
    };

    enum key_state
    {
        up,
        down,
    };

    struct key_event
    {
        key_code code;
        key_state state;
    };

    struct key_modifiers
    {
    };
};

#endif /* !FOROS_KEY_CODES_HPP */
