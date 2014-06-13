#include <assert.h>
#include <stdio.h>

#include "geophile/Space.h"
#include "geophile/Point2.h"
#include "geophile/Box2.h"
#include "geophile/Z.h"
#include "geophile/ZArray.h"
#include "geophile/ByteBuffer.h"
#include "geophile/ByteBufferOverflowException.h"
#include "geophile/ByteBufferUnderflowException.h"
#include "geophile/SessionMemory.h"
#include "geophile/OutputArray.h"

#include "RecordArray.h"
#include "TestSpatialObject.h"

using namespace geophile;

//----------------------------------------------------------------------

// Testing

#define ASSERT_EQ(x, y) assert((x) == (y))

#define ASSERT_TRUE(x) assert(x)

//----------------------------------------------------------------------

// Utility functions

static int64_t unsigned_right_shift(int64_t z)
{
    return (z >> 1) & 0x7fffffffffffffffL;
}

static Z zvalue(int64_t left_justified_bits, uint32_t length)
{
    return Z(unsigned_right_shift(left_justified_bits), length);
}

static int64_t prefix(int64_t x, uint32_t bits)
{
    return x & (((1L << bits) - 1) << (64 - bits));
}

static int64_t mask(uint32_t position)
{
    return 1L << (63 - position);
}

//----------------------------------------------------------------------

// Space

static void zSize()
{
    ASSERT_EQ(sizeof(int64_t), sizeof(Z));
}

static void tooFewDimensions()
{
    try {
        double lo[] = {};
        double hi[] = {};
        uint32_t x_bits[] = {};
        Space space(0, lo, hi, x_bits);
        ASSERT_TRUE(false);
    } catch (const GeophileException& e) {
        // expected
    }
}

static void tooManyDimensions()
{
    try {
        double lo[] = {0.0, 0.0, 0.0, 0.0, 0.0, 
                       0.0, 0.0, 0.0, 0.0, 0.0};
        double hi[] = {1000.0, 1000.0, 1000.0, 1000.0, 1000.0, 
                       1000.0, 1000.0, 1000.0, 1000.0, 1000.0};
        uint32_t x_bits[] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
        ASSERT_TRUE(10 > Space::MAX_DIMENSIONS);
        Space space(10, lo, hi, x_bits);
        ASSERT_TRUE(false);
    } catch (const GeophileException& e) {
        // expected
    }
}

static void createAndDestroySpace()
{
    double lo[] = {0.0, 0.0};
    double hi[] = {1000000.0, 1000000.0};
    uint32_t x_bits[] = {20, 20};
    Space space(2, lo, hi, x_bits);
}

static void testSpace()
{
    zSize();
    tooFewDimensions();
    tooManyDimensions();
    createAndDestroySpace();
}

//----------------------------------------------------------------------

// Interleave

static void checkInterleave(const Space& space, int64_t expected, uint64_t x)
{
    const uint64_t coords[] = {x};
    ASSERT_EQ(zvalue(expected, space.zBits()), space.shuffle(coords));
}

static void checkInterleave(const Space& space, int64_t expected, uint64_t x, uint64_t y)
{
    const uint64_t coords[] = {x, y};
    ASSERT_EQ(zvalue(expected, space.zBits()), space.shuffle(coords));
}

static void checkInterleave(const Space& space, 
                            int64_t expected, 
                            uint64_t x, 
                            uint64_t y, 
                            uint64_t z)
{
    const uint64_t coords[] = {x, y, z};
    ASSERT_EQ(zvalue(expected, space.zBits()), space.shuffle(coords));
}

static void defaultInterleave()
{
    double lo[] = {0.0, 0.0};
    double hi[] = {1000.0, 1000.0};
    uint32_t x_bits[] = {3, 5};
    Space space(2, lo, hi, x_bits);
    const uint32_t* interleave = space.interleave();
    ASSERT_EQ(0, interleave[0]);
    ASSERT_EQ(1, interleave[1]);
    ASSERT_EQ(0, interleave[2]);
    ASSERT_EQ(1, interleave[3]);
    ASSERT_EQ(0, interleave[4]);
    ASSERT_EQ(1, interleave[5]);
    ASSERT_EQ(1, interleave[6]);
    ASSERT_EQ(1, interleave[7]);
}

