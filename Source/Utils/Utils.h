#pragma once

namespace Silent::Utils
{
    template<typename T>
    std::span<T> ToSpan(T* array, size_t size)
    {
        return std::span<T>(array, size);
    }

    template <typename T, size_t size>
    std::span<T> ToSpan(T (&array)[size])
    {
        return std::span<T>(array, size);
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
    std::span<const T> ToSpan(const std::vector<T>& vec)
    {
        return std::span<const T>(vec.data(), vec.size());
    }

    template<typename T>
    std::span<T> ToSpan(std::vector<T>& vec)
    {
        return std::span<T>(vec.data(), vec.size());
    }
}
