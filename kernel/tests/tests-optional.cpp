/*
** Created by doom on 07/11/18.
*/

#include "tests_config.hpp"
#include <utils/optional.hpp>

ut_test(map)
{
    utils::optional<int> o;
    auto mult2 = [](int i) {
        return i * 2;
    };

    auto res1 = o.map(mult2);
    ut_assert_false(res1.has_value());

    o = 123;
    auto res2 = o.map(mult2);
    ut_assert(res2.has_value());
    ut_assert_eq(*res2, 246);
}

ut_test(and_then)
{
    utils::optional<int> o;
    auto mult2_if_odd = [](int i) -> utils::optional<int> {
        if (i % 2 == 0)
            return std::nullopt;
        return {i * 2};
    };

    auto res1 = o.and_then(mult2_if_odd);
    ut_assert_false(res1.has_value());

    o = 122;
    auto res2 = o.and_then(mult2_if_odd);
    ut_assert_false(res2.has_value());

    o = 123;
    auto res3 = o.and_then(mult2_if_odd);
    ut_assert(res3.has_value());
    ut_assert_eq(*res3, 246);
}

ut_test(reference)
{
    int i = 2;
    utils::optional<int &> opt(i);

    ut_assert_eq(*opt, i);
    ++i;
    ut_assert_eq(*opt, i);
    --*opt;
    ut_assert_eq(*opt, i);
    i = 0;
    ut_assert_eq(*opt, i);

    int j = 3;
    opt = j;
    ut_assert_eq(*opt, j);
    ++j;
    ut_assert_eq(*opt, j);
    --*opt;
    ut_assert_eq(*opt, j);

    utils::optional<int &> opt2(opt);
    ut_assert_eq(*opt, j);
    ut_assert_eq(*opt2, j);
    ++j;
    ut_assert_eq(*opt, j);
    ut_assert_eq(*opt2, j);
    --*opt;
    ut_assert_eq(*opt, j);
    ut_assert_eq(*opt2, j);

    utils::optional<int &> opt3;
    ut_assert_false(opt3);
    opt3 = opt;
    ut_assert_eq(*opt, j);
    ut_assert_eq(*opt3, j);
    ++j;
    ut_assert_eq(*opt, j);
    ut_assert_eq(*opt3, j);
    --*opt;
    ut_assert_eq(*opt, j);
    ut_assert_eq(*opt3, j);

    opt3.map([](int &j_ref) -> int & {
        j_ref = 2;
        return j_ref;
    });
    ut_assert_eq(j, 2);
    ut_assert_eq(*opt3, 2);
}

ut_group(optional,
         ut_get_test(map),
         ut_get_test(and_then),
         ut_get_test(reference)
);

void run_optional_tests()
{
    ut_run_group(ut_get_group(optional));
}
