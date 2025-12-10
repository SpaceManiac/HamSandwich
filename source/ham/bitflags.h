#ifndef BITFLAGS_H
#define BITFLAGS_H

#include <type_traits> // for underlying_type_t

// Helper macro to define bitflag operators on an `enum` or `enum class`.
#define BITFLAGS(TY) \
	inline constexpr TY  operator| (TY  a, TY b) { return     static_cast<TY>( static_cast<std::underlying_type_t<TY>>(a) | static_cast<std::underlying_type_t<TY>>(b)); } \
	inline constexpr TY &operator|=(TY &a, TY b) { return a = static_cast<TY>( static_cast<std::underlying_type_t<TY>>(a) | static_cast<std::underlying_type_t<TY>>(b)); } \
	inline constexpr TY  operator& (TY  a, TY b) { return     static_cast<TY>( static_cast<std::underlying_type_t<TY>>(a) & static_cast<std::underlying_type_t<TY>>(b)); } \
	inline constexpr TY &operator&=(TY &a, TY b) { return a = static_cast<TY>( static_cast<std::underlying_type_t<TY>>(a) & static_cast<std::underlying_type_t<TY>>(b)); } \
	inline constexpr TY  operator^ (TY  a, TY b) { return     static_cast<TY>( static_cast<std::underlying_type_t<TY>>(a) ^ static_cast<std::underlying_type_t<TY>>(b)); } \
	inline constexpr TY &operator^=(TY &a, TY b) { return a = static_cast<TY>( static_cast<std::underlying_type_t<TY>>(a) ^ static_cast<std::underlying_type_t<TY>>(b)); } \
	inline constexpr TY  operator~ (TY  a)       { return     static_cast<TY>(~static_cast<std::underlying_type_t<TY>>(a)); }

#endif
