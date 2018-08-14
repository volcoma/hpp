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
	caller_false call(bool_constant<true> tag, F&& f)
	{
		static_assert(std::is_same<void, std::result_of_t<F(bool_constant<true>)>>::value,
					  "This version of 'if constexpr(...)' doesn't allow the usage of the "
					  "following statements 'return', 'break', 'continue' and 'goto'");
		f(tag);
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

#define constexpr_impl_begin_branch [&](auto)
#define constexpr_impl_end_branch )

#define if_constexpr(...) hpp::cexpr::caller_true().call(hpp::cexpr::bool_constant<(__VA_ARGS__)>(), constexpr_impl_begin_branch
#define else_if_constexpr(...) constexpr_impl_end_branch.call(hpp::cexpr::bool_constant<(__VA_ARGS__)>(), constexpr_impl_begin_branch
#define else_constexpr constexpr_impl_end_branch.call(hpp::cexpr::bool_constant<true>(), constexpr_impl_begin_branch
#define end_if_constexpr constexpr_impl_end_branch
