#pragma once

namespace Silent::Utils
{
    class Bitfield
    {
    public:
        // Aliases

         using ChunkType = uint;

        private:
        // Constants

        static constexpr uint DEFAULT_SIZE = 32;
        static constexpr uint CHUNK_SIZE   = std::numeric_limits<ChunkType>::digits;

        // Fields

        uint                   _size   = 0;
        std::vector<ChunkType> _chunks = {};

    public:
        // Presets

        static const Bitfield Empty;
        static const Bitfield Default;

        // Constructors

        Bitfield();
        Bitfield(uint size);
        Bitfield(const std::initializer_list<bool>& bits);
        Bitfield(const std::vector<ChunkType>& bitChunks, uint size);
        Bitfield(const std::string& bitString);
        
        // Getters

        uint                          GetSize() const;
        uint                          GetCount() const;
        const std::vector<ChunkType>& GetChunks() const;

        // Setters

        void Set(uint bitIdx);
        void Set(const std::vector<uint>& bitIdxs);
        void SetAll();
        void Clear(uint bitIdx);
        void Clear(const std::vector<uint>& bitIdxs);
        void ClearAll();
        void Flip(uint bitIdx);
        void Flip(const std::vector<uint>& bitIdxs);
        void FlipAll();

        // Inquirers

        bool IsEmpty() const;

        bool Test(const std::vector<uint>& bitIdxs, bool testAny = true) const;
        bool Test(uint bitIdx) const;
        bool TestAny() const;
        bool TestAll() const;

        // Utilities

        void Resize(uint size);

        // Converters

        std::string ToString() const;

        // Operators

        bool      operator ==(const Bitfield& bitfield) const;
        bool      operator !=(const Bitfield& bitfield) const;
        Bitfield& operator =(const Bitfield& bitfield) = default;
        Bitfield& operator &=(const Bitfield& bitfield);
        Bitfield& operator |=(const Bitfield& bitfield);
        Bitfield& operator ^=(const Bitfield& bitfield);
        Bitfield  operator &(const Bitfield& bitfield) const;
        Bitfield  operator |(const Bitfield& bitfield) const;
        Bitfield  operator ^(const Bitfield& bitfield) const;
        Bitfield  operator ~() const;

    private:
        // Helpers

        void Fill(bool value);
        bool IsBitIdxValid(uint bitIdx) const;
    };
}

namespace std
{
    template<>
    struct hash<Silent::Utils::Bitfield>
    {
        size_t operator()(const Silent::Utils::Bitfield& Bitfield) const
        {
            size_t hashValue = 0;
            for (auto chunk : Bitfield.GetChunks())
                hashValue ^= hash<Silent::Utils::Bitfield::ChunkType>{}(chunk) + 0x9E3779B9 + (hashValue << 6) + (hashValue >> 2);

            return hashValue;
        }
    };
}
