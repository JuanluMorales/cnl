
//          Copyright John McFarlane 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#if !defined(CNL_IMPL_DUPLEX_INTEGER_DIVIDE_H)
#define CNL_IMPL_DUPLEX_INTEGER_DIVIDE_H

#include "../custom_operator/definition.h"
#include "../custom_operator/native_tag.h"
#include "../custom_operator/op.h"
#include "../numbers/set_signedness.h"
#include "../wide_integer/definition.h"
#include "ctors.h"
#include "definition.h"
#include "numbers.h"
#include "numeric_limits.h"

#include <algorithm>

/// compositional numeric library
namespace cnl {
    namespace _impl {
        // cnl::_impl::heterogeneous_duplex_divide_operator
        template<typename Lhs, typename Rhs>
        struct heterogeneous_duplex_divide_operator {
            using common_type = rep_of_t<wide_integer<
                    std::max(digits_v<Lhs>, digits_v<Rhs>),
                    numbers::set_signedness_t<int, numbers::signedness_v<Lhs> | numbers::signedness_v<Rhs>>>>;

            [[nodiscard]] constexpr auto operator()(Lhs const& lhs, Rhs const& rhs) const -> Lhs
            {
                return static_cast<Lhs>(
                        static_cast<common_type>(lhs) / static_cast<common_type>(rhs));
            }
        };
    }

    // duplex_integer<> / duplex_integer<>
    template<typename Upper, typename Lower>
    struct custom_operator<
            _impl::divide_op,
            op_value<_impl::duplex_integer<Upper, Lower>>,
            op_value<_impl::duplex_integer<Upper, Lower>>> {
    private:
        using duplex_integer = _impl::duplex_integer<Upper, Lower>;
        using unsigned_duplex_integer = numbers::set_signedness_t<duplex_integer, false>;

    public:
        [[nodiscard]] constexpr auto operator()(
                duplex_integer const& lhs, duplex_integer const& rhs) const -> duplex_integer
        {
            return (lhs < duplex_integer{0}) ? (rhs < duplex_integer{0})
                                                     ? non_negative_division(-lhs, -rhs)
                                                     : -non_negative_division(-lhs, rhs)
                 : (rhs < duplex_integer{0}) ? -non_negative_division(lhs, -rhs)
                                             : non_negative_division(lhs, rhs);
        }

        // lifted from:
        // https://github.com/torvalds/linux/blob/5ac94332248ee017964ba368cdda4ce647e3aba7/lib/math/div64.c#L142
        static constexpr auto non_negative_division(
                unsigned_duplex_integer const& dividend, unsigned_duplex_integer const& divisor)
                -> unsigned_duplex_integer
        {
            auto const high = divisor.upper();
            if (!high) {
                return div_by_lower(dividend, divisor.lower());
            }

            int n = fls(high);
            auto quot = div_by_lower(dividend >> n, (divisor >> n).lower());

            if (quot) {
                --quot;
            }
            if ((dividend - quot * divisor) >= divisor) {
                ++quot;
            }

            return quot;
        }

        static constexpr auto fls(Upper n) -> int
        {
            auto half_digits = numeric_limits<duplex_integer>::digits / 2;

            if (!n) {
                return 0;
            }
            auto const msd = Upper{1} << (half_digits - 1);
            for (int r = half_digits;; n <<= 1, r--) {
                if (n & msd) {
                    return r;
                }
            }
        };

        // from Linux div64_32
        static constexpr auto div_by_lower(
                unsigned_duplex_integer const& dividend, Lower const& divisor)
                -> unsigned_duplex_integer
        {
            unsigned_duplex_integer rem = dividend;
            unsigned_duplex_integer b = divisor;
            unsigned_duplex_integer d = 1;

            using unsigned_upper = numbers::set_signedness_t<Upper, false>;
            auto high = rem.upper();

            unsigned_duplex_integer quot = 0;
            if (static_cast<unsigned_upper>(high) >= divisor) {
                high /= divisor;  // NOLINT(clang-analyzer-core.DivideZero)
                quot = unsigned_duplex_integer{high, 0};
                rem -= unsigned_duplex_integer(high * divisor, 0);
            }

            while (b < rem) {
                b <<= 1;
                d <<= 1;
            }

            do {
                if (rem >= b) {
                    rem -= b;
                    quot += d;
                }
                b >>= 1;
                d >>= 1;
            } while (d);

            return quot;
        };
    };

    template<typename LhsUpper, typename LhsLower, typename RhsUpper, typename RhsLower>
    struct custom_operator<
            _impl::divide_op,
            op_value<_impl::duplex_integer<LhsUpper, LhsLower>>,
            op_value<_impl::duplex_integer<RhsUpper, RhsLower>>>
        : _impl::heterogeneous_duplex_divide_operator<
                  _impl::duplex_integer<LhsUpper, LhsLower>,
                  _impl::duplex_integer<RhsUpper, RhsLower>> {
    };

    template<typename Lhs, typename RhsUpper, typename RhsLower>
    struct custom_operator<
            _impl::divide_op,
            op_value<Lhs>,
            op_value<_impl::duplex_integer<RhsUpper, RhsLower>>>
        : _impl::heterogeneous_duplex_divide_operator<
                  Lhs, _impl::duplex_integer<RhsUpper, RhsLower>> {
    };

    template<typename LhsUpper, typename LhsLower, typename Rhs>
    struct custom_operator<
            _impl::divide_op,
            op_value<_impl::duplex_integer<LhsUpper, LhsLower>>,
            op_value<Rhs>>
        : _impl::heterogeneous_duplex_divide_operator<
                  _impl::duplex_integer<LhsUpper, LhsLower>, Rhs> {
    };
}

#endif  // CNL_IMPL_DUPLEX_INTEGER_DIVIDE_H
