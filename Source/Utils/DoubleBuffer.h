#pragma once

namespace Silent::Utils
{
    /** @brief `std::vector`-based double buffer. */
    template <typename T>
    struct DoubleBuffer
    {
        std::vector<T> Active = {};
        std::vector<T> Render = {};

        void Swap();
    };

    template <typename T>
    void DoubleBuffer<T>::Swap()
    {
        std::swap(Render, Active);
        Active.clear();
    }
}
