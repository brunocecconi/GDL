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

#ifndef BC_ALLOCATOR_H
#define BC_ALLOCATOR_H

#include "Memory.h"
#include "Macros.h"

#include <cstddef>

struct MemoryBlock
{
	uint8_t* Ptr;
	size_t	 Size;
};

static constexpr size_t RoundToAligned(size_t Size, size_t Alignment = sizeof(std::max_align_t))
{
	return ((Size + (Alignment - 1)) & ~(Alignment - 1));
}

template<typename TFirstAllocator, typename TSecondAllocator>
class FallbackAllocator: private TFirstAllocator, private TSecondAllocator
{
public:
	MemoryBlock Allocate(size_t Size, size_t Alignment = sizeof(std::max_align_t))
	{
		MemoryBlock lMemoryBlock = TFirstAllocator::Allocate(Size, Alignment);
		if (!lMemoryBlock.Ptr)
			lMemoryBlock = TSecondAllocator::Allocate(Size, Alignment);
		return lMemoryBlock;
	}

	void Deallocate(MemoryBlock& Mb)
	{
		if (TFirstAllocator::Owns(Mb))
			TFirstAllocator::Deallocate(Mb);
		else
			TSecondAllocator::Deallocate(Mb);
		Mb = {};
	}

	[[nodiscard]] bool Owns(MemoryBlock Mb) const
	{
		return TFirstAllocator::Owns(Mb) || TSecondAllocator::Owns(Mb);
	}
};

class Mallocator
{
public:
	MemoryBlock Allocate(size_t Size, size_t Alignment = sizeof(std::max_align_t))
	{
		UNUSED(Alignment);
		return MemoryBlock{static_cast<uint8_t*>(malloc(Size)), Size};
	}

	void Deallocate(MemoryBlock& Mb)
	{
		free(Mb.Ptr);
		Mb = {};
	}

	[[nodiscard]] bool Owns(MemoryBlock Mb) const
	{
		return true;
	}
};

class WindowsMallocator
{
public:
	MemoryBlock Allocate(size_t Size, size_t Alignment = sizeof(std::max_align_t))
	{
		return MemoryBlock{static_cast<uint8_t*>(_aligned_malloc(Size, Alignment)), Size};
	}

	void Deallocate(MemoryBlock& Mb)
	{
		_aligned_free(Mb.Ptr);
		Mb = {};
	}

	[[nodiscard]] bool Owns(MemoryBlock Mb) const
	{
		return true;
	}
};

#if _WIN32
using platform_mallocator_t = WindowsMallocator;
#endif

template<size_t N>
class StackAllocator
{
	uint8_t	 mData[N] = {};
	uint8_t *mCursor{}, *mEnd{};

public:
	StackAllocator() : mCursor{mData}, mEnd{mData + N}
	{
	}

public:
	MemoryBlock Allocate(size_t Size, size_t Alignment = sizeof(std::max_align_t))
	{
		const auto lAlignedSize = RoundToAligned(Size, Alignment);
		if (lAlignedSize > (mEnd - mCursor))
		{
			return MemoryBlock{};
		}
		void* lPtr = mCursor;
		mCursor += lAlignedSize;
		return MemoryBlock{lPtr, Size};
	}

	void Deallocate(MemoryBlock& Mb)
	{
		if (Mb.Ptr + RoundToAligned(Mb.Size) == mCursor)
		{
			mCursor = Mb.Ptr;
		}
		Mb = {};
	}

	void DeallocateAll()
	{
		mCursor = mData;
	}

	[[nodiscard]] bool Owns(MemoryBlock Mb) const
	{
		return Mb.Ptr >= mData && Mb.Ptr < mEnd;
	}
};

template<typename TSupportAllocator, size_t BlockSize, size_t ToleranceMin, size_t ToleranceMax>
class FreeListAllocator
{
	static_assert(BlockSize >= sizeof(intptr_t), "BlockSize needs to be greater or equal sizeof(intptr_t).");

	struct Node
	{
		Node* Next{};
	};
	Node*			  mHead{};
	TSupportAllocator mAllocator{};

public:
	MemoryBlock Allocate(size_t Size, size_t Alignment = sizeof(std::max_align_t))
	{
#ifndef NDEBUG
		if ((Size >= ToleranceMin && Size <= ToleranceMax) && mHead)
		{
			intptr_t* lPtr = reinterpret_cast<intptr_t*>(mHead);
			mHead		   = mHead->Next;
			return MemoryBlock{lPtr + 1ull, Size};
		}
		const MemoryBlock lMemoryBlock = mAllocator.Allocate(BlockSize + sizeof(intptr_t), Alignment);
		intptr_t*		  lIntPtr	   = static_cast<intptr_t*>(lMemoryBlock.Ptr);
		*lIntPtr					   = reinterpret_cast<intptr_t>(this);
		return MemoryBlock{lIntPtr + 1ull, lMemoryBlock.Size};
#else
		if ((Size >= ToleranceMin && Size <= ToleranceMax) && mHead)
		{
			void* lPtr = mHead;
			mHead	   = mHead->Next;
			return MemoryBlock{lPtr, Size};
		}
		return mAllocator.Allocate(BlockSize, Alignment);
#endif
	}

	void Deallocation(MemoryBlock Mb)
	{
#ifndef NDEBUG
		if (!OwnsConditionDebug(Mb))
			return;
#endif
		if (Mb.Size != BlockSize)
			mAllocator.Deallocate(Mb);
		Node* lNewNode = static_cast<Node*>(Mb.Ptr);
		lNewNode->Next = mHead;
		mHead		   = lNewNode;
	}

