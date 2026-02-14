#include "Framework.h"
#include "Utils/BlockAllocator.h"

namespace Silent::Utils
{
    BlockAllocator::BlockAllocator(int size)
    {
        size = std::max(size, 0);

        _size = size;
        _blocks.push_back(BlockMetadata
        {
            .Offset = 0,
            .Size   = size,
            .IsFree = true
        });
    }

    int BlockAllocator::GetSize() const
    {
        return _size;
    }

    int BlockAllocator::Allocate(int size, int alignment)
    {
        constexpr int SPLIT_SIZE_MIN = 3;

        size      = std::max(size, 0);
        alignment = std::max(alignment, 0);

        for (int i = 0; i < _blocks.size(); i++)
        {
            auto& block = _blocks[i];

            int alignedOffset = ((block.Offset + alignment) - 1) & ~(alignment - 1);
            int padding       = alignedOffset - block.Offset;

            if (block.IsFree && block.Size >= (size + padding))
            {
                // Split block if significant space is left over.
                if (block.Size > ((size + padding) + SPLIT_SIZE_MIN))
                {
                    auto nextBlock = BlockMetadata
                    {
                        .Offset = alignedOffset + size,
                        .Size   = block.Size - (size + padding),
                        .IsFree = true
                    };

                    _blocks.insert((_blocks.begin() + i) + 1, nextBlock);
                    block.Size = size + padding;
                }
                
                block.IsFree = false;
                return alignedOffset;
            }
        }

        // Out of memory.
        return NO_VALUE;
    }

    void BlockAllocator::Deallocate(int offset)
    {
        offset = std::max(offset, 0);

        for (int i = 0; i < _blocks.size(); i++)
        {
            auto& block = _blocks[i];

            // Set as free and merge free neighbors.
            if (block.Offset == offset)
            {
                block.IsFree = true;
                MergeNeighbors(i);
                break;
            }
        }
    }

    void BlockAllocator::Clear()
    {
        _blocks.clear();
        _blocks.push_back(BlockMetadata
        {
            .Offset = 0,
            .Size   = _size,
            .IsFree = true
        });
    }

    void BlockAllocator::MergeNeighbors(int idx)
    {
        // Check next block.
        if ((idx + 1) < _blocks.size() && _blocks[idx + 1].IsFree)
        {
            _blocks[idx].Size += _blocks[idx + 1].Size;
            _blocks.erase((_blocks.begin() + idx) + 1);
        }

        // Check previous block.
        if (idx > 0 && _blocks[idx - 1].IsFree)
        {
            _blocks[idx - 1].Size += _blocks[idx].Size;
            _blocks.erase(_blocks.begin() + idx);
        }
    }
}
