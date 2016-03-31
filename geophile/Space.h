#ifndef _SPACE_H
#define _SPACE_H

#include <stdint.h>
#include <stddef.h>
#include "Z.h"

namespace geophile
{
    class SessionMemoryBase;
    class SpatialObject;
    class Region;
    class RegionPool;
    class ZArray;

    /*
     * A Space represents the space in which SpatialObjects
     * reside. The lower bound of each dimension is zero, and the
     * upper bounds are given when the Space is created. Conceptually,
     * a Space is a multi-dimensional grid of cells.
     */
    class Space
    {
    public:
        static const uint32_t MAX_DIMENSIONS = 6;

        /*
         * The number of dimensions of this Space.
         */
        int32_t dimensions() const;

        /*
         * The low coordinate of dimension d.
         */
        double lo(int32_t d) const;

        /*
         * The high coordinate of dimension d.
         */
        double hi(int32_t d) const;

        /*
         * Decompose spatial_object into z-values, stored in
         * memory->zArray().  The maximum number of z-values is
         * max_z. If fewer are needed, then the unused zArray
         * positions are denoted by -1 at the end of the array.
         */
        void decompose(const SpatialObject* spatial_object, 
                       uint32_t max_z,
                       SessionMemoryBase* memory) const;

        /*
         * Returns a coordinate in the Z space, right-justified, not a z-value.
         */
        int64_t appToZ(uint32_t d, double x) const;

        /*
         * Destructor
         */
        ~Space();

        /*
         * Constructor.
         * dimensions: Number of dimensions of the space.
         * lo: Low coordinates of the space in an array of size dimensions.
         * hi: High coordinates of the space in an array of size dimensions.
         * x_bits: Specifies, for the Z space, the number of bits in each dimension.
         * interleave: Specifies the how bits of coordinates are interleaved. 
         *     0 <= interleave[i] < 2**x_bits[d], 0 <= d < 2**x_bits.length. The more bits that are 
         *     present for a given dimension earlier in the interleaving, the more the spatial 
         *     index will be optimized for selectivity in that dimension.
         */
        Space(uint32_t dimensions,
              const double* lo, 
              const double* hi, 
              const uint32_t* x_bits, 
              const uint32_t* interleave = NULL);

    public: // Not part of the API. Public for testing.
        Z shuffle(const uint64_t* x, uint32_t level = -1) const;
        uint32_t zBits() const;
        const uint32_t* interleave() const;
        // z is a right-justified coordinate in the Z space, not a z-value.
        double zToApp(uint32_t d, uint64_t z) const;

    private:
        // Returns the z-value for a point in the application space.
        Z spatialIndexKey(const double* point) const;
        void useDefaultInterleaving();
        void computeShuffleMasks();
        void generateZValueFromRegion(ZArray* zs, Region* region) const;
        Region* copyRegion(const Region* region, RegionPool* regions) const;

    private:
        // Application space
        double* _lo;
        double* _hi;
        // The 'x' prefix refers to coordinates. The 'z' prefix refers to z values.
        uint32_t _dimensions;
        uint32_t* _interleave;
        uint32_t* _x_bits;
        uint32_t* _x_bytes;
        uint32_t _z_bits;
        // Translation to/from application space
        double* _app_lo;
        double* _app_hi;
        double* _app_width;
        uint64_t* _z_range;
        // For shuffling
        int64_t*** _shuffle;
    };
}

#endif
