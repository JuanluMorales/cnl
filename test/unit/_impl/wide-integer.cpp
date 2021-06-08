
//          Copyright John McFarlane 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cnl/_impl/wide-integer.h>

#include <cnl/_impl/num_traits/rounding.h>
#include <cnl/_impl/rounding/native_rounding_tag.h>

#include <cnl/_impl/type_traits/identical.h>

#include <gtest/gtest.h>

using cnl::_impl::identical;

static_assert(cnl::is_integer_v<math::wide_integer::uintwide_t<64>>);

static_assert(
        std::is_same_v<
                cnl::native_rounding_tag,
                cnl::rounding_t<math::wide_integer::uintwide_t<64>>>);

TEST(wide_integer, float_ctor)
{
    auto constexpr expected{math::wide_integer::uintwide_t<64>(42)};
    auto const actual{math::wide_integer::uintwide_t<64>(42.f)};
    ASSERT_EQ(expected, actual);
}

namespace test_conversion_op {
    static_assert(identical(
            42,
            static_cast<int>(math::wide_integer::uintwide_t<64>{42})));
    static_assert(identical(
            42.L,
            static_cast<long double>(math::wide_integer::uintwide_t<64>{42})));
}

TEST(wide_integer, lowest_max)  // NOLINT
{
    using type = math::wide_integer::uintwide_t<64, unsigned, void, true>;
    using limits = std::numeric_limits<type>;
    constexpr auto expected{-1-limits::max()};
    constexpr auto actual{limits::lowest()};
    ASSERT_EQ(expected, actual);
}
