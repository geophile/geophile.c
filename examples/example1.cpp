#include <stdio.h>
#include <math.h>

#include <geophile/geophile.h>
#include <geophile/InMemorySpatialObjectReferenceManager.h>

#include "RecordArray.h"
#include "Stopwatch.h"

using namespace geophile;

static const int64_t X_MAX = 1000000;
static const int64_t Y_MAX = 1000000;
static const uint32_t N_POINTS = 1000000;
static const uint32_t N_QUERIES = 5;
static const uint32_t DESIRED_RESULT_SIZE = 5;
static const uint32_t MAX_REGIONS = 8;
static SpatialObjectTypes spatial_object_types;
static SessionMemory<SpatialObjectPointer> memory;
static InMemorySpatialObjectReferenceManager spatial_object_reference_manager;
static Stopwatch stopwatch;
static Point2* points[N_POINTS];

class PointFilter : public SpatialIndexFilter
{
public:
    virtual bool overlap(const SpatialObject* query_object,
                         const SpatialObject* spatial_object) const
    {
        Box2* box = (Box2*) query_object;
        Point2* point = (Point2*) spatial_object;
        return
            box->xlo() <= point->x() && point->x() <= box->xhi() &&
            box->ylo() <= point->y() && point->y() <= box->yhi();
    }
};

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
    spatial_object_types.registerType(Point2::TYPE_ID, newPoint);
    spatial_object_types.registerType(Box2::TYPE_ID, newBox);
}

static Space* createSpace()
{
    double lo[] = {0.0, 0.0};
    double hi[] = {X_MAX, Y_MAX};
    uint32_t x_bits[] = {10, 10};
    Space* space = new Space(2, lo, hi, x_bits);
    return space;
}

static OrderedIndex<SpatialObjectPointer>* createIndex()
{
    return new RecordArray<SpatialObjectPointer>(&spatial_object_types, 
                                                 &spatial_object_reference_manager,
                                                 &memory);
}

static void loadRandomPoints(SpatialIndex<SpatialObjectPointer>* spatial_index, 
                             SessionMemory<SpatialObjectPointer>* memory)
{
    srand(419);
    stopwatch.reset();
    int64_t id = 0;
    for (uint32_t id = 0; id < N_POINTS; id++) {
        double x = rand() % X_MAX;
        double y = rand() % Y_MAX;
        // Add a point in the heap, that will be owned by the index.
        Point2* point = new Point2(x, y);
        point->id(id);
        spatial_index->add(point, memory);
        points[id] = point;
    }
    spatial_index->freeze();
    stopwatch.stop();
    double sec = stopwatch.usec() / 1000000.0;
    printf("Loaded %d points in %f sec (%f points/sec)\n",
           N_POINTS, sec, N_POINTS / sec);
}

static void deletePoints()
{
    for (uint32_t id = 0; id < N_POINTS; id++) {
        delete points[id];
    }
}

static void runQueries(SpatialIndex<SpatialObjectPointer>* spatial_index, 
                       SessionMemory<SpatialObjectPointer>* memory)
{
    // Select query size based on the desired result size. Use square queries.
    uint32_t x_width = 
        sqrt(((double)DESIRED_RESULT_SIZE / N_POINTS) * 
             ((double) (X_MAX * Y_MAX)));
    uint32_t y_width = x_width;
    stopwatch.reset();
    uint32_t total_points = 0;
    for (uint32_t q = 0; q < N_QUERIES; q++) {
        double x_lo = rand() % (X_MAX - x_width);
        double y_lo = rand() % (Y_MAX - y_width);
        double x_hi = x_lo + x_width;
        double y_hi = y_lo + y_width;
        Box2 box(x_lo, x_hi, y_lo, y_hi);
        printf("Query %d: (%f : %f, %f : %f)\n", q, x_lo, x_hi, y_lo, y_hi);
        PointFilter filter;
        spatial_index->findOverlapping(&box, &filter, memory);
        OutputArray<SpatialObjectPointer>* output = 
            (OutputArray<SpatialObjectPointer>*) memory->output();
        for (uint32_t j = 0; j < output->length(); j++) {
            Point2* p = (Point2*) output->at(j).spatialObject();
            printf("        (%f, %f)\n", p->x(), p->y());
        }
        total_points += output->length();
        memory->clearOutput();
    }
    stopwatch.stop();
    double average_points_per_query = (double) total_points / N_QUERIES;
    double sec = stopwatch.usec() / 1000000.0;
    double average_query_sec = sec / N_QUERIES;
    printf("Average points per query: %f\n", average_points_per_query);
    printf("Average query time: %f msec\n", average_query_sec * 1000);
}

//----------------------------------------------------------------------

// main

int main(int32_t argc, const char** argv)
{
    setup();
    Space* space = createSpace();
    OrderedIndex<SpatialObjectPointer>* index = createIndex();
    SpatialIndex<SpatialObjectPointer>* spatial_index = 
        new SpatialIndex<SpatialObjectPointer>(space, 
                                               index, 
                                               &spatial_object_reference_manager);
    SessionMemory<SpatialObjectPointer> memory;
    loadRandomPoints(spatial_index, &memory);
    runQueries(spatial_index, &memory);
    delete index;
    delete space;
    delete spatial_index;
    deletePoints();
}
