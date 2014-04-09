#include <assert.h>
#include <stdio.h>

#include "Space.h"
#include "SpatialObjectTypes.h"
#include "Point2.h"
#include "Box2.h"
#include "Z.h"
#include "ZArray.h"
#include "ByteBuffer.h"
#include "SpatialObjectKey.h"
#include "OrderedIndex.h"
#include "SessionMemory.h"
#include "SpatialIndex.h"
#include "SpatialIndexFilter.h"
#include "SpatialIndexScan.h"
#include "Cursor.h"
#include "IntSet.h"
#include "IntList.h"
#include "SpatialObjectArray.h"

#include "RecordArray.h"
#include "TestSpatialObject.h"

using namespace geophile;

SpatialObjectTypes SPATIAL_OBJECT_TYPES;

//----------------------------------------------------------------------

// Testing

#define ASSERT_EQ(x, y) assert((x) == (y))

#define ASSERT_TRUE(x) assert(x)

//----------------------------------------------------------------------

// SpatialObject constructors and registration

static SpatialObject* newTestSpatialObject()
{
    return new TestSpatialObject();
}

static SpatialObject* newPoint()
{
    return new Point2();
}

static SpatialObject* newBox()
{
    return new Box2();
}

static void setup()
{
    SPATIAL_OBJECT_TYPES.registerType(TestSpatialObject::TYPE_ID, newTestSpatialObject);
    SPATIAL_OBJECT_TYPES.registerType(Point2::TYPE_ID, newPoint);
    SPATIAL_OBJECT_TYPES.registerType(Box2::TYPE_ID, newBox);
}

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
        double lo[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        double hi[] = {1000.0, 1000.0, 1000.0, 1000.0, 1000.0, 1000.0, 1000.0, 1000.0, 1000.0, 1000.0};
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

static void checkInterleave(const Space& space, int64_t expected, uint64_t x, uint64_t y, uint64_t z)
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
    SessionMemory memory;
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
    SessionMemory memory;
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
    SessionMemory memory;
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
    SessionMemory memory;
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
    SessionMemory memory;
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
    SessionMemory memory;
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

static void byteBuffer()
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
    // TODO: Check exceptions
}

static void testByteBuffer()
{
    byteBuffer();
}

//----------------------------------------------------------------------

// Index

/*
 *  With zCount = 3, GAP = 10:
 *
 *   spatial object id   0     1     2     ...     (nObjects - 1)
 *
 *                z      0
 *                      10    10
 *                      20    20    20
 *                            30    30    30
 *                                  40    40    40
 *                                        50    50
 *
 */

static const int32_t GAP = 10;

static Z int_to_z(int64_t x)
{
    return Z(x << Z::LENGTH_BITS, Z::MAX_Z_BITS);
}

static int64_t z_to_int(Z z)
{
    return z.asInteger() >> Z::LENGTH_BITS;
}

static int32_t key_position(const SpatialObjectKey* keys, 
                            uint32_t n_keys,
                            SpatialObjectKey key,
                            int32_t forward_on_missing_key)
{
    int32_t position;
    int32_t lo = 0;
    int32_t hi = n_keys - 1;
    int32_t mid;
    int32_t found = false;
    const SpatialObjectKey* mid_key;
    while (lo <= hi) {
        mid = (lo + hi) / 2;
        mid_key = &keys[mid];
        int32_t c = mid_key->compare(key);
        if (c < 0) {
            lo = mid + 1;
        } else if (c > 0) {
            hi = mid - 1;
        } else {
            // key found at mid
            found = true;
        }
    }
    position = 
        found ? mid : 
        forward_on_missing_key ? lo : 
        lo - 1;
    return position;
}

static void load(OrderedIndex<SpatialObject*>& index, uint32_t n_objects, uint32_t copies)
{
    for (uint32_t id = 0; id < n_objects; id++) {
        TestSpatialObject spatial_object(id);
        for (uint32_t c = 0; c < copies; c++) {
            Z z = int_to_z((id + c) * GAP);
            index.add(z, &spatial_object);
        }
    }
}

