#pragma once

namespace Silent::Utils
{
    /** @brief Double-buffered arbitrary data. */
    template <typename T>
    struct DoubleBuffer
    {
        T Back  = {};
        T Front = {};

        void Swap();
    };

    template <typename T>
    void DoubleBuffer<T>::Swap()
    {
        std::swap(Back, Front);
    }
}
