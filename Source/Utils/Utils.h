#pragma once

namespace Silent::Utils
{
    template <typename T>
    requires std::is_arithmetic_v<T>
    int GetSign(T val)
    {
        return (val >= 0) ? 1 : -1;
    }

    template <typename T>
    bool Contains(const std::span<T>& span, const T& element)
    {
        auto it = std::find(span.begin(), span.end(), element);
        return it != span.end();
    }

    template<typename T>
    std::span<const T> ToSpan(const T* rawArray, uint size)
    {
        return std::span<const T>(rawArray, size);
    }

    template<typename T>
    std::span<T> ToSpan(T* rawArray, uint size)
    {
        return std::span<T>(rawArray, size);
    }

    template <typename T, uint size>
    std::span<T> ToSpan(T(&cArray)[size])
    {
        return std::span<T>(cArray, size);
    }

    template<typename T>
    std::span<const typename T::value_type> ToSpan(const T& container)
    {
        return std::span<const typename T::value_type>(std::begin(container), std::end(container));
    }

    template<typename T>
    std::span<typename T::value_type> ToSpan(T& container)
    {
        return std::span<typename T::value_type>(std::begin(container), std::end(container));
    }
}