static void checkContents(OrderedIndex<SpatialObject*>& index, 
                          uint32_t n_objects, 
                          uint32_t copies, 
                          const IntSet& removed_ids)
{
    IntSet present_ids(n_objects);
    IntList** z_by_id = new IntList*[n_objects];
    for (uint32_t id = 0; id < n_objects; id++) {
        z_by_id[id] = new IntList(copies);
    }
    Cursor<SpatialObject*>* cursor = index.cursor();
    cursor->goTo(Z(Z::Z_MIN, 0));
    geophile::Record<SpatialObject*> record;
    while (!(record = cursor->next()).eof()) {
        int64_t id = (int) record.spatialObject()->id();
        ASSERT_TRUE(!removed_ids.contains(id));
        present_ids.add(id);
        IntList& z_list = *z_by_id[id];
        z_list.append(z_to_int(record.key().z()));
        delete record.spatialObject();
    }
    for (int64_t id = 0; id < n_objects; id++) {
        if (!removed_ids.contains(id)) {
            ASSERT_TRUE(present_ids.contains(id));
            IntList& z_list = *z_by_id[id];
            ASSERT_EQ(copies, z_list.count());
            int64_t z_expected = id * GAP;
            for (uint32_t z_list_position = 0; z_list_position < z_list.count(); z_list_position++) {
                int64_t z = z_list.at(z_list_position);
                ASSERT_EQ(z_expected, z);
                z_expected += GAP;
            }
        }
    }
    for (uint32_t id = 0; id < n_objects; id++) {
        delete z_by_id[id];
    }
    delete [] z_by_id;
    delete cursor;
}

static int32_t compareSpatialObjectKey(const void* x, const void* y)
{
    return ((const SpatialObjectKey*)x)->compare(*(const SpatialObjectKey*)y);
}

static void checkRetrieval(OrderedIndex<SpatialObject*>& index, uint32_t n_objects, uint32_t copies)
{
    // Expected
    uint32_t n_keys = n_objects * copies;
    SpatialObjectKey* all_keys = new SpatialObjectKey[n_keys];
    SpatialObjectKey* all_keys_limit = &all_keys[n_keys];
    SpatialObjectKey* all_keys_reverse_limit = &all_keys[-1];
    SpatialObjectKey* key = all_keys;
    for (int32_t id = 0; id < n_objects; id++) {
        for (int32_t c = 0; c < copies; c++) {
            key->set(int_to_z((id + c) * GAP), id);
            key++;
        }
    }
    qsort(all_keys, n_objects * copies, sizeof(SpatialObjectKey), compareSpatialObjectKey);
    SpatialObjectKey* expected;
    geophile::Record<SpatialObject*> record;
    int64_t start;
    SpatialObjectKey start_key;
    Cursor<SpatialObject*>* cursor = index.cursor();
    // Try traversal forward from the beginning
    start_key = SpatialObjectKey(zvalue(Z::Z_MIN, 0));
    expected = &all_keys[0];
    cursor->goTo(start_key.z());
    while (!(record = cursor->next()).eof()) {
        ASSERT_EQ(expected->z(), record.key().z());
        delete record.spatialObject();
        expected++;
    }
    ASSERT_TRUE(expected == all_keys_limit);
    // Try traversal forward from halfway
    start = GAP * n_objects / 2 + GAP / 2;
    start_key = SpatialObjectKey(int_to_z(start));
    expected = &all_keys[key_position(all_keys, n_keys, start_key, true)];
    cursor->goTo(start_key.z());
    while (!(record = cursor->next()).eof()) {
        ASSERT_EQ(expected->z(), record.key().z());
        delete record.spatialObject();
        expected++;
    }
    ASSERT_EQ(expected, all_keys_limit);
    // Try traversal forward from the end
    start_key = SpatialObjectKey(zvalue(Z::Z_MAX, Z::MAX_Z_BITS));
    expected = all_keys_limit;
    cursor->goTo(start_key.z());
    while (!(record = cursor->next()).eof()) {
        delete record.spatialObject();
        ASSERT_TRUE(false);
    }
    ASSERT_TRUE(expected == all_keys_limit);
    // Try traversal backward from the beginning (lower bound is before first key)
    start_key = SpatialObjectKey(zvalue(Z::Z_MIN, 0));
    expected = all_keys_reverse_limit;
    cursor->goTo(start_key.z());
    while (!(record = cursor->previous()).eof()) {
        delete record.spatialObject();
        ASSERT_TRUE(false);
    }
    ASSERT_TRUE(expected == all_keys_reverse_limit);
    // Try traversal backward from halfway
    start = GAP * n_objects / 2 + GAP / 2;
    start_key = SpatialObjectKey(int_to_z(start));
    expected = &all_keys[key_position(all_keys, n_keys, start_key, false)];
    cursor->goTo(start_key.z());
    while (!(record = cursor->previous()).eof()) {
        ASSERT_EQ(expected->z(), record.key().z());
        delete record.spatialObject();
        expected--;
    }
    ASSERT_EQ(expected, all_keys_reverse_limit);
    // Try traversal backward from the end (upper bound gets everything)
    start_key = SpatialObjectKey(zvalue(Z::Z_MAX, Z::MAX_Z_BITS));
    expected = all_keys_limit - 1;
    cursor->goTo(start_key.z());
    while (!(record = cursor->previous()).eof()) {
        ASSERT_EQ(expected->z(), record.key().z());
        delete record.spatialObject();
        expected--;
    }
    ASSERT_EQ(expected, all_keys_reverse_limit);
    delete cursor;
    delete [] all_keys;
}

