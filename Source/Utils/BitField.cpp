#include "Framework.h"
#include "Utils/BitField.h"

namespace Silent::Utils
{
    const BitField BitField::Empty	 = BitField(0);
    const BitField BitField::Default = BitField(DEFAULT_SIZE);

    BitField::BitField()
    {
        _chunks.resize((DEFAULT_SIZE + (CHUNK_SIZE - 1)) / CHUNK_SIZE);
        _size = DEFAULT_SIZE;
    }

    BitField::BitField(unsigned int size)
    {
        _chunks.resize((size + (CHUNK_SIZE - 1)) / CHUNK_SIZE);
        _size = size;
    }

    BitField::BitField(const std::initializer_list<bool>& bits)
    {
        _chunks.resize((bits.size() + (CHUNK_SIZE - 1)) / CHUNK_SIZE);
        _size = (unsigned int)bits.size();

        unsigned int bitIdx = 0;
        for (auto it = bits.begin(); it != bits.end(); it++, bitIdx++)
        {
            unsigned int localBitIdx = bitIdx % CHUNK_SIZE;
            unsigned int chunkIdx = bitIdx / CHUNK_SIZE;

            bool bit = *it;
            if (bit)
            {
                _chunks[chunkIdx] |= (ChunkType)1 << localBitIdx;
            }
        }
    }

    BitField::BitField(const std::vector<ChunkType>& bitChunks, unsigned int size)
    {
        _chunks = bitChunks;
        _size = std::min(size, (unsigned int)bitChunks.size() * CHUNK_SIZE);
    }

    BitField::BitField(const std::string& bitString)
    {
        _chunks.resize((bitString.size() + (CHUNK_SIZE - 1)) / CHUNK_SIZE);
        _size = (unsigned int)bitString.size();

        unsigned int bitIdx = 0;
        for (char bit : bitString)
        {
            unsigned int i = bitIdx % CHUNK_SIZE;
            unsigned int chunkIdx = bitIdx / CHUNK_SIZE;

            if (bit == '1')
            {
                _chunks[chunkIdx] |= (ChunkType)1 << i;
            }

            bitIdx++;
        }
    }

    unsigned int BitField::GetSize() const
    {
        return _size;
    }

    unsigned int BitField::GetCount() const
    {
        unsigned int count = 0;
        for (auto chunk : _chunks)
        {
            for (int localBitIdx = 0; localBitIdx < CHUNK_SIZE; localBitIdx++)
            {
                bool bit = bool(chunk & ((ChunkType)1 << localBitIdx));
                if (bit)
                {
                    count++;
                }
            }
        }

        return count;
    }

    const std::vector<BitField::ChunkType>& BitField::GetChunks() const
    {
        return _chunks;
    }

    void BitField::Set(unsigned int bitIdx)
    {
        if constexpr (IS_DEBUG_MODE)
        {
            if (!IsBitIdxValid(bitIdx))
                return;
        }

        unsigned int localBitIdx = bitIdx % CHUNK_SIZE;
        unsigned int chunkIdx = bitIdx / CHUNK_SIZE;
        _chunks[chunkIdx] |= (ChunkType)1 << localBitIdx;
    }

    void BitField::Set(const std::vector<unsigned int>& bitIdxs)
    {
        for (int bitIdx : bitIdxs)
        {
            Set(bitIdx);
        }
    }

    void BitField::SetAll()
    {
        Fill(true);
    }

    void BitField::Clear(unsigned int bitIdx)
    {
        if constexpr (IS_DEBUG_MODE)
        {
            if (!IsBitIdxValid(bitIdx))
                return;
        }

        unsigned int localBitIdx = bitIdx % CHUNK_SIZE;
        unsigned int chunkIdx = bitIdx / CHUNK_SIZE;
        _chunks[chunkIdx] &= ~((ChunkType)1 << localBitIdx);
    }

    void BitField::Clear(const std::vector<unsigned int>& bitIdxs)
    {
        for (int bitIdx : bitIdxs)
        {
            Clear(bitIdx);
        }
    }

    void BitField::ClearAll()
    {
        Fill(false);
    }

