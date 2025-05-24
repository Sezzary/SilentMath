#pragma once

namespace Silent::Utils
{
    char* CopyString(const char src[], uint size);

    template <typename T>
    requires std::is_arithmetic_v<T>
    int GetSign(T val)
    {
        return (val >= 0) ? 1 : -1;
    }

    template <typename Container, typename T>
    bool Contains(const Container& container, const T& element)
    {
        auto it = std::find(container.begin(), container.end(), element);
        return it != container.end();
    }

    template <typename Container, typename Predicate>
    void EraseIf(Container& container, Predicate pred)
    {
        container.erase(std::remove_if(container.begin(), container.end(), pred),
                        container.end());
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