static void removeAll(OrderedIndex<SpatialObject*>& index, uint32_t n_objects, uint32_t copies)
{
    IntSet removed_ids(n_objects);
    for (int64_t id = 0; id < n_objects; id++) {
        // Remove id and check remaining contents
        for (int64_t c = 0; c < copies; c++) {
            int64_t expected = (id + c) * GAP;
            int32_t removed = index.remove(int_to_z(expected), id);
            ASSERT_TRUE(removed);
            removed = index.remove(int_to_z(expected + GAP / 2), id);
            ASSERT_TRUE(!removed);
            removed = index.remove(int_to_z(expected), 0x7fffffffffffffffLL);
            ASSERT_TRUE(!removed);
        }
        removed_ids.add(id);
        if (id % 1000 == 0) {
            checkContents(index, n_objects, copies, removed_ids);
        }
    }
}

static void testIndexCreationAndDestruction()
{
    RecordArray<SpatialObject*> index(&SPATIAL_OBJECT_TYPES, 100);
}

static void testIndexOperations()
{
    for (uint32_t n_objects = 0; n_objects <= 1000; n_objects += 100) {
        for (uint32_t copies = 1; copies <= 8; copies++) {
            RecordArray<SpatialObject*> index(&SPATIAL_OBJECT_TYPES, n_objects * copies);
            load(index, n_objects, copies);
            index.freeze();
            checkContents(index, n_objects, copies, IntSet(n_objects));
            checkRetrieval(index, n_objects, copies);
            removeAll(index, n_objects, copies);
        }
    }
}

static void testIndex()
{
    testIndexCreationAndDestruction();
    testIndexOperations();
}

//----------------------------------------------------------------------

// Cursor

static Z key(const geophile::Record<SpatialObject*>& entry)
{
    return entry.key().z();
}


