#include "Framework.h"
#include "Utils/Bitfield.h"

namespace Silent::Utils
{
    const Bitfield Bitfield::Empty   = Bitfield(0);
    const Bitfield Bitfield::Default = Bitfield(DEFAULT_SIZE);

    Bitfield::Bitfield()
    {
        _chunks.resize((DEFAULT_SIZE + (CHUNK_SIZE - 1)) / CHUNK_SIZE);
        _size = DEFAULT_SIZE;
    }

    Bitfield::Bitfield(uint size)
    {
        _chunks.resize((size + (CHUNK_SIZE - 1)) / CHUNK_SIZE);
        _size = size;
    }

    Bitfield::Bitfield(const std::initializer_list<bool>& bits)
    {
        _chunks.resize((bits.size() + (CHUNK_SIZE - 1)) / CHUNK_SIZE);
        _size = (uint)bits.size();

        uint bitIdx = 0;
        for (auto it = bits.begin(); it != bits.end(); it++, bitIdx++)
        {
            uint localBitIdx = bitIdx % CHUNK_SIZE;
            uint chunkIdx = bitIdx / CHUNK_SIZE;

            bool bit = *it;
            if (bit)
            {
                _chunks[chunkIdx] |= (ChunkType)1 << localBitIdx;
            }
        }
    }

    Bitfield::Bitfield(const std::vector<ChunkType>& bitChunks, uint size)
    {
        _chunks = bitChunks;
        _size = std::min(size, (uint)bitChunks.size() * CHUNK_SIZE);
    }

    Bitfield::Bitfield(const std::string& bitString)
    {
        _chunks.resize((bitString.size() + (CHUNK_SIZE - 1)) / CHUNK_SIZE);
        _size = (uint)bitString.size();

        uint bitIdx = 0;
        for (char bit : bitString)
        {
            uint i = bitIdx % CHUNK_SIZE;
            uint chunkIdx = bitIdx / CHUNK_SIZE;

            if (bit == '1')
            {
                _chunks[chunkIdx] |= (ChunkType)1 << i;
            }

            bitIdx++;
        }
    }

    uint Bitfield::GetSize() const
    {
        return _size;
    }

