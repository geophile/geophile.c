#include <assert.h>
#include <stdio.h>
// geophile includes
#include "Space.h"
#include "SpatialObjectTypes.h"
#include "Point2.h"
#include "Box2.h"
#include "SpatialObjectKey.h"
#include "OrderedIndex.h"
#include "SessionMemory.h"
#include "InMemorySpatialObjectMemoryManager.h"
#include "SpatialIndex.h"
#include "SpatialIndexFilter.h"
#include "SpatialIndexScan.h"
#include "Cursor.h"
#include "IntSet.h"
#include "IntList.h"
#include "OutputArray.h"
#include "ByteBuffer.h"
// Testing includes
#include "OrderedIndexFactory.h"
#include "TestSpatialObject.h"

using namespace geophile;

static SpatialObjectTypes SPATIAL_OBJECT_TYPES;
static InMemorySpatialObjectMemoryManager spatial_object_memory_manager;

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

static void load(OrderedIndex<SpatialObjectPointer>* index, 
                 uint32_t n_objects, 
                 uint32_t copies)
{
    for (uint32_t id = 0; id < n_objects; id++) {
        TestSpatialObject* spatial_object = new TestSpatialObject(id);
        for (uint32_t c = 0; c < copies; c++) {
            Z z = int_to_z((id + c) * GAP);
            index->add(z, spatial_object);
        }
    }
}

