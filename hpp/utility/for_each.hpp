#pragma once
#include "invoke.hpp"
#include <tuple>

namespace hpp
{
namespace detail
{

template <typename Tuple, typename F, std::size_t... Indices>
void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>)
{
	ignore(tuple, f);
	using swallow = int[];
	(void)swallow{1, (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...};
}

} // namespace detail

template <typename Tuple, typename F>
void for_each(Tuple&& tuple, F&& f)
{
	detail::for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
						  std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
}
} // namespace hpp
