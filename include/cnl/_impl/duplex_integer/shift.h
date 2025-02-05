
//          Copyright John McFarlane 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#if !defined(CNL_IMPL_DUPLEX_INTEGER_SHIFT_H)
#define CNL_IMPL_DUPLEX_INTEGER_SHIFT_H

#include "../custom_operator/definition.h"
#include "../custom_operator/op.h"
#include "definition.h"

/// compositional numeric library
namespace cnl {
    template<typename Upper, typename Lower, typename Rhs>
    struct custom_operator<
            _impl::shift_left_op,
            op_value<_impl::duplex_integer<Upper, Lower>>,
            op_value<Rhs>> {
    private:
        using duplex_integer = _impl::duplex_integer<Upper, Lower>;
        [[nodiscard]] constexpr auto with_int(duplex_integer const& lhs, int const& rhs) const
                -> duplex_integer
        {
            return duplex_integer(
                    _impl::sensible_left_shift<Upper>(lhs.upper(), rhs)
                            | _impl::extra_sensible_right_shift<Upper>(
                                    lhs.lower(), _impl::width<Lower> - rhs),
                    _impl::sensible_left_shift<Lower>(lhs.lower(), rhs));
        }

    public:
        [[nodiscard]] constexpr auto operator()(duplex_integer const& lhs, Rhs const& rhs) const
                -> duplex_integer
        {
            return with_int(lhs, static_cast<int>(rhs));
        }
    };

    template<typename Upper, typename Lower, typename Rhs>
    struct custom_operator<
            _impl::shift_right_op,
            op_value<_impl::duplex_integer<Upper, Lower>>,
            op_value<Rhs>> {
    private:
        using duplex_integer = _impl::duplex_integer<Upper, Lower>;

        [[nodiscard]] constexpr auto with_int(duplex_integer const& lhs, int rhs) const
                -> duplex_integer
        {
            return duplex_integer(calculate_upper(lhs, rhs), calculate_lower(lhs, rhs));
        }

        [[nodiscard]] constexpr auto calculate_upper(duplex_integer const& lhs, int rhs) const
                -> Upper
        {
            return _impl::sensible_right_shift<Upper>(lhs.upper(), rhs);
        }

        [[nodiscard]] constexpr auto calculate_lower(duplex_integer const& lhs, int rhs) const
                -> Lower
        {
            return static_cast<Lower>(
                    _impl::sensible_right_shift<Lower>(lhs.lower(), rhs)
                    | _impl::extra_sensible_right_shift<Lower>(
                            lhs.upper(), rhs - _impl::width<Lower>));
        }

    public:
        [[nodiscard]] constexpr auto operator()(duplex_integer const& lhs, Rhs const& rhs) const
                -> duplex_integer
        {
            return with_int(lhs, static_cast<int>(rhs));
        }
    };
}

#endif  // CNL_IMPL_DUPLEX_INTEGER_SHIFT_H