    void BitField::Flip(unsigned int bitIdx)
    {
        if constexpr (IS_DEBUG_MODE)
        {
            if (!IsBitIdxValid(bitIdx))
                return;
        }

        unsigned int localBitIdx = bitIdx % CHUNK_SIZE;
        unsigned int chunkIdx = bitIdx / CHUNK_SIZE;
        _chunks[chunkIdx] ^= (ChunkType)1 << localBitIdx;
    }

    void BitField::Flip(const std::vector<unsigned int>& bitIdxs)
    {
        for (int bitIdx : bitIdxs)
        {
            Flip(bitIdx);
        }
    }

    void BitField::FlipAll()
    {
        for (int chunkIdx = 0; chunkIdx < _chunks.size(); chunkIdx++)
        {
            _chunks[chunkIdx] = ~_chunks[chunkIdx];
        }

        unsigned int endBitCount = _size % CHUNK_SIZE;
        if (endBitCount > 0)
        {
            for (int localBitIdx = endBitCount; localBitIdx < CHUNK_SIZE; localBitIdx++)
            {
                _chunks.back() &= ~((ChunkType)1 << localBitIdx);
            }
        }
    }

    bool BitField::IsEmpty() const
    {
        if (_chunks.empty())
        {
            return true;
        }

        for (auto chunk : _chunks)
        {
            if (chunk != (ChunkType)0)
            {
                return false;
            }
        }

        return true;
    }

    bool BitField::Test(unsigned int bitIdx) const
    {
        if constexpr (IS_DEBUG_MODE)
        {
            if (!IsBitIdxValid(bitIdx))
                return;
        }

        unsigned int localBitIdx = bitIdx % CHUNK_SIZE;
        unsigned int chunkIdx = bitIdx / CHUNK_SIZE;
        return (bool)(_chunks[chunkIdx] & ((ChunkType)1 << localBitIdx));
    }

    bool BitField::Test(const std::vector<unsigned int>& bitIdxs, bool testAny) const
    {
        for (int bitIdx : bitIdxs)
        {
            if (testAny)
            {
                if (Test(bitIdx))
                {
                    return true;
                }
            }
            else // Test all.
            {
                if (!Test(bitIdx))
                {
                    return false;
                }
            }
        }

        return !testAny;
    }

    bool BitField::TestAny() const
    {
        for (auto chunk : _chunks)
        {
            if (chunk != 0)
            {
                return true;
            }
        }

        return false;
    }

    bool BitField::TestAll() const
    {
        for (int chunkIdx = 0; chunkIdx < (_chunks.size() - 1); chunkIdx++)
        {
            if (_chunks[chunkIdx] != (ChunkType)~0)
            {
                return false;
            }
        }

        unsigned int endBitCount = _size % CHUNK_SIZE;
        if (endBitCount > 0)
        {
            auto validMask = ((ChunkType)1 << endBitCount) - 1;
            if ((_chunks.back() & validMask) != validMask)
            {
                return false;
            }
        }

        return true;
    }

    void BitField::Resize(unsigned int size)
    {
        _chunks.resize((size + (CHUNK_SIZE - 1)) / CHUNK_SIZE);
        _size = size;

        unsigned int endBitCount = size % CHUNK_SIZE;
        if (endBitCount > 0)
        {
            for (int localBitID = endBitCount; localBitID < CHUNK_SIZE; localBitID++)
            {
                _chunks.back() &= ~((ChunkType)1 << localBitID);
            }
        }
    }

    std::string BitField::ToString() const
    {
        auto bitString = std::string();
        bitString.reserve(_size);

        for (int chunkIdx = 0; chunkIdx < _chunks.size(); chunkIdx++)
        {
            for (int localBitIdx = 0; localBitIdx < CHUNK_SIZE; localBitIdx++)
            {
                if (((chunkIdx * CHUNK_SIZE) + localBitIdx) >= _size)
                {
                    break;
                }

                bool bit = _chunks[chunkIdx] & ((ChunkType)1 << localBitIdx);
                bitString += bit ? '1' : '0';
            }
        }

        return bitString;
    }

