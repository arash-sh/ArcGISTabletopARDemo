// COPYRIGHT 1995-2022 ESRI
// TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
// Unpublished material - all rights reserved under the
// Copyright Laws of the United States and applicable international
// laws, treaties, and conventions.
//
// For additional information, contact:
// Environmental Systems Research Institute, Inc.
// Attn: Contracts and Legal Services Department
// 380 New York Street
// Redlands, California, 92373
// USA
//
// email: contracts@esri.com

#pragma once

namespace Esri
{
namespace Unreal
{
class ArcGISDataBufferBase
{
public:
	void* GetData() const;

	size_t GetItemCount() const;

	size_t GetItemSize() const;

	size_t GetSizeBytes() const;

public:
	ArcGISDataBufferBase() = default;
	explicit ArcGISDataBufferBase(void* handle);
	ArcGISDataBufferBase(const ArcGISDataBufferBase&) = delete;
	ArcGISDataBufferBase(ArcGISDataBufferBase&& other) noexcept;
	~ArcGISDataBufferBase();

	ArcGISDataBufferBase& operator=(const ArcGISDataBufferBase&) = delete;
	ArcGISDataBufferBase& operator=(ArcGISDataBufferBase&& other) noexcept;

	explicit operator bool() const noexcept;

private:
	void* m_handle{nullptr};
};

template <typename T>
class ArcGISDataBuffer
{
public:
	class ConstIterator
	{
	public:
		ConstIterator(T* pointer) : pointer(pointer)
		{
		}

		ConstIterator& operator++()
		{
			++pointer;

			return *this;
		}

		bool operator!=(const ConstIterator& rhs) const
		{
			return pointer != rhs.pointer;
		}

		const T& operator*() const
		{
			return *pointer;
		}

	private:
		T* pointer;
	};

	class Iterator
	{
	public:
		Iterator(T* pointer) : pointer(pointer)
		{
		}

		Iterator& operator++()
		{
			++pointer;

			return *this;
		}

		bool operator!=(const Iterator& rhs) const
		{
			return pointer != rhs.pointer;
		}

		T& operator*()
		{
			return *pointer;
		}

	private:
		T* pointer;
	};

public:
	void* GetData() const
	{
		return dataBufferBase.GetData();
	}

	template <typename U>
	U* GetData() const
	{
		return (U*)dataBufferBase.GetData();
	}

	size_t GetItemCount() const
	{
		return dataBufferBase.GetItemCount();
	}

	size_t GetItemSize() const
	{
		return dataBufferBase.GetItemSize();
	}

	size_t GetSizeBytes() const
	{
		return dataBufferBase.GetSizeBytes();
	}

	std::vector<T> ToVector() const
	{
		return std::vector<T>((T*)GetData(), (T*)GetData() + GetItemCount());
	}

	template <typename U>
	std::vector<U> ToVector() const
	{
		return std::vector<U>((U*)GetData(), (U*)GetData() + (GetSizeBytes() / sizeof(U)));
	}

	Iterator begin()
	{
		return Iterator((T*)GetData());
	}

	ConstIterator begin() const
	{
		return ConstIterator((T*)GetData());
	}

	Iterator end()
	{
		return Iterator((T*)GetData() + GetItemCount());
	}

	ConstIterator end() const
	{
		return ConstIterator((T*)GetData() + GetItemCount());
	}

public:
	ArcGISDataBuffer() = default;

	explicit ArcGISDataBuffer(void* handle) : dataBufferBase(handle)
	{
	}

	ArcGISDataBuffer(const ArcGISDataBuffer<T>&) = delete;

	ArcGISDataBuffer(ArcGISDataBuffer<T>&& other) noexcept
	{
		using std::swap;

		swap(dataBufferBase, other.dataBufferBase);
	}

	~ArcGISDataBuffer()
	{
#if PLATFORM_COMPILER_HAS_IF_CONSTEXPR
		if constexpr (!std::is_trivially_destructible<T>::value)
#else
		if (!std::is_trivially_destructible<T>::value)
#endif
		{
			if (dataBufferBase)
			{
				auto count = GetItemCount();
				auto element = (T*)GetData();

				while (count)
				{
					element->~T();

					++element;
					--count;
				}
			}
		}
	}

	ArcGISDataBuffer<T>& operator=(const ArcGISDataBuffer<T>&) = delete;

	ArcGISDataBuffer<T>& operator=(ArcGISDataBuffer<T>&& other) noexcept
	{
		if (this != &other)
		{
			using std::swap;

			swap(dataBufferBase, other.dataBufferBase);
		}

		return *this;
	}

	explicit operator bool() const noexcept
	{
		return (bool)dataBufferBase;
	}

private:
	ArcGISDataBufferBase dataBufferBase;
};
} // namespace Unreal
} // namespace Esri
