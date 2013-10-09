// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_COMMON
#define SSVOB_CESYSTEM_COMMON

#include <stack>
#include <vector>
#include <tuple>
#include <numeric>
#include <cstdint>
#include <array>
#include <cassert>
#include <unordered_map>
#include <bitset>
#include <SSVUtils/SSVUtils.h>

namespace ssvces
{
	class Component { };

	using Id = std::size_t;
	using Ctr = std::uint8_t;
	using TypeId = std::size_t;
	static constexpr std::size_t maxEntities{1000000};
	static constexpr std::size_t maxComponentsPerEntity{32};
	using Bitset = std::bitset<maxComponentsPerEntity>;

	namespace Internal
	{
		// Last used bit index
		static unsigned int lastTypeIdBitIdx{0};

		template<typename T> struct TypeIdStorage
		{
			// TypeIdStorage statically stores the TypeId and bit index of a Component type

			static_assert(std::is_base_of<Component, T>::value, "TypeIdStorage only works with types that derive from Component");

			static const TypeId typeId;
			static const std::size_t bitIdx;
		};
		template<typename T> const TypeId TypeIdStorage<T>::typeId{typeid(T).hash_code()};
		template<typename T> const std::size_t TypeIdStorage<T>::bitIdx{lastTypeIdBitIdx++};

		// These functions use variadic template recursion to "build" a bitset for a set of Component types
		template<typename T> inline static void buildBitsetHelper(Bitset& mBitset) noexcept { mBitset.set(Internal::TypeIdStorage<T>::bitIdx); }
		template<typename T1, typename T2, typename... TArgs> inline static void buildBitsetHelper(Bitset& mBitset) noexcept { buildBitsetHelper<T1>(mBitset); buildBitsetHelper<T2, TArgs...>(mBitset); }
		template<typename... TArgs> inline static Bitset getBuildBitset() noexcept { Bitset result; buildBitsetHelper<TArgs...>(result); return result; }

		template<typename... TArgs> struct TypeIdPackBitset{ static const Bitset bitset; };
		template<typename... TArgs> const Bitset TypeIdPackBitset<TArgs...>::bitset{getBuildBitset<TArgs...>()};
	}

	// Shortcut to get the static TypeId of a Component type from TypeIdStorage
	template<typename T> inline constexpr const TypeId& getTypeId() noexcept { return Internal::TypeIdStorage<T>::typeId; }

	// Shortcut to get the static Bitset of a pack of Component types
	template<typename... TArgs> inline constexpr static const Bitset& getTypeIdPackBitset() noexcept { return Internal::TypeIdPackBitset<TArgs...>::bitset; }

	// Shortcut to get the bit index of a Component type
	template<typename T> inline constexpr static const std::size_t& getTypeIdBitIdx() noexcept { return Internal::TypeIdStorage<T>::bitIdx; }

	// Shortcut to append a bit of a Component type to an existing bitset
	template<typename T> inline static void appendTypeIdBit(Bitset& mBitset) noexcept { Internal::buildBitsetHelper<T>(mBitset); }
}

#endif
