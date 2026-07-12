#pragma once

namespace Silent::Utils
{
    /** @brief `std::vector`-based double buffer. */
    template <typename T>
    struct DoubleBuffer
    {
        std::vector<T> Active = {};
        std::vector<T> Render = {};

        void Swap(bool clearActive = true);
    };

    template <typename T>
    void DoubleBuffer<T>::Swap(bool clearActive)
    {
        std::swap(Render, Active);

        if (clearActive)
        {
            Active.clear();
        }
    }
}