static void space10()
{
    double lo[] = {0.0};
    double hi[] = {1000.0};
    uint32_t x_bits[] = {10};
    Space space(1, lo, hi, x_bits);
    checkInterleave(space, 0x0000000000000000L, 0x000);
    checkInterleave(space, 0xffc0000000000000L, 0x3ff);
    checkInterleave(space, 0xaa80000000000000L, 0x2aa);
    checkInterleave(space, 0x5540000000000000L, 0x155);
}

static void space10x10()
{
    double lo[] = {0.0, 0.0};
    double hi[] = {1000.0, 1000.0};
    uint32_t x_bits[] = {10, 10};
    Space space(2, lo, hi, x_bits);
    checkInterleave(space, 0x0000000000000000L, 0x000, 0x000);
    checkInterleave(space, 0x5555500000000000L, 0x000, 0x3ff);
    checkInterleave(space, 0xaaaaa00000000000L, 0x3ff, 0x000);
    checkInterleave(space, 0xfffff00000000000L, 0x3ff, 0x3ff);
    checkInterleave(space, 0x1111100000000000L, 0x000, 0x155);
    checkInterleave(space, 0x8888800000000000L, 0x2aa, 0x000);
    checkInterleave(space, 0x9999900000000000L, 0x2aa, 0x155);
}

static void space10x12()
{
    double lo[] = {0.0, 0.0};
    double hi[] = {4000.0, 4000.0};
    uint32_t x_bits[] = {10, 12};
    Space space(2, lo, hi, x_bits);
    checkInterleave(space, 0x0000000000000000L, 0x000, 0x000);
    checkInterleave(space, 0x55555c0000000000L, 0x000, 0xfff);
    checkInterleave(space, 0xaaaaa00000000000L, 0x3ff, 0x000);
    checkInterleave(space, 0xfffffc0000000000L, 0x3ff, 0xfff);
    checkInterleave(space, 0x1111140000000000L, 0x000, 0x555);
    checkInterleave(space, 0x8888800000000000L, 0x2aa, 0x000);
    checkInterleave(space, 0x9999940000000000L, 0x2aa, 0x555);
}

static void space10x10x10()
{
    double lo[] = {0.0, 0.0, 0.0};
    double hi[] = {1000.0, 1000.0, 1000.0};
    uint32_t x_bits[] = {10, 10, 10};
    Space space(3, lo, hi, x_bits);
    checkInterleave(space, 0x0000000000000000L, 0x000, 0x000, 0x000);
    checkInterleave(space, 0x2492492400000000L, 0x000, 0x000, 0x3ff);
    checkInterleave(space, 0x4924924800000000L, 0x000, 0x3ff, 0x000);
    checkInterleave(space, 0x6db6db6c00000000L, 0x000, 0x3ff, 0x3ff);
    checkInterleave(space, 0x9249249000000000L, 0x3ff, 0x000, 0x000);
    checkInterleave(space, 0xb6db6db400000000L, 0x3ff, 0x000, 0x3ff);
    checkInterleave(space, 0xdb6db6d800000000L, 0x3ff, 0x3ff, 0x000);
    checkInterleave(space, 0xfffffffc00000000L, 0x3ff, 0x3ff, 0x3ff);
    checkInterleave(space, 0x0400400400000000L, 0x000, 0x000, 0x111);
    checkInterleave(space, 0x4004004000000000L, 0x000, 0x222, 0x000);
    checkInterleave(space, 0x4404404400000000L, 0x000, 0x222, 0x111);
    checkInterleave(space, 0x9009009000000000L, 0x333, 0x000, 0x000);
    checkInterleave(space, 0x9409409400000000L, 0x333, 0x000, 0x111);
    checkInterleave(space, 0xd00d00d000000000L, 0x333, 0x222, 0x000);
    checkInterleave(space, 0xd40d40d400000000L, 0x333, 0x222, 0x111);
}

static void testInterleave()
{
    defaultInterleave();
    space10();
    space10x10();
    space10x12();
    space10x10x10();
}

//----------------------------------------------------------------------

// Z-values