static void test_cursor(OrderedIndex<SpatialObject*>* index, int32_t n) 
{
    Cursor<SpatialObject*>* cursor = index->cursor();
    int64_t expected_key;
    int64_t expected_last_key;
    int32_t expected_empty;
    geophile::Record<SpatialObject*> record;
    // Full cursor
    {
        cursor->goTo(SpatialObjectKey(zvalue(Z::Z_MIN, 0)));
        expected_key = 0;
        while (!(record = cursor->next()).eof()) {
            ASSERT_EQ(expected_key, z_to_int(key(record)));
            expected_key += GAP;
            delete record.spatialObject();
        }
        ASSERT_EQ(n * GAP, expected_key);
    }
    // Try scans starting at, before, and after each key and ending at, before and 
    // after each key.
    {
        for (uint32_t i = 0; i < n; i++) {
            uint32_t start_base = GAP * i;
            uint32_t end_base = GAP * (n - 1 - i);
            for (int64_t start = start_base - 1; start <= start_base + 1; start++) {
                for (int64_t end = end_base - 1; end <= end_base + 1; end++) {
                    if (start <= end) {
                        cursor->goTo(int_to_z(start));
                        expected_key = 
                            start <= start_base 
                            ? start_base 
                            : start_base + GAP;
                        expected_last_key = 
                            end >= end_base 
                            ? end_base 
                            : end_base - GAP;
                        expected_empty = 
                            start > end || 
                            (start <= end && (end >= start_base || start <= end_base));
                        int32_t empty = true;
                        while (!(record = cursor->next()).eof() && 
                               record.key().z() <= int_to_z(end)) {
                            ASSERT_EQ(expected_key, z_to_int(key(record)));
                            expected_key += GAP;
                            empty = false;
                            delete record.spatialObject();
                        }
                        if (!record.eof()) {
                            delete record.spatialObject();
                        }
                        if (empty) {
                            ASSERT_TRUE(expected_empty);
                        } else {
                            ASSERT_EQ(expected_last_key + GAP, expected_key);
                        }
                    }
                }
            }
        }
    }
    // Alternating next and previous
    {
        cursor->goTo(SpatialObjectKey(zvalue(Z::Z_MIN, 0)));
        expected_key = 0;
        record = cursor->next();
        if (!record.eof()) {
            expected_key += GAP;
            delete record.spatialObject();
        }
        while (!(record = cursor->next()).eof()) {
            ASSERT_EQ(expected_key, z_to_int(key(record)));
            expected_key += GAP;
            delete record.spatialObject();
            if (expected_key != n * GAP) {
                record = cursor->next();
                ASSERT_TRUE(!record.eof());
                ASSERT_EQ(expected_key, z_to_int(key(record)));
                delete record.spatialObject();
                expected_key -= GAP;
                record = cursor->previous();
                ASSERT_EQ(expected_key, z_to_int(key(record)));
                expected_key += GAP; // About to go to next
                delete record.spatialObject();
            }
        }
        ASSERT_EQ(n * GAP, expected_key);
    }
    // Alternating previous and next
    {
        cursor->goTo(SpatialObjectKey(zvalue(Z::Z_MAX, Z::MAX_Z_BITS)));
        expected_key = (n - 1) * GAP;
        record = cursor->previous();
        if (!record.eof()) {
            expected_key -= GAP;
            delete record.spatialObject();
        }
        while (!(record = cursor->previous()).eof()) {
            delete record.spatialObject();
            ASSERT_EQ(expected_key, z_to_int(key(record)));
            expected_key -= GAP;
            if (expected_key >= 0) {
                record = cursor->previous();
                delete record.spatialObject();
                ASSERT_TRUE(!record.eof());
                ASSERT_EQ(expected_key, z_to_int(key(record)));
                expected_key += GAP;
                record = cursor->next();
                delete record.spatialObject();
                ASSERT_EQ(expected_key, z_to_int(key(record)));
                expected_key -= GAP; // About to go to next
            }
        }
        ASSERT_EQ(-GAP, expected_key);
    }
    // goTo
    if (n > 0) {
        cursor->goTo(SpatialObjectKey(zvalue(Z::Z_MIN, 0)));
        int32_t match;
        int32_t before;
        // Why not start at 0: Because before < 0 on i = 0, and Z doesn't allow that.
        for (uint32_t i = 1; i <= n; i++) {
            match = i * GAP;
            if (i < n) {
                // Match, next
                cursor->goTo(SpatialObjectKey(int_to_z(match)));
                ASSERT_EQ(match, z_to_int(key(record = cursor->next())));
                delete record.spatialObject();
                // Match, previous
                cursor->goTo(SpatialObjectKey(int_to_z(match + 1)));
                ASSERT_EQ(match, z_to_int(key(record = cursor->previous())));
                delete record.spatialObject();
            }
            // Before, next
            before = match - GAP / 2;
            cursor->goTo(SpatialObjectKey(int_to_z(before)));
            if (i == n) {
                ASSERT_TRUE(cursor->next().eof());
            } else {
                ASSERT_EQ(match, z_to_int(key(record = cursor->next())));
                delete record.spatialObject();
            }
            // Before, previous
            cursor->goTo(SpatialObjectKey(int_to_z(before)));
            if (i == 0) {
                ASSERT_TRUE(cursor->previous().eof());
            } else {
                ASSERT_EQ(match - GAP, z_to_int(key(record = cursor->previous())));
                delete record.spatialObject();
            }
        }
    }
    delete cursor;
    delete index;
}

static void testCursorVaryingIndexSizes()
{
    static const uint32_t N_MAX = 100;
    for (uint32_t n = 0; n <= N_MAX; n++) {
        OrderedIndex<SpatialObject*>* index = new RecordArray<SpatialObject*>(&SPATIAL_OBJECT_TYPES, n);
        ASSERT_TRUE(GAP > 1);
        // Populate map with keys 0, GAP, ..., GAP * (n - 1)
        for (uint32_t i = 0; i < n; i++) {
            uint32_t key = GAP * i;
            Point2 point(key, key);
            index->add(int_to_z(key), &point);
        }
        test_cursor(index, n);
    }
}

static void testCursor()
{
    testCursorVaryingIndexSizes();
}

//----------------------------------------------------------------------

// Spatial index

static int32_t contains(const Box2* box, const Point2* point)
{
    return
        box->xlo() <= point->x() && point->x() <= box->xhi() &&
        box->ylo() <= point->y() && point->y() <= box->yhi();
}

class PointFilter : public SpatialIndexFilter
{
public:
    virtual bool overlap(const SpatialObject* query_object,
                         const SpatialObject* spatial_object) const
    {
        return contains((Box2*) query_object, (Point2*) spatial_object);
    }
};

