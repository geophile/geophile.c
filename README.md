# Geophile.C

`Geophile.C` is a C++ implementation of a *spatial index*. A spatial
index organizes a set of spatial objects so that those overlapping a
given spatial object can be found quickly.

This code will eventually be extended to implement *spatial join*
as in the [Java version of Geophile](https://github.com/geophile/geophile).

This implementation is built on two abstractions:

* **Ordered Index:** Any key/value data structure supporting both
random and sequential access. Binary trees, skiplists, and b-trees are
examples of indexes that are compatible with Geophile. Hash tables are
not, because they do not provide for sequential access in key order.

* **Spatial Object:** Any kind of spatial object, in any number of
dimensions. The implementation must provide a method that determines the
relationship of the object to a box: the box is outside the spatial
object; the box is contained by the spatial object; or the box
overlaps the spatial object.

## Installation

`Geophile.C` can be built as follows:

        cmake . -DCMAKE_INSTALL_PREFIX=/usr/local
        make
        sudo make install

Change `/usr/local` to whatever makes sense for your installation. If
you use `/usr/local`, then Geophile header files will go to
`/usr/local/include`, and `libgeophile.so` will go to
`/usr/local/lib`. Depending on where you are installing files, you may
or may not need to run `make install` as root.

## Concepts

In addition to OrderedIndex and Spatial Object, described above,
`Geophile.C` relies on the following concepts.

### Space 

In order to use `Geophile.C`, you must describe the *space* in which spatial
objects reside by giving the number of cells in each dimension, e.g.

        #include <Space.h>
        ...
        using namespace geophile;
        ...
        double lo[] = {0.0, 0.0};
        double hi[] = {1000000.0, 1000000.0};
        uint32_t x_bits[] = {20, 20};
        Space* space = new Space(2, lo, hi, x_bits);

A `Space` describes the space from the application's point of view.
In this case, the space has two dimensions, whose coordinates go from
0 to 1,000,000 in each dimension. The last argument says that Geophile
represents the space internally using a grid with 20 bits of
resolution in each dimension.

The total resolution must never exceed 57. In this case, the total
resolution is 40 (20 + 20).

### Spatial Index

A *spatial index* is layered over a `Space` and an
`OrderedIndex`. This is the interface for adding and removing spatial
objects.

Geophile works by mapping spatial objects into *z-values*, 64-bit
integers which encode spatial information. This encoding of spatial
objects as z-values, and the use of z-values as index keys is managed
by the `SpatialIndex` implementation.

## Example

The source code for this example can be found in
`test/example1.cpp`. The code relies on
the following `Index` and `SpatialObject` classes:

* `geophile::RecordArray`: An `OrderedIndex` subclass that stores
index records in memory.

* `geophile::Point2`: A 2d point.

* `geophile::Box2`: A 2d box.

`example1.cpp` loads 1,000,000 points into a spatial index, and then
runs 5 queries which find the points inside a randomly generated box.
The box size is set so that the average number of points found is
5.

The space is created as follows:

        static int64_t X_MAX = 1000000;
        static int64_t Y_MAX = 1000000;
        ...
        double lo[] = {0.0, 0.0};
        double hi[] = {X_MAX, Y_MAX};
        uint32_t x_bits[] = {10, 10};
        Space* space = new Space(2, lo, hi, x_bits);

An `OrderedIndex` is created, using the `RecordArray` implementation:

        static uint32_t N_POINTS = 1000000;
        ...
        static OrderedIndex* createIndex()
        {
            return new RecordArray(&spatial_object_types, N_POINTS);
        }
        ...
        OrderedIndex* index = createIndex();

`RecordArray` is an example of an `OrderedIndex` implementation, and
is included in the `test` directory. It is not intended for commerical
use, and so is not included in `libgeophile.so`.

A `SpatialIndex` is created:

        SpatialIndex* spatial_index = new SpatialIndex(space, index);

The spatial index is loaded with 1,000,000 points:

        static void loadRandomPoints(SpatialIndex* spatial_index, SessionMemory* memory)
        {
            srand(419);
            int64_t id = 0;
            for (uint32_t id = 0; id < N_POINTS; id) {
                double x = rand() % X_MAX;
                double y = rand() % Y_MAX;
                Point2 point(x, y);
                point.id(id);
                spatial_index->add(&point, memory);
            }
            spatial_index->freeze();
        }

Here is how the points located in a box are found:

        uint32_t x_width = ...;
        uint32_t y_width = x_width;
        ...
            double x_lo = rand() % (X_MAX - x_width);
            double y_lo = rand() % (Y_MAX - y_width);
            double x_hi = x_lo + x_width;
            double y_hi = y_lo + y_width;
            Box2 box(x_lo, x_hi, y_lo, y_hi);
            printf("Query %d: (%f : %f, %f : %f)\\n", q, x_lo, x_hi, y_lo, y_hi);
            PointFilter filter;
            spatial_index->findOverlapping(&box, &filter, memory);
            SpatialObjectArray* output = memory->output();
            // output contains the query result
            memory->clearOutput();
        }

Sample output:

jao@minizack:~/git/geophile.c$ test/example1
Loaded 1000000 points in 1.330791 sec (751432.794481 points/sec)
Query 0: (952116.000000 : 954352.000000, 211272.000000 : 213508.000000)
        (952260.000000, 212945.000000)
        (954052.000000, 211398.000000)
        (953614.000000, 213305.000000)
Query 1: (391437.000000 : 393673.000000, 113014.000000 : 115250.000000)
        (391802.000000, 113657.000000)
        (391862.000000, 114410.000000)
        (391643.000000, 115199.000000)
        (393057.000000, 113716.000000)
        (393176.000000, 113559.000000)
        (393145.000000, 114331.000000)
        (393095.000000, 115029.000000)
Query 2: (380805.000000 : 383041.000000, 604649.000000 : 606885.000000)
        (381388.000000, 606038.000000)
        (381572.000000, 605622.000000)
        (381819.000000, 606221.000000)
        (381362.000000, 605666.000000)
        (381374.000000, 606652.000000)
        (381395.000000, 606793.000000)
        (382180.000000, 605565.000000)
        (381921.000000, 606480.000000)
        (382776.000000, 606690.000000)
        (382384.000000, 606598.000000)
Query 3: (614640.000000 : 616876.000000, 899812.000000 : 902048.000000)
        (614910.000000, 900652.000000)
        (615917.000000, 900303.000000)
        (615650.000000, 900505.000000)
        (615875.000000, 901173.000000)
        (615240.000000, 900683.000000)
        (615290.000000, 901951.000000)
        (616394.000000, 900952.000000)
Query 4: (751870.000000 : 754106.000000, 178593.000000 : 180829.000000)
        (752444.000000, 180001.000000)
        (753559.000000, 180080.000000)
Average points per query: 5.800000
Average query time: 0.077600 msec