static void siblings()
{
    int64_t Z_TEST = 0xaaaaaaaaaaaaaa80L;
    // Root not sibling of itself
    ASSERT_TRUE(!zvalue(0, 0).siblingOf(zvalue(0, 0)));
    // ... or of something it contains
    for (uint32_t bits = 0; bits < Z::MAX_Z_BITS; bits++) {
        int64_t base = prefix(Z_TEST, bits);
        int64_t sibling = base ^ (1L << (64 - bits));
        // z-value isn't its own sibling
        ASSERT_TRUE(!zvalue(base, bits).siblingOf(zvalue(base, bits)));
        if (bits > 0) {
            // z-value is its sibling's sibling
            ASSERT_TRUE(zvalue(base, bits).siblingOf(zvalue(sibling, bits)));
            // siblings have to be at the same level
            ASSERT_TRUE(!zvalue(base, bits).siblingOf(zvalue(sibling, bits + 1)));
            // Flip an ancestor bit in sibling
            int64_t not_sibling = sibling ^ mask(bits / 2);
            ASSERT_TRUE(!zvalue(base, bits).siblingOf(zvalue(not_sibling, bits)));
        }
    }
}

static void parent()
{
    int64_t Z_TEST = 0xaaaaaaaaaaaaaa80L;
    for (uint32_t bits = 1; bits <= Z::MAX_Z_BITS; bits++) {
        Z z = zvalue(prefix(Z_TEST, bits), bits);
        Z parent = zvalue(prefix(Z_TEST, bits - 1), bits - 1);
        ASSERT_EQ(parent, z.parent());
        
    }
}

static void contains()
{
    int64_t Z_TEST = 0xaaaaaaaaaaaaaa80L;
    Z z = zvalue(Z_TEST, Z::MAX_Z_BITS);
    for (uint32_t bits = 0; bits <= Z::MAX_Z_BITS; bits++) {
        int64_t ancestor = prefix(Z_TEST, bits);
        ASSERT_TRUE(zvalue(ancestor, bits).contains(z));
        if (bits > 0) {
            // Flip a bit in ancestor
            int64_t not_ancestor = ancestor ^ mask(bits / 2);
            ASSERT_TRUE(!zvalue(not_ancestor, bits).contains(z));
        }
    }
}

static void zLoZHi()
{
    int64_t Z_TEST = 0xaaaaaaaaaaaaaa80L;
    for (uint32_t bits = 0; bits <= Z::MAX_Z_BITS; bits++) {
        int64_t expected_lo = prefix(Z_TEST, bits);
        int64_t expected_hi = 
            expected_lo | 
            (((1L << (Z::MAX_Z_BITS - bits)) - 1) << (Z::LENGTH_BITS + 1));
        ASSERT_EQ(zvalue(expected_lo, bits).asInteger(), 
                      zvalue(prefix(Z_TEST, bits), bits).lo());
        ASSERT_EQ(zvalue(expected_hi, bits).asInteger(), 
                      zvalue(prefix(Z_TEST, bits), bits).hi());
    }
}

static void testZValues()
{
    siblings();
    parent();
    contains();
    zLoZHi();
}

//----------------------------------------------------------------------

// Decomposition

static void decomposeEntireSpace()
{
    double lo[] = {0.0, 0.0};
    double hi[] = {1024.0, 1024.0};
    uint32_t x_bits[] = {10, 10};
    Space space(2, lo, hi, x_bits);
    Box2 box(0, 1023, 0, 1023);
    OutputArray<const SpatialObject*> output;
    SessionMemory<const SpatialObject*> memory;
    ZArray* zs = memory.zArray();
    space.decompose(&box, 4, &memory);
    ASSERT_EQ(1, zs->length());
    ASSERT_EQ(zvalue(0x0000000000000000L, 0), zs->at(0));
}

static void decomposeLeftHalfSpace()
{
    double lo[] = {0.0, 0.0};
    double hi[] = {1024.0, 1024.0};
    uint32_t x_bits[] = {10, 10};
    Space space(2, lo, hi, x_bits);
    Box2 box(0, 511, 0, 1023);
    OutputArray<const SpatialObject*> output;
    SessionMemory<const SpatialObject*> memory;
    ZArray* zs = memory.zArray();
    space.decompose(&box, 4, &memory);
    ASSERT_EQ(1, zs->length());
    ASSERT_EQ(zvalue(0x0000000000000000L, 1), zs->at(0));
}