static void compare(const SpatialObjectArray* expected, const SpatialObjectArray* actual)
{
    ASSERT_EQ(expected->length(), actual->length());
    for (uint32_t i = 0; i < actual->length(); i++) {
        ASSERT_TRUE(expected->at(i)->equalTo(*actual->at(i)));
    }
}

static int32_t comparePoint(const void* x, const void* y)
{
    const Point2* p = *(const Point2**) x;
    const Point2* q = *(const Point2**) y;
    return
        p->x() < q->x() ? -1 : p->x() > q->x() ? 1 :
        p->y() < q->y() ? -1 : p->y() > q->y() ? 1 : 0;
}

static void dump(const char* label, SpatialObjectArray* array)
{
    printf("%s - %d:\n", label, array->length());
    for (int i = 0; i < array->length(); i++) {
        Point2* point = (Point2*) array->at(i);
        printf("    (%f, %f)\n", point->x(), point->y());
    }
}

static void test_retrieval(SpatialIndex<SpatialObject*>* spatial_index,
                           SessionMemory* memory,
                           int64_t xlo, int64_t xhi, int64_t ylo, int64_t yhi) 
{
    Box2 box(xlo, xhi, ylo, yhi);
    PointFilter filter;
    box.id(0);
    const Space* space = spatial_index->space();
    ZArray* zs = memory->zArray();
    space->decompose(&box, box.maxZ(), memory);
    SpatialIndexScan<SpatialObject*>* scan = spatial_index->newScan(&box, &filter, memory);
    for (uint32_t i = 0; i < zs->length(); i++) {
        scan->find(zs->at(i));
    }
    SpatialObjectArray* actual = memory->output();
    SpatialObjectArray* expected = new SpatialObjectArray();
    for (double x = 10 * ((xlo + 9) / 10); x <= 10 * (xhi / 10); x += 10) {
        for (double y = 10 * ((ylo + 9) / 10); y <= 10 * (yhi / 10); y += 10) {
            Point2* point = new Point2(x, y);
            if (contains(&box, point)) {
                expected->append(point);
            }
        }
    }
    actual->sort(comparePoint);
    expected->sort(comparePoint);
    compare(expected, actual);
    memory->clearOutput();
    delete expected;
    delete scan;
}

static void testRetrievalRandomized()
{
    static const uint32_t X_MAX = 1000;
    static const uint32_t Y_MAX = 1000;
    static const uint32_t N_RECORDS = (X_MAX / 10) * (Y_MAX / 10);
    double lo[] = {0.0, 0.0};
    double hi[] = {X_MAX, Y_MAX};
    uint32_t x_bits[] = {10, 10};
    Space* space = new Space(2, lo, hi, x_bits);
    RecordArray<SpatialObject*>* index = new RecordArray<SpatialObject*>(&SPATIAL_OBJECT_TYPES, N_RECORDS);
    SpatialIndex<SpatialObject*>* spatial_index = new SpatialIndex<SpatialObject*>(space, index);
    SessionMemory memory;
    int64_t id = 0;
    for (double x = 0; x < X_MAX; x += 10) {
        for (double y = 0; y < Y_MAX; y += 10) {
            Point2 point(x, y);
            point.id(id++);
            spatial_index->add(&point, &memory);
        }
    }
    // fflush(stdout);
    spatial_index->freeze();
    srand(419419);
    int64_t xlo;
    int64_t xhi;
    int64_t ylo;
    int64_t yhi;
    for (uint32_t i = 0; i < 100; i++) {
        do {
            xlo = rand() % X_MAX;
            xhi = xlo + (rand() % (X_MAX - xlo));
        } while (xhi < xlo);
        do {
            ylo = rand() % Y_MAX;
            yhi = ylo + (rand() % (Y_MAX - ylo));
        } while (yhi < ylo);
        test_retrieval(spatial_index, &memory, xlo, xhi, ylo, yhi);
    }
    delete spatial_index;
    delete index;
    delete space;
}

static void testRetrieval()
{
    testRetrievalRandomized();
}

//----------------------------------------------------------------------

// main

#define RUN_TEST(test) { printf("%s\n", #test); test(); }

int main(int32_t argc, const char** argv)
{
    setup();
    RUN_TEST(testSpace);
    RUN_TEST(testInterleave);
    RUN_TEST(testZValues);
    RUN_TEST(testDecomposition);
    RUN_TEST(testByteBuffer);
    RUN_TEST(testIndexCreationAndDestruction);
    RUN_TEST(testIndexOperations);
    RUN_TEST(testIndex);
    RUN_TEST(testCursor);
    RUN_TEST(testRetrieval);
}