    uint Bitfield::GetCount() const
    {
        uint count = 0;
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

    const std::vector<Bitfield::ChunkType>& Bitfield::GetChunks() const
    {
        return _chunks;
    }

    void Bitfield::Set(uint bitIdx)
    {
        if constexpr (IS_DEBUG)
        {
            if (!IsBitIdxValid(bitIdx))
            {
                return;
            }
        }

        uint localBitIdx = bitIdx % CHUNK_SIZE;
        uint chunkIdx = bitIdx / CHUNK_SIZE;
        _chunks[chunkIdx] |= (ChunkType)1 << localBitIdx;
    }

    void Bitfield::Set(const std::vector<uint>& bitIdxs)
    {
        for (int bitIdx : bitIdxs)
        {
            Set(bitIdx);
        }
    }

    void Bitfield::SetAll()
    {
        Fill(true);
    }

    void Bitfield::Clear(uint bitIdx)
    {
        if constexpr (IS_DEBUG)
        {
            if (!IsBitIdxValid(bitIdx))
            {
                return;
            }
        }

        uint localBitIdx = bitIdx % CHUNK_SIZE;
        uint chunkIdx = bitIdx / CHUNK_SIZE;
        _chunks[chunkIdx] &= ~((ChunkType)1 << localBitIdx);
    }

    void Bitfield::Clear(const std::vector<uint>& bitIdxs)
    {
        for (int bitIdx : bitIdxs)
        {
            Clear(bitIdx);
        }
    }

    void Bitfield::ClearAll()
    {
        Fill(false);
    }

    void Bitfield::Flip(uint bitIdx)
    {
        if constexpr (IS_DEBUG)
        {
            if (!IsBitIdxValid(bitIdx))
            {
                return;
            }
        }

        uint localBitIdx = bitIdx % CHUNK_SIZE;
        uint chunkIdx = bitIdx / CHUNK_SIZE;
        _chunks[chunkIdx] ^= (ChunkType)1 << localBitIdx;
    }

    void Bitfield::Flip(const std::vector<uint>& bitIdxs)
    {
        for (int bitIdx : bitIdxs)
        {
            Flip(bitIdx);
        }
    }

    void Bitfield::FlipAll()
    {
        for (int chunkIdx = 0; chunkIdx < _chunks.size(); chunkIdx++)
        {
            _chunks[chunkIdx] = ~_chunks[chunkIdx];
        }

        uint endBitCount = _size % CHUNK_SIZE;
        if (endBitCount > 0)
        {
            for (int localBitIdx = endBitCount; localBitIdx < CHUNK_SIZE; localBitIdx++)
            {
                _chunks.back() &= ~((ChunkType)1 << localBitIdx);
            }
        }
    }

    bool Bitfield::IsEmpty() const
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

    bool Bitfield::Test(uint bitIdx) const
    {
        if constexpr (IS_DEBUG)
        {
            if (!IsBitIdxValid(bitIdx))
            {
                return false;
            }
        }

        uint localBitIdx = bitIdx % CHUNK_SIZE;
        uint chunkIdx = bitIdx / CHUNK_SIZE;
        return (bool)(_chunks[chunkIdx] & ((ChunkType)1 << localBitIdx));
    }

    bool Bitfield::Test(const std::vector<uint>& bitIdxs, bool testAny) const
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

    bool Bitfield::TestAny() const
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

    bool Bitfield::TestAll() const
    {
        for (int chunkIdx = 0; chunkIdx < (_chunks.size() - 1); chunkIdx++)
        {
            if (_chunks[chunkIdx] != ~(ChunkType)0)
            {
                return false;
            }
        }

        uint endBitCount = _size % CHUNK_SIZE;
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

    void Bitfield::Resize(uint size)
    {
        _chunks.resize((size + (CHUNK_SIZE - 1)) / CHUNK_SIZE);
        _size = size;

        uint endBitCount = size % CHUNK_SIZE;
        if (endBitCount > 0)
        {
            for (int localBitID = endBitCount; localBitID < CHUNK_SIZE; localBitID++)
            {
                _chunks.back() &= ~((ChunkType)1 << localBitID);
            }
        }
    }

    std::string Bitfield::ToString() const
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

    bool Bitfield::operator ==(const Bitfield& bitfield) const
    {
        if (_size != bitfield.GetSize())
        {
            return false;
        }

        for (int chunkIdx = 0; chunkIdx < _chunks.size(); chunkIdx++)
        {
            if (_chunks[chunkIdx] != bitfield.GetChunks()[chunkIdx])
            {
                return false;
            }
        }

        return true;
    }

    bool Bitfield::operator !=(const Bitfield& bitfield) const
    {
        return !(*this == bitfield);
    }

    Bitfield& Bitfield::operator &=(const Bitfield& bitfield)
    {
        for (int chunkIdx = 0; chunkIdx < std::min(_chunks.size(), bitfield.GetChunks().size()); chunkIdx++)
        {
            _chunks[chunkIdx] &= bitfield.GetChunks()[chunkIdx];
        }

        _chunks.resize(std::min((uint)_chunks.size(), bitfield.GetSize()));
        _size = std::min(_size, bitfield.GetSize());
        return *this;
    }

    Bitfield& Bitfield::operator |=(const Bitfield& bitfield)
    {
        _chunks.resize(std::max(_chunks.size(), bitfield.GetChunks().size()));
        _size = std::max(_size, bitfield.GetSize());

        for (int chunkIdx = 0; chunkIdx < bitfield.GetChunks().size(); chunkIdx++)
        {
            _chunks[chunkIdx] |= bitfield.GetChunks()[chunkIdx];
        }

        return *this;
    }

    Bitfield& Bitfield::operator ^=(const Bitfield& bitfield)
    {
        _chunks.resize(std::max(_chunks.size(), bitfield.GetChunks().size()));
        _size = std::max(_size, bitfield.GetSize());

        for (int chunkIdx = 0; chunkIdx < std::min(_chunks.size(), bitfield.GetChunks().size()); chunkIdx++)
        {
            _chunks[chunkIdx] ^= bitfield.GetChunks()[chunkIdx];
        }

        return *this;
    }

    Bitfield Bitfield::operator &(const Bitfield& bitfield) const
    {
        auto chunks = std::vector<ChunkType>(std::min(_chunks.size(), bitfield.GetChunks().size()));

        for (int chunkIdx = 0; chunkIdx < chunks.size(); chunkIdx++)
        {
            chunks[chunkIdx] = _chunks[chunkIdx] & bitfield.GetChunks()[chunkIdx];
        }

        return Bitfield(chunks, std::min(_size, bitfield.GetSize()));
    }

    Bitfield Bitfield::operator |(const Bitfield& bitfield) const
    {
        auto chunks = std::vector<ChunkType>(std::max(_chunks.size(), bitfield.GetChunks().size()));

        for (int chunkIdx = 0; chunkIdx < _chunks.size(); chunkIdx++)
        {
            chunks[chunkIdx] |= _chunks[chunkIdx];
        }

        for (int chunkIdx = 0; chunkIdx < bitfield.GetChunks().size(); chunkIdx++)
        {
            chunks[chunkIdx] |= bitfield.GetChunks()[chunkIdx];
        }

        return Bitfield(chunks, std::max(_size, bitfield.GetSize()));
    }

    Bitfield Bitfield::operator ^(const Bitfield& bitfield) const
    {
        auto chunks = std::vector<ChunkType>(std::max(_chunks.size(), bitfield.GetChunks().size()));

        for (int chunkIdx = 0; chunkIdx < chunks.size(); chunkIdx++)
        {
            if (chunkIdx < _chunks.size() && chunkIdx < bitfield.GetChunks().size())
            {
                chunks[chunkIdx] = _chunks[chunkIdx] ^ bitfield.GetChunks()[chunkIdx];
            }
            else if (chunkIdx < _chunks.size())
            {
                chunks[chunkIdx] = _chunks[chunkIdx];
            }
            else if (chunkIdx < bitfield.GetChunks().size())
            {
                chunks[chunkIdx] = bitfield.GetChunks()[chunkIdx];
            }
        }

        return Bitfield(chunks, std::max(_size, bitfield.GetSize()));
    }

    Bitfield Bitfield::operator ~() const
    {
        auto newBitfield = *this;
        newBitfield.FlipAll();
        return newBitfield;
    }

    void Bitfield::Fill(bool value)
    {
        auto fillChunk = value ? ~(ChunkType)0 : (ChunkType)0;
        std::fill(_chunks.begin(), _chunks.end(), fillChunk);

        uint endBitCount = _size % CHUNK_SIZE;
        if (endBitCount > 0 && value)
        {
            _chunks.back() &= ((ChunkType)1 << endBitCount) - 1;
        }
    }

    bool Bitfield::IsBitIdxValid(uint bitIdx) const
    {
        if (bitIdx >= _size)
        {
            Log("Bitfield attempted to access bit at invalid index.", LogLevel::Warning);
            return false;
        }

        return true;
    }
}
