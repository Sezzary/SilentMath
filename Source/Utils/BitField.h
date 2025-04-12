#pragma once

namespace Utils
{
    class BitField
    {
    private:
        // Aliases

        using ChunkType = unsigned int;

        // Constants

        static constexpr auto DEFAULT_SIZE = 32;
        static constexpr auto CHUNK_SIZE   = std::numeric_limits<ChunkType>::digits;

        // Fields

        unsigned int           _size   = 0;
        std::vector<ChunkType> _chunks = {};

    public:
        // Presets

        static const BitField Empty;
        static const BitField Default;

        // Constructors

        BitField();
        BitField(unsigned int size);
        BitField(const std::initializer_list<bool>& bits);
        BitField(const std::vector<ChunkType>& bitChunks, unsigned int size);
        BitField(const std::string& bitString);
        
        // Getters

        unsigned int                  GetSize() const;
        unsigned int                  GetCount() const;
        const std::vector<ChunkType>& GetChunks() const;

        // Setters

        void Set(unsigned int bitIdx);
        void Set(const std::vector<unsigned int>& bitIdxs);
        void SetAll();
        void Clear(unsigned int bitIdx);
        void Clear(const std::vector<unsigned int>& bitIdxs);
        void ClearAll();
        void Flip(unsigned int bitIdx);
        void Flip(const std::vector<unsigned int>& bitIdxs);
        void FlipAll();

        // Inquirers

        bool IsEmpty() const;

        bool Test(const std::vector<unsigned int>& bitIdxs, bool testAny = true) const;
        bool Test(unsigned int bitIdx) const;
        bool TestAny() const;
        bool TestAll() const;

        // Utilities

        void Resize(unsigned int size);

        // Converters

        std::string ToString() const;
        ChunkType	ToPackedBits() const;

        // Operators

        bool      operator ==(const BitField& bitField) const;
        bool      operator !=(const BitField& bitField) const;
        BitField& operator =(const BitField& bitField) = default;
        BitField& operator &=(const BitField& bitField);
        BitField& operator |=(const BitField& bitField);
        BitField& operator ^=(const BitField& bitField);
        BitField  operator &(const BitField& bitField) const;
        BitField  operator |(const BitField& bitField) const;
        BitField  operator ^(const BitField& bitField) const;
        BitField  operator ~() const;

        // Compatibility operators

        bool         operator ==(unsigned int packedBits) const;
        bool         operator !=(unsigned int packedBits) const;
        BitField&    operator =(unsigned int packedBits);
        BitField&    operator &=(unsigned int packedBits);
        BitField&    operator |=(unsigned int packedBits);
        unsigned int operator &(unsigned int packedBits) const;
        unsigned int operator |(unsigned int packedBits) const;

    private:
        // Helpers

        void Fill(bool value);
        bool IsBitIdxValid(unsigned int bitIdx) const;
    };
}
