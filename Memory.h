/**
 * MIT License
 *
 * Copyright(c) 2023 Bruno Cecconi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef BC_MEMORY_H
#define BC_MEMORY_H

#include "Platform.h"

#include <type_traits>
#include <cassert>

#ifndef BC_ALLOCATION_FUNCTIONS
#define BC_ALLOCATION_FUNCTIONS
#if _WIN32
#define BC_MALLOC(N)					_aligned_malloc(N, BC_PLATFORM_ALIGNMENT)
#define BC_MALLOC_ALIGNED(N, ALIGNMENT) _aligned_malloc(N, ALIGNMENT)
#define BC_FREE(BLOCK, N)				_aligned_free(BLOCK)
#elif __linux__
#define BC_MALLOC(N)					aligned_alloc(N, BC_PLATFORM_ALIGNMENT)
#define BC_MALLOC_ALIGNED(N, ALIGNMENT) aligned_alloc(N, ALIGNMENT)
#define BC_FREE(BLOCK, N)				free(BLOCK)
#endif
#endif

#ifndef BC_MEMORY_MANIPULATION_FUNCTIONS
#define BC_MEMORY_MANIPULATION_FUNCTIONS
#if _WIN32
#define BC_MEMCHR(PTR, VALUE, N) memchr(STR, VALUE, N)
#define BC_MEMCMP(A, B, N)		 memcmp(A, B, N)
#define BC_MEMCPY(DST, SRC, N)	 memcpy(DST, SRC, N)
#define BC_MEMMOVE(DST, SRC, N)	 memcpy(DST, SRC, N)
#define BC_MEMSET(PTR, VALUE, N) memset(PTR, VALUE, N)
#define BC_MEMZERO(PTR, N)		 BC_MEMSET(PTR, 0, N)
#elif __linux__
#define BC_MEMCHR(PTR, VALUE, N) memchr(STR, VALUE, N)
#define BC_MEMCMP(A, B, N)		 memcmp(A, B, N)
#define BC_MEMCPY(DST, SRC, N)	 memcpy(DST, SRC, N)
#define BC_MEMMOVE(DST, SRC, N)	 memcpy(DST, SRC, N)
#define BC_MEMSET(PTR, VALUE, N) memset(PTR, VALUE, N)
#define BC_MEMZERO(PTR, N)		 BC_MEMSET(PTR, 0, N)
#endif
#endif

template<typename T>
struct Instance
{
	using Type = T;
	T*			 Value{};
	BC_INLINE T* operator->() const
	{
		assert(Value && "Invalid pointer.");
		return Value;
	}
	BC_INLINE BC_EXPLICIT operator bool() const
	{
		return Value != nullptr;
	}
};

template<typename T>
struct ArrayInstance
{
	using Type = T;
	T*			 Value;
	size_t		 Size{};
	BC_INLINE T& operator[](size_t Index) const
	{
		assert(Value && "Invalid pointer.");
		assert(Index < Size && "Out of bounds.");
		return Value[Index];
	}
	BC_INLINE BC_EXPLICIT operator bool() const
	{
		return Value != nullptr;
	}
	BC_INLINE const T* begin() const
	{
		return Value;
	}
	BC_INLINE const T* end() const
	{
		return Value + Size;
	}
};

template<typename TIterator>
BC_INLINE TIterator UninitializedCopyFill(TIterator Begin, TIterator End, std::remove_pointer_t<TIterator> Value)
{
	static_assert(std::is_pointer_v<TIterator>, "Invalid iterator type.");
	static_assert(std::is_copy_assignable_v<std::remove_pointer_t<TIterator>>, "Type is not copy assignable.");
	auto lData = Begin;
	while (lData < End)
		*(lData++) = Value;
	return Begin;
}

template<typename TIterator, typename... TArgs>
BC_INLINE TIterator UninitializedConstruct(TIterator Begin, TIterator End, TArgs&&... Args)
{
	using Type = std::remove_pointer_t<TIterator>;
	static_assert(std::is_pointer_v<TIterator>, "Invalid iterator type.");
	static_assert(std::is_move_assignable_v<Type>, "Type is not move assignable.");
	auto lData = Begin;
	while (lData < End)
		*(lData++) = Type{std::forward<TArgs>(Args)...};
	return Begin;
}

template<typename TIterator, typename... TArgs>
BC_INLINE TIterator UninitializedConstruct(const TIterator Begin, const size_t Size, TArgs&&... Args)
{
	return UninitializedConstruct<TIterator, TArgs...>(Begin, Begin + Size, std::forward<TArgs>(Args)...);
}

template<typename TIterator, typename... TArgs>
BC_INLINE TIterator Destruct(TIterator Begin, TIterator End)
{
	using Type = std::remove_pointer_t<TIterator>;
	static_assert(std::is_pointer_v<TIterator>, "Invalid iterator type.");
	static_assert(std::is_move_assignable_v<Type>, "Type is not move assignable.");
	auto lData = Begin;
	while (lData < End)
		(lData++)->~Type();
	return Begin;
}

template<typename TIterator, typename... TArgs>
BC_INLINE TIterator Destruct(TIterator Begin, const size_t Size)
{
	return Destruct<TIterator>(Begin, Begin + Size);
}

template<typename T, typename... TArgs>
BC_INLINE Instance<T> Create(TArgs&&... Args)
{
	if constexpr (std::is_fundamental_v<T>)
	{
		return Instance<T>{new (BC_MALLOC_ALIGNED(sizeof(T), alignof(T))) T{Args...}};
	}
	else
	{
		return Instance<T>{new (BC_MALLOC_ALIGNED(sizeof(T), alignof(T))) T{std::forward<TArgs>(Args)...}};
	}
}

template<typename T>
BC_INLINE void Destroy(Instance<T>& Value)
{
	Value.Value->~T();
	BC_MEMZERO(Value->Value, sizeof(T));
	Value.Value = nullptr;
}

template<typename T, typename... TArgs>
BC_INLINE ArrayInstance<T> CreateArray(const size_t Size, TArgs&&... Args)
{
	assert(Size > 0ull && "Invalid array size.");
	if constexpr (std::is_fundamental_v<T>)
	{
		return ArrayInstance<T>{
			UninitializedConstruct<T*>(new (BC_MALLOC_ALIGNED(sizeof(T) * Size, alignof(T))) T[Size], Size, Args...),
			Size};
	}
	else
	{
		return ArrayInstance<T>{UninitializedConstruct<T*>(new (BC_MALLOC_ALIGNED(sizeof(T) * Size, alignof(T)))
															   T[Size],
														   Size, std::forward<TArgs>(Args)...),
								Size};
	}
}

template<typename T>
BC_INLINE void DestroyArray(ArrayInstance<T>& Value)
{
	BC_MEMZERO(Destruct<T*>(Value.Value, Value.Size), sizeof(T) * Value.Size);
	Value.Value = nullptr;
}

#endif
