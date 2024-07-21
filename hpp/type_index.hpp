#ifndef HPP_TYPE_INDEX
#define HPP_TYPE_INDEX

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "type_name.hpp"
#include "crc.hpp"

namespace hpp
{

class type_index
{
public:
    auto operator==(const type_index& o) const noexcept -> bool;
    auto operator!=(const type_index& o) const noexcept -> bool;
    auto operator<(const type_index& o) const noexcept -> bool;
    auto operator>(const type_index& o) const noexcept -> bool;

	auto hash_code() const noexcept -> uint64_t;
    auto name() const noexcept -> hpp::string_view;

private:
    struct construct_t
    {
		uint64_t hash_code{};
        hpp::string_view name{};
        // other
    };

    construct_t info_ = {};
    explicit type_index(construct_t info) noexcept;

    template<typename T>
    static auto get() -> const type_index&;

    template<typename T>
    friend auto type_id() -> const type_index&;

    template<typename T>
    static auto construct() -> construct_t;
};

template<typename T>
auto type_index::construct() -> construct_t
{
    construct_t result;
	result.name = hpp::type_name<T>();
    result.hash_code = crc64(result.name.data(), result.name.size());
    return result;
}

template<typename T>
auto type_index::get() -> const type_index&
{
    static const type_index id(construct<T>());
    return id;
}

template<typename T>
auto type_id() -> const type_index&
{
    using type_t = typename std::remove_cv<T>::type;
    return type_index::get<type_t>();
}

inline type_index::type_index(type_index::construct_t info) noexcept : info_{info}
{
}

inline auto type_index::operator==(const type_index& o) const noexcept -> bool
{
    return hash_code() == o.hash_code();
}

inline auto type_index::operator!=(const type_index& o) const noexcept -> bool
{
    return hash_code() != o.hash_code();
}

inline auto type_index::operator<(const type_index& o) const noexcept -> bool
{
    return hash_code() < o.hash_code();
}

inline auto type_index::operator>(const type_index& o) const noexcept -> bool
{
    return hash_code() > o.hash_code();
}

inline auto type_index::hash_code() const noexcept -> uint64_t
{
    return info_.hash_code;
}

inline auto type_index::name() const noexcept -> hpp::string_view
{
    return info_.name;
}

} // namespace hpp

#endif