static void decomposeRightHalfSpace()
{
    double lo[] = {0.0, 0.0};
    double hi[] = {1024.0, 1024.0};
    uint32_t x_bits[] = {10, 10};
    Space space(2, lo, hi, x_bits);
    Box2 box(512, 1023, 0, 1023);
    OutputArray<const SpatialObject*> output;
    SessionMemory<const SpatialObject*> memory;
    ZArray* zs = memory.zArray();
    space.decompose(&box, 4, &memory);
    ASSERT_EQ(1, zs->length());
    ASSERT_EQ(zvalue(0x8000000000000000L, 1), zs->at(0));
}

static void decomposeBottomHalfSpace()
{
    double lo[] = {0.0, 0.0};
    double hi[] = {1024.0, 1024.0};
    uint32_t x_bits[] = {10, 10};
    Space space(2, lo, hi, x_bits);
    Box2 box(0, 1023, 0, 511);
    OutputArray<const SpatialObject*> output;
    SessionMemory<const SpatialObject*> memory;
    ZArray* zs = memory.zArray();
    space.decompose(&box, 4, &memory);
    ASSERT_EQ(2, zs->length());
    ASSERT_EQ(zvalue(0x0000000000000000L, 2), zs->at(0));
    ASSERT_EQ(zvalue(0x8000000000000000L, 2), zs->at(1));
}

static void decomposeTopHalfSpace()
{
    double lo[] = {0.0, 0.0};
    double hi[] = {1024.0, 1024.0};
    uint32_t x_bits[] = {10, 10};
    Space space(2, lo, hi, x_bits);
    Box2 box(0, 1023, 512, 1023);
    OutputArray<const SpatialObject*> output;
    SessionMemory<const SpatialObject*> memory;
    ZArray* zs = memory.zArray();
    space.decompose(&box, 4, &memory);
    ASSERT_EQ(2, zs->length());
    ASSERT_EQ(zvalue(0x4000000000000000L, 2), zs->at(0));
    ASSERT_EQ(zvalue(0xc000000000000000L, 2), zs->at(1));
}

static void decomposeTinyBoxInMiddleOfSpace()
{
    double lo[] = {0.0, 0.0};
    double hi[] = {1024.0, 1024.0};
    uint32_t x_bits[] = {10, 10};
    Space space(2, lo, hi, x_bits);
    Box2 box(511, 512, 511, 512);
    OutputArray<const SpatialObject*> output;
    SessionMemory<const SpatialObject*> memory;
    ZArray* zs = memory.zArray();
    space.decompose(&box, 4, &memory);
    ASSERT_EQ(4, zs->length());
    ASSERT_EQ(zvalue(0x3ffff00000000000L, 20), zs->at(0));
    ASSERT_EQ(zvalue(0x6aaaa00000000000L, 20), zs->at(1));
    ASSERT_EQ(zvalue(0x9555500000000000L, 20), zs->at(2));
    ASSERT_EQ(zvalue(0xc000000000000000L, 20), zs->at(3));
}

static void testDecomposition()
{
    decomposeEntireSpace();
    decomposeLeftHalfSpace();
    decomposeRightHalfSpace();
    decomposeBottomHalfSpace();
    decomposeTopHalfSpace();
    decomposeTinyBoxInMiddleOfSpace();
}

//----------------------------------------------------------------------

// ByteBuffer

