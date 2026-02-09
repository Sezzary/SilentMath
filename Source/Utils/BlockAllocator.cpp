#include "Framework.h"
#include "Utils/BlockAllocator.h"

namespace Silent::Utils
{
    BlockAllocator::BlockAllocator(uint32 size)
    {
        _size = size;
        _blocks.push_back(BlockMetadata
        {
            .Offset = 0,
            .Size   = size,
            .IsFree = true
        });
    }

    uint32 BlockAllocator::GetSize() const
    {
        return _size;
    }

    uint32 BlockAllocator::Allocate(uint32 offset, uint32 alignment)
    {
        constexpr int SPLIT_SIZE_MIN = 64;

        for (auto& block : _blocks)
        {
            uint32 alignedOffset = ((block.Offset + alignment) - 1) & ~(alignment - 1);
            uint32 padding       = alignedOffset - block.Offset;

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

    void BlockAllocator::Deallocate(uint32 offset)
    {
        for (int i = 0; i < _blocks.size(); i++)
        {
            auto& block = _blocks[i];

            if (block.Offset == offset)
            {
                block.IsFree = true;
                MergeNeighbors(i);
                break;
            }
        }
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
