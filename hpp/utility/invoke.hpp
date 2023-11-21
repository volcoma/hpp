#pragma once
#include <cstddef>
#include <type_traits>
#include <utility>
#include <functional>
namespace hpp
{
template <typename... Args>
inline void ignore(Args&&...)
{
}

namespace detail
{
template <class T>
struct is_reference_wrapper : std::false_type
{
};
template <class U>
struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type
{
};
/*
 * invoke implemented as per the C++17 standard specification.
 */
template <class B, class T, class D, class... Args>
constexpr inline auto
invoke(T B::*f, D&& d,
	   Args&&... args) noexcept(noexcept((std::forward<D>(d).*f)(std::forward<Args>(args)...))) ->
	typename std::enable_if<std::is_function<T>::value &&
								std::is_base_of<B, typename std::decay<D>::type>::value,
							decltype((std::forward<D>(d).*f)(std::forward<Args>(args)...))>::type
{
	return (std::forward<D>(d).*f)(std::forward<Args>(args)...);
}

template <class B, class T, class R, class... Args>
constexpr inline auto invoke(T B::*f, R&& r,
							 Args&&... args) noexcept(noexcept((r.get().*f)(std::forward<Args>(args)...))) ->
	typename std::enable_if<std::is_function<T>::value &&
								is_reference_wrapper<typename std::decay<R>::type>::value,
							decltype((r.get().*f)(std::forward<Args>(args)...))>::type
{
	return (r.get().*f)(std::forward<Args>(args)...);
}

template <class B, class T, class P, class... Args>
constexpr inline auto
invoke(T B::*f, P&& p,
	   Args&&... args) noexcept(noexcept(((*std::forward<P>(p)).*f)(std::forward<Args>(args)...))) ->
	typename std::enable_if<std::is_function<T>::value &&
								!is_reference_wrapper<typename std::decay<P>::type>::value &&
								!std::is_base_of<B, typename std::decay<P>::type>::value,
							decltype(((*std::forward<P>(p)).*f)(std::forward<Args>(args)...))>::type
{
	return ((*std::forward<P>(p)).*f)(std::forward<Args>(args)...);
}

template <class B, class T, class D>
constexpr inline auto invoke(T B::*m, D&& d) noexcept(noexcept(std::forward<D>(d).*m)) ->
	typename std::enable_if<!std::is_function<T>::value &&
								std::is_base_of<B, typename std::decay<D>::type>::value,
							decltype(std::forward<D>(d).*m)>::type
{
	return std::forward<D>(d).*m;
}

template <class B, class T, class R>
constexpr inline auto invoke(T B::*m, R&& r) noexcept(noexcept(r.get().*m)) ->
	typename std::enable_if<!std::is_function<T>::value &&
								is_reference_wrapper<typename std::decay<R>::type>::value,
							decltype(r.get().*m)>::type
{
	return r.get().*m;
}

template <class B, class T, class P>
constexpr inline auto invoke(T B::*m, P&& p) noexcept(noexcept((*std::forward<P>(p)).*m)) ->
	typename std::enable_if<!std::is_function<T>::value &&
								!is_reference_wrapper<typename std::decay<P>::type>::value &&
								!std::is_base_of<B, typename std::decay<P>::type>::value,
							decltype((*std::forward<P>(p)).*m)>::type
{
	return (*std::forward<P>(p)).*m;
}

template <class Callable, class... Args>
constexpr inline auto
invoke(Callable&& c,
	   Args&&... args) noexcept(noexcept(std::forward<Callable>(c)(std::forward<Args>(args)...)))
	-> decltype(std::forward<Callable>(c)(std::forward<Args>(args)...))
{
	return std::forward<Callable>(c)(std::forward<Args>(args)...);
}

} // namespace detail

template <class F, class... Args>
constexpr inline auto
invoke(F&& f,
	   Args&&... args) noexcept(noexcept(detail::invoke(std::forward<F>(f), std::forward<Args>(args)...)))
	-> decltype(detail::invoke(std::forward<F>(f), std::forward<Args>(args)...))
{
	return detail::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}


template <class C, typename Ret, typename... Ts>
std::function<Ret(Ts...)> bind(C* c, Ret (C::*m)(Ts...))
{
    return [=](auto&&... args) { return (c->*m)(std::forward<decltype(args)>(args)...); };
}

template <class C, typename Ret, typename... Ts>
std::function<Ret(Ts...)> bind(const C* c, Ret (C::*m)(Ts...) const)
{
    return [=](auto&&... args) { return (c->*m)(std::forward<decltype(args)>(args)...); };
}

template <class C, typename Ret, typename... Ts>
std::function<Ret(Ts...)> bind(C* const c, Ret (C::*m)(Ts...) const)
{
    return [=](auto&&... args) { return (c->*m)(std::forward<decltype(args)>(args)...); };
}

// Conforming C++14 implementation (is also a valid C++11 implementation):
namespace detail
{
///////////////////////////////////////////////////////////////////////////

template <typename T, typename Enable = void>
struct invoke_result_impl
{
};

template <typename F, typename... Ts>
struct invoke_result_impl<F(Ts...), decltype((void)invoke(std::declval<F>(), std::declval<Ts>()...))>
{
	using type = decltype(invoke(std::declval<F>(), std::declval<Ts>()...));
};
} // namespace detail

//! template <class Fn, class... ArgTypes> struct invoke_result;
//!
//! - _Comments_: If the expression `INVOKE(std::declval<Fn>(),
//!   std::declval<ArgTypes>()...)` is well-formed when treated as an
//!   unevaluated operand, the member typedef `type` names the type
//!   `decltype(INVOKE(std::declval<Fn>(), std::declval<ArgTypes>()...))`;
//!   otherwise, there shall be no member `type`. Access checking is
//!   performed as if in a context unrelated to `Fn` and `ArgTypes`. Only
//!   the validity of the immediate context of the expression is considered.
//!
//! - _Preconditions_: `Fn` and all types in the template parameter pack
//!   `ArgTypes` are complete types, _cv_ `void`, or arrays of unknown
//!   bound.
template <typename Fn, typename... ArgTypes>
struct invoke_result : detail::invoke_result_impl<Fn && (ArgTypes && ...)>
{
};

//! template <class Fn, class... ArgTypes>
//! using invoke_result_t = typename invoke_result<Fn, ArgTypes...>::type;
template <typename Fn, typename... ArgTypes>
using invoke_result_t = typename invoke_result<Fn, ArgTypes...>::type;
} // namespace hpp
