#pragma once

#include <type_traits>
#include <utility>

namespace hpp
{

namespace cexpr
{
template <bool predicate>
using bool_constant = std::integral_constant<bool, predicate>;

struct caller_false
{
	template <bool E, typename F>
	caller_false call(bool_constant<E>, F&&)
	{
		return {};
	}
};
struct caller_true
{
	template <typename F>
	caller_false call(bool_constant<true>, F&& f)
	{
		f(std::true_type{});
		return {};
	}
	template <typename F>
	caller_true call(bool_constant<false>, F&&)
	{
		return {};
	}
};
} // namespace cexpr
} // namespace hpp

#define constexpr_if(...) hpp::cexpr::caller_true().call(hpp::cexpr::bool_constant<(__VA_ARGS__)>(), [&](auto /*_cexrp_arg_*/)
#define constexpr_else_if(...) ).call(hpp::cexpr::bool_constant<(__VA_ARGS__)>(), [&](auto /*_cexrp_arg_*/)
#define constexpr_else ).call(hpp::cexpr::bool_constant<true>(),[&](auto /*_cexrp_arg_*/)
#define constexpr_end_if )
