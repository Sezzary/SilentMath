#pragma once

namespace Silent::Utils
{
    class Bitfield;

    /** @brief Binary file data stream. */
    class Stream
    {
    private:
        // =======
        // Fields
        // =======

        std::fstream _stream = {};
        int          _flags  = 0;

    public:
        // =============
        // Constructors
        // =============

        /** @brief Constructs a binary file data `Stream` which can be read from or written to.
         *
         * @param filename Full file path.
         * @param read Allow reading from the file.
         * @param write Allow writing to the file.
         */
        Stream(const std::filesystem::path& filename, bool read, bool write);

        /** @brief Gracefully destroys the `Stream` and closes the file. */
        ~Stream();

        // ========
        // Getters
        // ========

        /** @brief Gets the size of the file data stream in bytes.
         *
         * @return Size in bytes.
         */
        int GetSize();

        /** @brief Gets the current position in the file.
         *
         * @return Position in bytes from the start of the file.
         */
        int GetPosition();

        // ========
        // Setters
        // ========

        /** @brief Sets the file pointer to a given position in the file.
         *
         * @param pos Position in bytes from the start of the file.
         */
        void SetPosition(int pos);

        // ==========
        // Inquirers
        // ==========

        /** @brief Checks if the file is open.
         *
         * @return `true` if the file is open, `false` otherwise.
         */
        bool IsOpen() const;

        /** @brief Checks if the end of the file has been reached.
         *
         * @return `true` if the end of the file has been reached, `false` otherwise.
         */
        bool IsEndOfFile() const;

        // ==========
        // Utilities
        // ==========

        /** @brief Closes the file. */
        void Close();

        /** @brief Skips the file pointer of the data stream ahead a given number of bytes.
         *
         * @param size Size in bytes to skip.
         */
        void Skip(int size);

        /** @brief Reads buffer data from the data stream and increments the file pointer.
         *
         * @param[out] buffer Output buffer.
         * @param size Buffer size in bytes.
         */
        void Read(void* buffer, int size);

        /** @brief Reads a boolean from the data stream and increments the file pointer.
         *
         * @return `bool` data.
         */
        bool ReadBool();

        /** @brief Reads a byte from the data stream and increments the file pointer.
         *
         * @return `byte` data.
         */
        byte ReadByte();

        /** @brief Reads an 8-bit integer from the data stream and increments the file pointer.
         *
         * @return `int8` data.
         */
        int8 ReadInt8();

        /** @brief Reads a 16-bit integer from the data stream and increments the file pointer.
         *
         * @return `int16` data.
         */
        int16 ReadInt16();

        /** @brief Reads a 32-bit integer from the data stream and increments the file pointer.
         *
         * @return `int32` data.
         */
        int32 ReadInt32();

        /** @brief Reads a 64-bit integer from the data stream and increments the file pointer.
         *
         * @return `int64` data.
         */
        int64 ReadInt64();

        /** @brief Reads an 8-bit unsigned integer from the data stream and increments the file pointer.
         *
         * @return `uint8` data.
         */
        uint8 ReadUint8();

        /** @brief Reads a 16-bit unsigned integer from the data stream and increments the file pointer.
         *
         * @return `uint16` data.
         */
        uint16 ReadUint16();

        /** @brief Reads a 32-bit unsigned integer from the data stream and increments the file pointer.
         *
         * @return `uint32` data.
         */
        uint32 ReadUint32();

        /** @brief Reads a 64-bit unsigned integer from the data stream and increments the file pointer.
         *
         * @return `uint64` data.
         */
        uint64 ReadUint64();

        /** @brief Reads a 32-bit float from the data stream and increments the file pointer.
         *
         * @return `float` data.
         */
        float ReadFloat();

        /** @brief Reads a length-prefixed string from the data stream and increments the file pointer.
         *
         * @return `std::string` data.
         */
        std::string ReadString();

        /** @brief Reads a null-terminated string from the data stream and increments the file pointer.
         *
         * @param size Optional fixed string size in bytes. If `NO_VALUE`, reads until a null character is encountered.
         * @return `std::string` data.
         */
        std::string ReadNullString(int size = NO_VALUE);

        /** @brief Reads a bitfield from the data stream and increments the file pointer.
         *
         * @return `Bitfield` data.
         */
        Bitfield ReadBitfield();

        /** @brief Reads an integer-based XY vector from the data stream and increments the file pointer.
         *
         * @return `Vector2i` data.
         */
        Vector2i ReadVector2i();

        /** @brief Reads a float-based XY vector from the data stream and increments the file pointer.
         *
         * @return `Vector2` data.
         */
        Vector2 ReadVector2();

