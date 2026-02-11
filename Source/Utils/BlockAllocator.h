#pragma once

namespace Silent::Utils
{
    /** @brief Memory block metadata. */
    struct BlockMetadata
    {
        int  Offset = 0;
        int  Size   = 0;
        bool IsFree = false;
    };

    /** @brief Dynamic memory block allocator. Used to track allocations to a fixed-size mega-buffer. */
    class BlockAllocator
    {
    private:
        // =======
        // Fields
        // =======

        int                        _size   = 0;
        std::vector<BlockMetadata> _blocks = {};

    public:
        // =============
        // Constructors
        // =============

        /** @brief Creates a default uninitialized instance. */
        BlockAllocator() = default;

        /** @brief Creates an instance with a fixed size.
         *
         * @param size Memory size.
         */
        BlockAllocator(int size);

        // ========
        // Getters
        // ========

        /** @brief Gets the available size of the memory pool.
         *
         * @return Size.
         */
        int GetSize() const;

        // ==========
        // Utilities
        // ==========

        /** @brief Allocates a memory block.
         *
         * @param size Block size.
         * @param alignment Block alignment.
         * @return Start offset of the allocated block. `NO_VALUE` if failed.
         */
        int Allocate(int size, int alignment = 1);

        /** @brief Deallocates a memory block at a given offset.
         *
         * @param offset Start offset of the memory block to deallocate.
         */
        void Deallocate(int offset);

        /** @brief Deallocates all memory blocks. */
        void Clear();

    private:
        // ========
        // Helpers
        // ========

        /** @brief Merges the free neighbors of a memory block.
         *
         * @param idx Index of the memory block to merge.
         */
        void MergeNeighbors(int idx);
    };
}
