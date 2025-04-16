#pragma once

namespace Silent::Utils
{
    int CopySign(int value, int sign);

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
    std::span<T> ToSpan(T (&cArray)[size])
    {
        return std::span<T>(cArray, size);
    }

    template<typename T, uint size>
    std::span<const T> ToSpan(const std::array<T, size>& array)
    {
        return std::span<const T>(array.data(), array.size());
    }

    template<typename T, uint size>
    std::span<T> ToSpan(std::array<T, size>& array)
    {
        return std::span<T>(array.data(), array.size());
    }

    template<typename T>
    std::span<const T> ToSpan(const std::set<T>& set)
    {
        return std::span<const T>(set.data(), set.size());
    }

    template<typename T>
    std::span<T> ToSpan(std::set<T>& set)
    {
        return std::span<T>(set.data(), set.size());
    }

    template<typename T>
    std::span<const T> ToSpan(const std::stack<T>& stack)
    {
        return std::span<const T>(stack.data(), stack.size());
    }

    template<typename T>
    std::span<T> ToSpan(std::stack<T>& stack)
    {
        return std::span<T>(stack.data(), stack.size());
    }
    
    template<typename T>
    std::span<const T> ToSpan(const std::queue<T>& queue)
    {
        return std::span<const T>(queue.data(), queue.size());
    }

    template<typename T>
    std::span<T> ToSpan(std::queue<T>& queue)
    {
        return std::span<T>(queue.data(), queue.size());
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
