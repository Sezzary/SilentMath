#pragma once

namespace Silent::Utils
{
    /** @brief Memory block metadata. */
    struct BlockMetadata
    {
        uint32 Offset = 0;
        uint32 Size   = 0;
        bool   IsFree = false;
    };

    /** @brief Dynamic memory block allocator for fixed-size buffer. */
    class BlockAllocator
    {
    private:
        // =======
        // Fields
        // =======

        uint32                     _size   = 0;
        std::vector<BlockMetadata> _blocks = {};

    public:
        // =============
        // Constructors
        // =============

        /** @brief Constructs a default uninitialized `BlockAllocator`. */
        BlockAllocator();

        /** @brief Constructs a `BlockAllocator` with a fixed size.
         *
         * @param size Memory size.
         */
        BlockAllocator(uint32 size);

        // ========
        // Getters
        // ========

        /** @brief Gets the available size of the memory pool.
         *
         * @return Size.
         */
        uint32 GetSize() const;

        // ==========
        // Utilities
        // ==========

        /** @brief Allocates a block of memory.
         *
         * @param size Block size.
         * @param alignment Block alignment.
         * @return Start offset of the allocated block. `NO_VALUE` if failed.
         */
        uint32 Allocate(uint32 size, uint32 alignment);

        /** @brief Deallocates a block of memory at a given offset.
         *
         * @param offset Start offset of the block of memory to deallocate.
         */
        void Deallocate(uint32 offset);

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