static void checkContents(OrderedIndex<SpatialObjectPointer>* index, 
                          uint32_t n_objects, 
                          uint32_t copies, 
                          const IntSet& removed_ids)
{
    IntSet present_ids(n_objects);
    IntList** z_by_id = new IntList*[n_objects];
    for (uint32_t id = 0; id < n_objects; id++) {
        z_by_id[id] = new IntList(copies);
    }
    Cursor<SpatialObjectPointer>* cursor = index->cursor();
    cursor->goTo(Z(Z::Z_MIN, 0));
    geophile::Record<SpatialObjectPointer> record;
    while (!(record = cursor->next()).eof()) {
        int64_t id = record.spatialObjectReference().spatialObjectId();
        ASSERT_TRUE(!removed_ids.contains(id));
        present_ids.add(id);
        IntList& z_list = *z_by_id[id];
        z_list.append(z_to_int(record.key().z()));
    }
    for (int64_t id = 0; id < n_objects; id++) {
        if (!removed_ids.contains(id)) {
            ASSERT_TRUE(present_ids.contains(id));
            IntList& z_list = *z_by_id[id];
            ASSERT_EQ(copies, z_list.count());
            int64_t z_expected = id * GAP;
            for (uint32_t z_list_position = 0; 
                 z_list_position < z_list.count(); 
                 z_list_position++) {
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

static void checkRetrieval(OrderedIndex<SpatialObjectPointer>* index, 
                           uint32_t n_objects, 
                           uint32_t copies)
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
    geophile::Record<SpatialObjectPointer> record;
    int64_t start;
    SpatialObjectKey start_key;
    Cursor<SpatialObjectPointer>* cursor = index->cursor();
    // Try traversal forward from the beginning
    start_key = SpatialObjectKey(zvalue(Z::Z_MIN, 0));
    expected = &all_keys[0];
    cursor->goTo(start_key.z());
    while (!(record = cursor->next()).eof()) {
        ASSERT_EQ(expected->z(), record.key().z());
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
        expected++;
    }
    ASSERT_EQ(expected, all_keys_limit);
    // Try traversal forward from the end
    start_key = SpatialObjectKey(zvalue(Z::Z_MAX, Z::MAX_Z_BITS));
    expected = all_keys_limit;
    cursor->goTo(start_key.z());
    while (!(record = cursor->next()).eof()) {
        ASSERT_TRUE(false);
    }
    ASSERT_TRUE(expected == all_keys_limit);
    // Try traversal backward from the beginning (lower bound is before first key)
    start_key = SpatialObjectKey(zvalue(Z::Z_MIN, 0));
    expected = all_keys_reverse_limit;
    cursor->goTo(start_key.z());
    while (!(record = cursor->previous()).eof()) {
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
        expected--;
    }
    ASSERT_EQ(expected, all_keys_reverse_limit);
    // Try traversal backward from the end (upper bound gets everything)
    start_key = SpatialObjectKey(zvalue(Z::Z_MAX, Z::MAX_Z_BITS));
    expected = all_keys_limit - 1;
    cursor->goTo(start_key.z());
    while (!(record = cursor->previous()).eof()) {
        ASSERT_EQ(expected->z(), record.key().z());
        expected--;
    }
    ASSERT_EQ(expected, all_keys_reverse_limit);
    delete cursor;
    delete [] all_keys;
}

static void removeAll(OrderedIndex<SpatialObjectPointer>* index, 
                      uint32_t n_objects, 
                      uint32_t copies)
{
    IntSet removed_ids(n_objects);
    for (int64_t id = 0; id < n_objects; id++) {
        const SpatialObject* target = NULL;
        // Remove id and check remaining contents
        for (int64_t c = 0; c < copies; c++) {
            int64_t expected = (id + c) * GAP;
            SpatialObjectPointer removed = index->remove(int_to_z(expected), id);
            ASSERT_TRUE(!removed.isNull());
            if (target) {
                ASSERT_EQ(target, spatial_object_memory_manager.spatialObject(removed));
            } else {
                target = spatial_object_memory_manager.spatialObject(removed);
            }
            removed = index->remove(int_to_z(expected + GAP / 2), id);
            ASSERT_TRUE(removed.isNull());
            removed = index->remove(int_to_z(expected), 0x7fffffffffffffffLL);
            ASSERT_TRUE(removed.isNull());
        }
        // Target was shared by all the removed index records. Safe to delete now that
        // the index records have been removed.
        delete target;
        removed_ids.add(id);
        if (id % 1000 == 0) {
            checkContents(index, n_objects, copies, removed_ids);
        }
    }
}

static void testIndexCreationAndDestruction
(const OrderedIndexFactory<SpatialObjectPointer>* index_factory)
{
    OrderedIndex<SpatialObjectPointer>* index = 
        index_factory->newIndex(&SPATIAL_OBJECT_TYPES);
    delete index;
}

static void testIndexOperations(const OrderedIndexFactory<SpatialObjectPointer>* index_factory)
{
    for (uint32_t n_objects = 0; n_objects <= 1000; n_objects += 100) {
        for (uint32_t copies = 1; copies <= 8; copies++) {
            OrderedIndex<SpatialObjectPointer>* index = 
                index_factory->newIndex(&SPATIAL_OBJECT_TYPES);
            load(index, n_objects, copies);
            index->freeze();
            checkContents(index, n_objects, copies, IntSet(n_objects));
            checkRetrieval(index, n_objects, copies);
            removeAll(index, n_objects, copies);
            delete index;
        }
    }
}


//----------------------------------------------------------------------

// Cursor

static Z key(const geophile::Record<SpatialObjectPointer>& entry)
{
    return entry.key().z();
}


static void testCursor(OrderedIndex<SpatialObjectPointer>* index, int32_t n) 
{
    Cursor<SpatialObjectPointer>* cursor = index->cursor();
    int64_t expected_key;
    int64_t expected_last_key;
    int32_t expected_empty;
    geophile::Record<SpatialObjectPointer> record;
    // Full cursor
    {
        cursor->goTo(SpatialObjectKey(zvalue(Z::Z_MIN, 0)));
        expected_key = 0;
        while (!(record = cursor->next()).eof()) {
            ASSERT_EQ(expected_key, z_to_int(key(record)));
            expected_key += GAP;
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
        }
        while (!(record = cursor->next()).eof()) {
            ASSERT_EQ(expected_key, z_to_int(key(record)));
            expected_key += GAP;
            if (expected_key != n * GAP) {
                record = cursor->next();
                ASSERT_TRUE(!record.eof());
                ASSERT_EQ(expected_key, z_to_int(key(record)));
                expected_key -= GAP;
                record = cursor->previous();
                ASSERT_EQ(expected_key, z_to_int(key(record)));
                expected_key += GAP; // About to go to next
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
        }
        while (!(record = cursor->previous()).eof()) {
            ASSERT_EQ(expected_key, z_to_int(key(record)));
            expected_key -= GAP;
            if (expected_key >= 0) {
                record = cursor->previous();
                ASSERT_TRUE(!record.eof());
                ASSERT_EQ(expected_key, z_to_int(key(record)));
                expected_key += GAP;
                record = cursor->next();
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
                // Match, previous
                cursor->goTo(SpatialObjectKey(int_to_z(match + 1)));
                ASSERT_EQ(match, z_to_int(key(record = cursor->previous())));
            }
            // Before, next
            before = match - GAP / 2;
            cursor->goTo(SpatialObjectKey(int_to_z(before)));
            if (i == n) {
                ASSERT_TRUE(cursor->next().eof());
            } else {
                ASSERT_EQ(match, z_to_int(key(record = cursor->next())));
            }
            // Before, previous
            cursor->goTo(SpatialObjectKey(int_to_z(before)));
            if (i == 0) {
                ASSERT_TRUE(cursor->previous().eof());
            } else {
                ASSERT_EQ(match - GAP, z_to_int(key(record = cursor->previous())));
            }
        }
    }
    delete cursor;
}

static void testCursorVaryingIndexSizes
(const OrderedIndexFactory<SpatialObjectPointer>* index_factory)
{
    static const uint32_t N_MAX = 100;
    for (uint32_t n = 0; n <= N_MAX; n++) {
        OrderedIndex<SpatialObjectPointer>* index = index_factory->newIndex(&SPATIAL_OBJECT_TYPES);
        ASSERT_TRUE(GAP > 1);
        // Populate map with keys 0, GAP, ..., GAP * (n - 1)
        for (uint32_t i = 0; i < n; i++) {
            uint32_t key = GAP * i;
            Point2* point = new Point2(key, key);
            index->add(int_to_z(key), point);
        }
        testCursor(index, n);
        delete index;
    }
}

static void testCursor(const OrderedIndexFactory<SpatialObjectPointer>* index_factory)
{
    testCursorVaryingIndexSizes(index_factory);
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

static void compare(const OutputArray<SpatialObjectPointer>* expected, 
                    const OutputArray<SpatialObjectPointer>* actual)
{
    ASSERT_EQ(expected->length(), actual->length());
    for (uint32_t i = 0; i < actual->length(); i++) {
        ASSERT_TRUE(expected->at(i).spatialObject()->equalTo(*actual->at(i).spatialObject()));
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

static void dump(const char* label, OutputArray<SpatialObjectPointer>* array)
{
    printf("%s - %d:\n", label, array->length());
    for (int i = 0; i < array->length(); i++) {
        Point2* point = (Point2*) spatial_object_memory_manager.spatialObject(array->at(i));
        printf("    (%f, %f)\n", point->x(), point->y());
    }
}

static void testRetrieval(SpatialIndex<SpatialObjectPointer>* spatial_index,
                           SessionMemory<SpatialObjectPointer>* memory,
                           int64_t xlo, int64_t xhi, int64_t ylo, int64_t yhi) 
{
    Box2 box(xlo, xhi, ylo, yhi);
    PointFilter filter;
    box.id(0);
    const Space* space = spatial_index->space();
    ZArray* zs = memory->zArray();
    space->decompose(&box, box.maxZ(), memory);
    SpatialIndexScan<SpatialObjectPointer>* scan = spatial_index->newScan(&box, &filter, memory);
    for (uint32_t i = 0; i < zs->length(); i++) {
        scan->find(zs->at(i));
    }
    OutputArray<SpatialObjectPointer>* actual = 
        (OutputArray<SpatialObjectPointer>*) memory->output();
    OutputArray<SpatialObjectPointer>* expected = 
        new OutputArray<SpatialObjectPointer>();
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
    for (uint32_t i = 0; i < expected->length(); i++) {
        delete (SpatialObject*) spatial_object_memory_manager.spatialObject(expected->at(i));
    }
    delete expected;
    delete scan;
}

static void testRetrievalRandomized(const OrderedIndexFactory<SpatialObjectPointer>* index_factory)
{
    static const uint32_t X_MAX = 1000;
    static const uint32_t Y_MAX = 1000;
    static const uint32_t N_RECORDS = (X_MAX / 10) * (Y_MAX / 10);
    double lo[] = {0.0, 0.0};
    double hi[] = {X_MAX, Y_MAX};
    uint32_t x_bits[] = {10, 10};
    Space* space = new Space(2, lo, hi, x_bits);
    OrderedIndex<SpatialObjectPointer>* index = index_factory->newIndex(&SPATIAL_OBJECT_TYPES);
    SpatialIndex<SpatialObjectPointer>* spatial_index = 
        new SpatialIndex<SpatialObjectPointer>(space, index, &spatial_object_memory_manager);
    SessionMemory<SpatialObjectPointer> memory;
    int64_t id = 0;
    for (double x = 0; x < X_MAX; x += 10) {
        for (double y = 0; y < Y_MAX; y += 10) {
            Point2* point = new Point2(x, y);
            point->id(id++);
            spatial_index->add(point, &memory);
        }
    }
    // fflush(stdout);
    spatial_index->freeze();
    srand(419419);
    int64_t xlo;
    int64_t xhi;
    int64_t ylo;
    int64_t yhi;
    const int TRIALS = 100;
    for (uint32_t i = 0; i < TRIALS; i++) {
        do {
            xlo = rand() % X_MAX;
            xhi = xlo + (rand() % (X_MAX - xlo));
        } while (xhi < xlo);
        do {
            ylo = rand() % Y_MAX;
            yhi = ylo + (rand() % (Y_MAX - ylo));
        } while (yhi < ylo);
        testRetrieval(spatial_index, &memory, xlo, xhi, ylo, yhi);
    }
    delete spatial_index;
    delete index;
    delete space;
}

static void testRetrieval(const OrderedIndexFactory<SpatialObjectPointer>* index_factory)
{
    testRetrievalRandomized(index_factory);
}

//----------------------------------------------------------------------

// main

#define RUN_TEST(test, index_factory) { printf("%s\n", #test); test(index_factory); }

int runTests(OrderedIndexFactory<SpatialObjectPointer>* index_factory)
{
    setup();
    RUN_TEST(testIndexCreationAndDestruction, index_factory);
    RUN_TEST(testIndexOperations, index_factory);
    RUN_TEST(testCursor, index_factory);
    RUN_TEST(testRetrieval, index_factory);
}