	[[nodiscard]] bool Owns(MemoryBlock Mb) const
	{
#ifndef NDEBUG
		return OwnsConditionDebug(Mb) || mAllocator.Owns(Mb);
#else
		return Mb.Size == BlockSize || mAllocator.Owns(Mb);
#endif
	}

#ifndef NDEBUG
	[[nodiscard]] bool OwnsConditionDebug(MemoryBlock Mb) const
	{
		return *(reinterpret_cast<intptr_t*>(Mb.Ptr) - 1ull) == reinterpret_cast<intptr_t>(this);
	}
#endif
};

template<size_t ElementSize, typename TSupportAllocator>
class PoolAllocator
{
	static_assert(ElementSize >= sizeof(intptr_t), "ElementSize needs to be greater or equal sizeof(intptr_t).");
	struct FreeList
	{
		FreeList* Next{};
	};

	MemoryBlock		  mData{};
	uint64_t		  mCursor{};
	FreeList*		  mFreeList{};
	TSupportAllocator mAllocator{};

public:
	static constexpr size_t ALIGNMENT = RoundToAligned(ElementSize);

public:
	PoolAllocator(const uint64_t Capacity) : mData{mAllocator.Allocate(ElementSize * Capacity, ALIGNMENT)}
	{
	}

	~PoolAllocator()
	{
		mAllocator.Deallocate(mData);
	}

public:
	MemoryBlock Allocate(size_t, size_t)
	{
		void* lPtr;
		if (mFreeList)
		{
			lPtr	  = mFreeList;
			mFreeList = mFreeList->Next;
		}
		else
		{
			if ((mCursor + ElementSize) < mData.Size)
				return MemoryBlock{};
			lPtr = mData.Ptr + mCursor;
			mCursor += ElementSize;
		}
		return MemoryBlock{lPtr, ElementSize};
	}

	void Deallocate(MemoryBlock Mb)
	{
		FreeList* lNewNode = reinterpret_cast<FreeList*>(Mb.Ptr);
		lNewNode->Next	   = mFreeList;
		mFreeList		   = lNewNode;
	}
};

#define AFFIX_ALLOCATOR_FILE_PREFIX()                                                                                  \
	AffixAllocator_FilePrefix                                                                                          \
	{                                                                                                                  \
		__FILE__, __LINE__                                                                                             \
	}
#define AFFIX_ALLOCATOR_FILE_SUFFIX()                                                                                  \
	AffixAllocator_FileSuffix                                                                                          \
	{                                                                                                                  \
	}

struct AffixAllocator_FilePrefix
{
	char	Filename[64];
	int32_t LineNumber;
};

struct AffixAllocator_FileSuffix
{
};

template<typename TSupportAllocator, typename TPrefix, typename TSuffix>
class AffixAllocator
{
	TSupportAllocator mAllocator{};

	struct InternalPrefix
	{
		intptr_t AllocatorAddress;
		size_t	 AllocationSize;
		TPrefix	 Prefix;
	};
	struct InternalSuffix
	{
		intptr_t AllocatorAddress;
		TSuffix	 Suffix;
	};

public:
	MemoryBlock Allocate(size_t Size, InternalPrefix&& Prefix, InternalSuffix&& Suffix,
						 size_t Alignment = sizeof(std::max_align_t))
	{
		auto lMb = mAllocator.Allocate(sizeof(InternalPrefix) + sizeof(InternalSuffix) + Size, Alignment);
		Prefix.AllocatorAddress														= reinterpret_cast<intptr_t>(this);
		Prefix.AllocationSize														= Size;
		Suffix.AllocatorAddress														= reinterpret_cast<intptr_t>(this);
		*reinterpret_cast<InternalPrefix*>(lMb.Ptr)									= std::move(Prefix);
		*reinterpret_cast<InternalSuffix*>(lMb.Ptr + sizeof(InternalPrefix) + Size) = std::move(Suffix);
		return MemoryBlock{lMb.Ptr + sizeof(InternalPrefix), Size};
	}
#define AllocateDefaultPrefixSuffix(Size)                                                                              \
	Allocate(Size, AffixAllocator_FilePrefix{__FILE__, __LINE__}, AffixAllocator_FileSuffix{})
#define AllocateDefaultPrefixSuffixAligned(Size, Alignment)                                                            \
	Allocate(Size, AffixAllocator_FilePrefix{__FILE__, __LINE__}, AffixAllocator_FileSuffix{}, Alignment)

	[[nodiscard]] bool Owns(MemoryBlock Mb) const
	{
		auto lAddress = reinterpret_cast<intptr_t>(this);
		auto lData	  = Mb.Ptr - sizeof(InternalPrefix);
		auto lPrefix  = reinterpret_cast<InternalPrefix*>(lData);
		auto lSuffix  = reinterpret_cast<InternalSuffix*>(lData + sizeof(InternalSuffix) + lPrefix->AllocationSize);
		return lPrefix->AllocatorAddress == lAddress && lSuffix->AllocatorAddress == lAddress;
	}

	void Deallocate(MemoryBlock Mb)
	{
		if (!Owns(Mb))
		{
			return;
		}
		auto lData = Mb.Ptr - sizeof(InternalPrefix);
		mAllocator.Deallocate(MemoryBlock{lData, sizeof(InternalPrefix) + sizeof(InternalSuffix) + Mb.Size});
	}
};

#endif
