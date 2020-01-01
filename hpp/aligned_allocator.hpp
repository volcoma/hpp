#ifndef ALLIGNED_ALLOCATOR_HPP
#define ALLIGNED_ALLOCATOR_HPP

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <cassert>

namespace hpp
{

void* aligned_malloc(size_t size, size_t alignment);
void aligned_free(void* ptr);
/**
 * @class aligned_allocator
 * @brief Allocator for aligned memory
 *
 * The aligned_allocator class template is an allocator that
 * performs memory allocation aligned by the specified value.
 *
 * @tparam T type of objects to allocate.
 * @tparam Align alignment in bytes.
 */
template <class T, size_t Align = alignof(T)>
class aligned_allocator
{
public:
	constexpr static bool is_power_of_2(size_t x)
	{
		return x > 0 && !(x & (x - 1));
	}

	static_assert(Align > 0, "Alignment must be greater than 0");
	static_assert(is_power_of_2(Align), "Alignment must be a power of 2");

	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

	template <class U>
	struct rebind
	{
		using other = aligned_allocator<U, Align>;
	};

	aligned_allocator() noexcept;
	aligned_allocator(const aligned_allocator& rhs) noexcept;

	template <class U>
	aligned_allocator(const aligned_allocator<U, Align>& rhs) noexcept;
	~aligned_allocator();

	pointer allocate(size_type n, const void* hint = nullptr);
	void deallocate(pointer p, size_type n);

	size_type max_size() const noexcept;
};

template <class T1, size_t Align1, class T2, size_t Align2>
bool operator==(const aligned_allocator<T1, Align1>& lhs, const aligned_allocator<T2, Align2>& rhs) noexcept;

template <class T1, size_t Align1, class T2, size_t Align2>
bool operator!=(const aligned_allocator<T1, Align1>& lhs, const aligned_allocator<T2, Align2>& rhs) noexcept;

/************************************
 * aligned_allocator implementation *
 ************************************/

/**
 * Default constructor.
 */
template <class T, size_t A>
inline aligned_allocator<T, A>::aligned_allocator() noexcept = default;

/**
 * Copy constructor.
 */
template <class T, size_t A>
inline aligned_allocator<T, A>::aligned_allocator(const aligned_allocator&) noexcept = default;

/**
 * Extended copy constructor.
 */
template <class T, size_t A>
template <class U>
inline aligned_allocator<T, A>::aligned_allocator(const aligned_allocator<U, A>&) noexcept
{
}

/**
 * Destructor.
 */
template <class T, size_t A>
inline aligned_allocator<T, A>::~aligned_allocator() = default;

/**
 * Allocates <tt>n * sizeof(T)</tt> bytes of uninitialized memory, aligned by \c A.
 * The alignment may require some extra memory allocation.
 * @param n the number of objects to allocate storage for.
 * @param hint unused parameter provided for standard compliance.
 * @return a pointer to the first byte of a memory block suitably aligned and sufficient to
 * hold an array of \c n objects of type \c T.
 */
template <class T, size_t A>
inline auto aligned_allocator<T, A>::allocate(size_type n, const void*) -> pointer
{
	auto res = reinterpret_cast<pointer>(aligned_malloc(sizeof(T) * n, A));
	if(res == nullptr)
		throw std::bad_alloc();
	return res;
}

/**
 * Deallocates the storage referenced by the pointer p, which must be a pointer obtained by
 * an earlier call to allocate(). The argument \c n must be equal to the first argument of the call
 * to allocate() that originally produced \c p; otherwise, the behavior is undefined.
 * @param p pointer obtained from allocate().
 * @param n number of objects earlier passed to allocate().
 */
template <class T, size_t A>
inline void aligned_allocator<T, A>::deallocate(pointer p, size_type)
{
	aligned_free(p);
}

/**
 * Returns the maximum theoretically possible value of \c n, for which the
 * call allocate(n, 0) could succeed.
 * @return the maximum supported allocated size.
 */
template <class T, size_t A>
inline auto aligned_allocator<T, A>::max_size() const noexcept -> size_type
{
	return size_type(-1) / sizeof(T);
}

/**
 * @defgroup allocator_comparison Comparison operators
 */

/**
 * @ingroup allocator_comparison
 * Compares two aligned memory allocator for equality. Since allocators
 * are stateless, return \c true iff <tt>A1 == A2</tt>.
 * @param lhs aligned_allocator to compare.
 * @param rhs aligned_allocator to compare.
 * @return true if the allocators have the same alignment.
 */
template <class T1, size_t A1, class T2, size_t A2>
inline bool operator==(const aligned_allocator<T1, A1>& /*lhs*/, const aligned_allocator<T2, A2>& /*rhs*/) noexcept
{
	return A1 == A2;
}

/**
 * @ingroup allocator_comparison
 * Compares two aligned memory allocator for inequality. Since allocators
 * are stateless, return \c true iff <tt>A1 != A2</tt>.
 * @param lhs aligned_allocator to compare.
 * @param rhs aligned_allocator to compare.
 * @return true if the allocators have different alignments.
 */
template <class T1, size_t A1, class T2, size_t A2>
inline bool operator!=(const aligned_allocator<T1, A1>& lhs, const aligned_allocator<T2, A2>& rhs) noexcept
{
	return !(lhs == rhs);
}

//****************************************
//* aligned malloc / free implementation *
//****************************************
inline void* aligned_malloc(size_t required_bytes, size_t alignment)
{
	void* res = nullptr;
	// we allocate extra bytes to keep the unaligned pointer there
	// so we can access it fast for when we need to free it.
	size_t offset = alignment - 1 + sizeof(void*);
	size_t malloc_size = required_bytes + offset;
	void* ptr = std::malloc(malloc_size);
	if(ptr != nullptr && alignment != 0)
	{
		res = reinterpret_cast<void*>((reinterpret_cast<size_t>(ptr) + offset)&~(size_t(alignment - 1)));
		*(reinterpret_cast<void**>(res) - 1) = ptr;
	}
	assert(uintptr_t(res) % alignment == 0);

	return res;
}

inline void aligned_free(void* ptr)
{
	if(ptr != nullptr)
	{
		auto unaligned = *(reinterpret_cast<void**>(ptr) - 1);
		std::free(unaligned);
	}
}
} // namespace hpp

#endif
