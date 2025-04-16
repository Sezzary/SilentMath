#pragma once

namespace Silent::Utils
{
	template <typename T>
	bool Contains(const std::span<T>& span, const T& element)
	{
		auto it = std::find(span.begin(), span.end(), element);
		return it != span.end();
	}

    template<typename T>
    std::span<T> ToSpan(T* rawArray, size_t size)
    {
        return std::span<T>(rawArray, size);
    }

    template <typename T, size_t size>
    std::span<T> ToSpan(T (&cArray)[size])
    {
        return std::span<T>(cArray, size);
    }

    template<typename T, size_t size>
    std::span<const T> ToSpan(const std::array<T, size>& array)
    {
        return std::span<const T>(array.data(), array.size());
    }

    template<typename T, size_t size>
    std::span<T> ToSpan(std::array<T, size>& array)
    {
        return std::span<T>(array.data(), array.size());
    }

    template<typename T>
    std::span<const T> ToSpan(const std::vector<T>& vector)
    {
        return std::span<const T>(vector.data(), vector.size());
    }

    template<typename T>
    std::span<T> ToSpan(std::vector<T>& vector)
    {
        return std::span<T>(vector.data(), vector.size());
    }
}