static void byteBufferReadWrite()
{
    byte bytes[1000];
    ByteBuffer buffer(bytes, 1000);
    ASSERT_EQ(1000, buffer.capacity());
    ASSERT_EQ(1000, buffer.limit());
    byte three_bytes[] = {0xab, 0x00, 0x9a};
    buffer.clear();
    buffer.putInt32(0x12345678); // positive
    ASSERT_EQ(4, buffer.position());
    ASSERT_EQ(996, buffer.remaining());
    buffer.putInt32(0xfedcba98); // negative
    ASSERT_EQ(8, buffer.position());
    ASSERT_EQ(992, buffer.remaining());
    buffer.putUint32(0x55555555);
    ASSERT_EQ(12, buffer.position());
    ASSERT_EQ(988, buffer.remaining());
    buffer.putInt64(0x0123456789abcdefLL); // positive
    ASSERT_EQ(20, buffer.position());
    ASSERT_EQ(980, buffer.remaining());
    buffer.putInt64(0xfedcba9876543210LL); // negative
    ASSERT_EQ(28, buffer.position());
    ASSERT_EQ(972, buffer.remaining());
    buffer.putUint64(0xaaaaaaaaaaaaaaaaLL);
    ASSERT_EQ(36, buffer.position());
    ASSERT_EQ(964, buffer.remaining());
    buffer.putDouble(3.1415926535897932);
    ASSERT_EQ(44, buffer.position());
    ASSERT_EQ(956, buffer.remaining());
    buffer.putBytes(three_bytes, 3);
    ASSERT_EQ(47, buffer.position());
    ASSERT_EQ(953, buffer.remaining());
    uint32_t after_write = buffer.position();
    buffer.flip();
    ASSERT_EQ(after_write, buffer.limit());
    ASSERT_EQ(0x12345678, buffer.getInt32());
    ASSERT_EQ(4, buffer.position());
    ASSERT_EQ(0xfedcba98, buffer.getInt32());
    ASSERT_EQ(8, buffer.position());
    ASSERT_EQ(0x55555555, buffer.getUint32());
    ASSERT_EQ(12, buffer.position());
    ASSERT_EQ(0x0123456789abcdefLL, buffer.getInt64());
    ASSERT_EQ(20, buffer.position());
    ASSERT_EQ(0xfedcba9876543210LL, buffer.getInt64());
    ASSERT_EQ(28, buffer.position());
    ASSERT_EQ(0xaaaaaaaaaaaaaaaaLL, buffer.getUint64());
    ASSERT_EQ(36, buffer.position());
    ASSERT_EQ(3.1415926535897932, buffer.getDouble());
    ASSERT_EQ(44, buffer.position());
    buffer.getBytes(three_bytes, 3);
    ASSERT_EQ(0xab, three_bytes[0]);
    ASSERT_EQ(0x00, three_bytes[1]);
    ASSERT_EQ(0x9a, three_bytes[2]);
    ASSERT_EQ(47, buffer.position());
    uint32_t after_read = buffer.position();
    ASSERT_EQ(after_write, after_read);
    buffer.rewind();
    ASSERT_EQ(0, buffer.position());
    ASSERT_EQ(after_read, buffer.limit());
    buffer.getInt32();
    ASSERT_EQ(4, buffer.position());
    buffer.mark();
    buffer.getInt32();
    ASSERT_EQ(8, buffer.position());
    buffer.reset();
    ASSERT_EQ(4, buffer.position());
}

static void byteBufferOverflow()
{
    for (int32_t buffer_size = 1; buffer_size <= 20; buffer_size++) {
        byte bytes[buffer_size];
        ByteBuffer buffer(bytes, buffer_size);
        int32_t bytes_written = 0;
        try {
            while (true) {
                buffer.putInt32(1);
                bytes_written += sizeof(int32_t);
            }
        } catch (ByteBufferOverflowException e) {
        }
        ASSERT_TRUE(bytes_written <= buffer_size);
        ASSERT_EQ(sizeof(int32_t) * (buffer_size / sizeof(int32_t)), bytes_written);
    }
}

static void byteBufferUnderflow()
{
    for (int32_t buffer_size = 1; buffer_size <= 20; buffer_size++) {
        byte bytes[buffer_size];
        ByteBuffer buffer(bytes, buffer_size);
        int32_t n_ints = buffer_size / sizeof(int32_t);
        for (int i = 0; i < n_ints; i++) {
            buffer.putInt32(1);
        }
        buffer.flip();
        for (int i = 0; i < n_ints; i++) {
            buffer.getInt32();
        }
        try {
            buffer.getInt32();
            ASSERT_TRUE(false);
        } catch (ByteBufferUnderflowException e) {
            // expected
        }
    }
}

static void testByteBuffer()
{
    byteBufferReadWrite();
    byteBufferOverflow();
    byteBufferUnderflow();
}

//----------------------------------------------------------------------

// main

#define RUN_TEST(test) { printf("%s\n", #test); test(); }

int main(int32_t argc, const char** argv)
{
    RUN_TEST(testSpace);
    RUN_TEST(testInterleave);
    RUN_TEST(testZValues);
    RUN_TEST(testDecomposition);
    RUN_TEST(testByteBuffer);
}
