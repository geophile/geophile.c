#ifndef _BYTE_BUFFER_H
#define _BYTE_BUFFER_H

#include <stdint.h>

typedef uint8_t byte;

namespace geophile
{
    class ByteBuffer
    {
    public:
        int32_t getInt32();
        int64_t getInt64();
        uint32_t getUint32();
        uint64_t getUint64();
        double getDouble();
        void getBytes(byte* target, uint32_t count);
        ByteBuffer& putInt32(int32_t source);
        ByteBuffer& putInt64(int64_t source);
        ByteBuffer& putUint32(uint32_t source);
        ByteBuffer& putUint64(uint64_t source);
        ByteBuffer& putDouble(double source);
        ByteBuffer& putBytes(byte* source, uint32_t count);
        uint32_t capacity() const;
        uint32_t limit() const;
        uint32_t position() const;
        uint32_t remaining() const;
        ByteBuffer& clear();
        ByteBuffer& flip();
        ByteBuffer& limit(uint32_t limit);
        ByteBuffer& position(uint32_t position);
        ByteBuffer& reset();
        ByteBuffer& rewind();
        ByteBuffer& mark();
        void copyTo(byte* target);

    public:
        ~ByteBuffer();
        ByteBuffer(byte* bytes, uint32_t length);

    private:
        void checkState() const;
        void maintainMark();

    public:
        static const uint32_t MAX_SIZE = 0x7fffffff;

    private:
        byte* const _start;
        byte* const _end;
        byte* _position;
        byte* _limit;
        byte* _mark;
    };
}

#endif