        /** @brief Reads an integer-based XYZ vector from the data stream and increments the file pointer.
         *
         * @return `Vector3i` data.
         */
        Vector3i ReadVector3i();

        /** @brief Reads a float-based XYZ vector from the data stream and increments the file pointer.
         *
         * @return `Vector3` data.
         */
        Vector3 ReadVector3();

        /** @brief Reads the data stream as a parsed JSON and increments the file pointer to the end.
         *
         * @return `json` data.
         */
        json ReadJson();

        /** @brief Reads an array from the data stream and increments the file pointer.
         *
         * @tparam T Array data type.
         * @param[out] dest Destination container.
         */
        template <typename T>
        void ReadArray(std::span<T> dest);

        /** @brief Writes buffer data to the data stream and increments the file pointer.
         *
         * @param buffer Input buffer.
         * @param size Buffer size in bytes.
         */
        void Write(const void* buffer, int size);

        /** @brief Writes a boolean to the data stream and increments the file pointer.
         *
         * @param val `bool` data to write.
         */
        void WriteBool(bool val);

        /** @brief Writes a byte to the data stream and increments the file pointer.
         *
         * @param val `byte` data to write.
         */
        void WriteByte(byte val);

        /** @brief Writes a 16-bit integer to the data stream and increments the file pointer.
         *
         * @param val `int16` data to write.
         */
        void WriteInt16(int16 val);

        /** @brief Writes a 32-bit integer to the data stream and increments the file pointer.
         *
         * @param val `int32` data to write.
         */
        void WriteInt32(int32 val);

        /** @brief Writes a 64-bit integer to the data stream and increments the file pointer.
         *
         * @param val `int64` data to write.
         */
        void WriteInt64(int64 val);

        /** @brief Writes a 16-bit unsigned integer to the data stream and increments the file pointer.
         *
         * @param val `uint16` data to write.
         */
        void WriteUint16(uint16 val);

        /** @brief Writes a 32-bit unsigned integer to the data stream and increments the file pointer.
         *
         * @param val `uint32` data to write.
         */
        void WriteUint32(uint32 val);

        /** @brief Writes a 64-bit unsigned integer to the data stream and increments the file pointer.
         *
         * @param val `uint64` data to write.
         */
        void WriteUint64(uint64 val);

        /** @brief Writes a 32-bit float to the data stream and increments the file pointer.
         *
         * @param val `float` data to write.
         */
        void WriteFloat(float val);

        /** @brief Writes a string to the data stream and increments the file pointer.
         *
         * @param val `std::string` data to write.
         */
        void WriteString(const std::string& val);

        /** @brief Writes a bitfield to the data stream and increments the file pointer.
         *
         * @param val `Bitfield` data to write.
         */
        void WriteBitfield(const Bitfield& val);

        /** @brief Writes an integer-based XY vector to the data stream and increments the file pointer.
         *
         * @param val `Vector2i` data.
         */
        void WriteVector2i(const Vector2i& val);
        
        /** @brief Writes a float-based XY vector to the data stream and increments the file pointer.
         *
         * @param val `Vector2` data.
         */
        void WriteVector2(const Vector2& val);
        
        /** @brief Writes an integer-based XYZ vector to the data stream and increments the file pointer.
         *
         * @param val `Vector3i` data.
         */
        void WriteVector3i(const Vector3i& val);
        
        /** @brief Writes a float-based XYZ vector to the data stream and increments the file pointer.
         *
         * @param val `Vector3` data.
         */
        void WriteVector3(const Vector3& val);

        /** @brief Writes a JSON to the data stream and increments the file pointer.
         *
         * @param val `json` data.
         */
        void WriteJson(const json& val);
        
        /** @brief Writes an array to the data stream and increments the file pointer.
         *
         * @tparam T Array data type.
         * @param val Array data.
         */
        template <typename T>
        void WriteArray(std::span<const T> val);

    private:
        // ========
        // Helpers
        // ========

        /** @brief Checks if the data stream can be read from.
         *
         * @return `true` if the file can be read from, `false` otherwise.
         */
        bool TestRead() const;

        /** @brief Checks if the data stream can be written to.
         *
         * @return `true` if the file can be written to, `false` otherwise.
         */
        bool TestWrite() const;
    };

    template <typename T>
    void Stream::ReadArray(std::span<T> dest)
    {
        Read((byte*)dest.data(), dest.size() * sizeof(T));
    }

    template <typename T>
    void Stream::WriteArray(std::span<const T> val)
    {
        Write((byte*)val.data(), val.size() * sizeof(T));
    }
}
