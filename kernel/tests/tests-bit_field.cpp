/*
** Created by doom on 28/10/18.
*/

#include "tests_config.hpp"
#include <cstdint>
#include <utils/bit_field.hpp>

ut_test(set_bit)
{
    utils::bit_field<uint8_t> bf1;

    ut_assert_eq(bf1.value, 0);
    bf1.set_bit<0>();
    ut_assert_eq(bf1.value, 1);
    bf1.set_bit<1>();
    ut_assert_eq(bf1.value, 3);
    bf1.set_bit<2>();
    ut_assert_eq(bf1.value, 7);
    bf1.set_bit<3>();
    ut_assert_eq(bf1.value, 15);
    bf1.set_bit<4>();
    ut_assert_eq(bf1.value, 31);
    bf1.set_bit<5>();
    ut_assert_eq(bf1.value, 63);
    bf1.set_bit<6>();
    ut_assert_eq(bf1.value, 127);
    bf1.set_bit<7>();
    ut_assert_eq(bf1.value, 255);

    ut_assert_eq(bf1.value, 255);
    bf1.unset_bit<0>();
    ut_assert_eq(bf1.value, 254);
    bf1.unset_bit<1>();
    ut_assert_eq(bf1.value, 252);
    bf1.unset_bit<2>();
    ut_assert_eq(bf1.value, 248);
    bf1.unset_bit<3>();
    ut_assert_eq(bf1.value, 240);
    bf1.unset_bit<4>();
    ut_assert_eq(bf1.value, 224);
    bf1.unset_bit<5>();
    ut_assert_eq(bf1.value, 192);
    bf1.unset_bit<6>();
    ut_assert_eq(bf1.value, 128);
    bf1.unset_bit<7>();
    ut_assert_eq(bf1.value, 0);
}

ut_test(set_bit_range)
{
    utils::bit_field<uint32_t> bf1;
    bf1.set_bit_range<0, 32>();
    ut_assert_eq(bf1.value, 4294967295);
    auto bf2 = bf1;
    bf1.unset_bit_range<0, 16>();
    ut_assert_eq(bf1.value, 4294901760);
    bf2.unset_bit_range<16, 32>();
    ut_assert_eq(bf2.value, 65535);

    utils::bit_field<uint16_t> bf3;
    bf3.set_bit_range<0, 3>();
    ut_assert_eq(bf3.value, 7);
}

ut_test(get_bit)
{
    constexpr utils::bit_field<uint16_t> bf{25946}; //01100101 01011010

    static_assert(!bf.get_bit<15>());
    static_assert(bf.get_bit<14>());
    static_assert(bf.get_bit<13>());
    static_assert(!bf.get_bit<12>());
    static_assert(!bf.get_bit<11>());
    static_assert(bf.get_bit<10>());
    static_assert(!bf.get_bit<9>());
    static_assert(bf.get_bit<8>());

    static_assert(!bf.get_bit<7>());
    static_assert(bf.get_bit<6>());
    static_assert(!bf.get_bit<5>());
    static_assert(bf.get_bit<4>());
    static_assert(bf.get_bit<3>());
    static_assert(!bf.get_bit<2>());
    static_assert(bf.get_bit<1>());
    static_assert(!bf.get_bit<0>());
}

ut_group(bit_field,
         ut_get_test(set_bit),
         ut_get_test(set_bit_range),
         ut_get_test(get_bit)
);

void run_bit_field_tests()
{
    ut_run_group(ut_get_group(bit_field));
}