    bool BitField::operator ==(const BitField& bitField) const
    {
        if (_size != bitField.GetSize())
        {
            return false;
        }

        for (int i = 0; i < _chunks.size(); i++)
        {
            if (_chunks[i] != bitField.GetChunks()[i])
            {
                return false;
            }
        }

        return true;
    }

    bool BitField::operator !=(const BitField& bitField) const
    {
        return !(*this == bitField);
    }

    BitField& BitField::operator &=(const BitField& bitField)
    {
        for (int i = 0; i < std::min(_chunks.size(), bitField.GetChunks().size()); i++)
        {
            _chunks[i] &= bitField.GetChunks()[i];
        }

        _chunks.resize(std::min((unsigned int)_chunks.size(), bitField.GetSize()));
        _size = std::min(_size, bitField.GetSize());
        return *this;
    }

    BitField& BitField::operator |=(const BitField& bitField)
    {
        _chunks.resize(std::max(_chunks.size(), bitField.GetChunks().size()));
        _size = std::max(_size, bitField.GetSize());

        for (int i = 0; i < bitField.GetChunks().size(); i++)
        {
            _chunks[i] |= bitField.GetChunks()[i];
        }

        return *this;
    }

    BitField& BitField::operator ^=(const BitField& bitField)
    {
        _chunks.resize(std::max(_chunks.size(), bitField.GetChunks().size()));
        _size = std::max(_size, bitField.GetSize());

        for (int i = 0; i < std::min(_chunks.size(), bitField.GetChunks().size()); i++)
        {
            _chunks[i] ^= bitField.GetChunks()[i];
        }

        return *this;
    }

    BitField BitField::operator &(const BitField& bitField) const
    {
        auto chunks = std::vector<ChunkType>(std::min(_chunks.size(), bitField.GetChunks().size()));

        for (int i = 0; i < chunks.size(); i++)
        {
            chunks[i] = _chunks[i] & bitField.GetChunks()[i];
        }

        return BitField(chunks, std::min(_size, bitField.GetSize()));
    }

    BitField BitField::operator |(const BitField& bitField) const
    {
        auto chunks = std::vector<ChunkType>(std::max(_chunks.size(), bitField.GetChunks().size()));

        for (int i = 0; i < _chunks.size(); i++)
        {
            chunks[i] |= _chunks[i];
        }

        for (int i = 0; i < bitField.GetChunks().size(); i++)
        {
            chunks[i] |= bitField.GetChunks()[i];
        }

        return BitField(chunks, std::max(_size, bitField.GetSize()));
    }

    BitField BitField::operator ^(const BitField& bitField) const
    {
        auto chunks = std::vector<ChunkType>(std::max(_chunks.size(), bitField.GetChunks().size()));

        for (int i = 0; i < chunks.size(); i++)
        {
            if (i < _chunks.size() && i < bitField.GetChunks().size())
            {
                chunks[i] = _chunks[i] ^ bitField.GetChunks()[i];
            }
            else if (i < _chunks.size())
            {
                chunks[i] = _chunks[i];
            }
            else if (i < bitField.GetChunks().size())
            {
                chunks[i] = bitField.GetChunks()[i];
            }
        }

        return BitField(chunks, std::max(_size, bitField.GetSize()));
    }

    BitField BitField::operator ~() const
    {
        auto newBitField = *this;
        newBitField.FlipAll();
        return newBitField;
    }

    void BitField::Fill(bool value)
    {
        auto fillChunk = value ? ~(ChunkType)0 : (ChunkType)0;
        std::fill(_chunks.begin(), _chunks.end(), fillChunk);

        unsigned int endBitCount = _size % CHUNK_SIZE;
        if (endBitCount > 0 && value)
        {
            _chunks.back() &= ((ChunkType)1 << endBitCount) - 1;
        }
    }

    bool BitField::IsBitIdxValid(unsigned int bitIdx) const
    {
        if (bitIdx >= _size)
        {
            Log("BitField attempted to access bit at invalid index.", LogLevel::Warning);
            return false;
        }

        return true;
    }
}
