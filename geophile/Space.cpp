#include "Space.h"
#include "SpatialObject.h"
#include "Region.h"
#include "RegionPool.h"
#include "RegionQueue.h"
#include "SessionMemory.h"
#include "ZArray.h"

using namespace geophile;

int32_t Space::dimensions() const
{
    return _dimensions;
}

double Space::lo(int32_t d) const
{
    return _lo[d];
}

double Space::hi(int32_t d) const
{
    return _hi[d];
}

void Space::decompose(const SpatialObject* spatial_object, 
                      uint32_t max_z,
                      SessionMemoryBase* memory) const
{
    ZArray* zs = memory->zArray();
    RegionPool* regions = memory->regions();
    zs->clear();
    double app_point[_dimensions];
    spatial_object->arbitraryPoint(app_point);
    uint64_t z_point[_dimensions];
    for (uint32_t d = 0; d < _dimensions; d++) {
        z_point[d] = appToZ(d, app_point[d]);
    }
    Region* region = regions->takeRegion();
    region->initialize(this, z_point, z_point, _z_bits);
    while (!spatial_object->containedBy(region)) {
        region->up();
    }
    RegionQueue queue(max_z);
    queue.add(region);
    while (queue.size() > 0) {
        region = queue.take();
        if (region->isPoint()) {
            generateZValueFromRegion(zs, region);
            regions->returnRegion(region);
        } else {
            region->downLeft();
            RegionComparison left_comparison = spatial_object->compare(region);
            region->up();
            region->downRight();
            RegionComparison right_comparison = spatial_object->compare(region);
            switch (left_comparison) {
                case REGION_OUTSIDE_OBJECT:
                    switch (right_comparison) {
                        case REGION_OUTSIDE_OBJECT:
                            GEOPHILE_ASSERT(false);
                            break;
                        case REGION_INSIDE_OBJECT:
                            generateZValueFromRegion(zs, region);
                            regions->returnRegion(region);
                            break;
                        case REGION_OVERLAPS_OBJECT:
                            queue.add(region);
                            break;
                    }
                    break;
                case REGION_INSIDE_OBJECT:
                    switch (right_comparison) {
                        case REGION_OUTSIDE_OBJECT:
                            region->up();
                            region->downLeft();
                            generateZValueFromRegion(zs, region);
                            regions->returnRegion(region);
                            break;
                        case REGION_INSIDE_OBJECT:
                            region->up();
                            generateZValueFromRegion(zs, region);
                            regions->returnRegion(region);
                            break;
                        case REGION_OVERLAPS_OBJECT:
                            if (queue.size() + 1  + zs->length() < max_z) {
                                queue.add(copyRegion(region, regions));
                                region->up();
                                region->downLeft();
                                generateZValueFromRegion(zs, region);
                                regions->returnRegion(region);
                            } else {
                                region->up();
                                generateZValueFromRegion(zs, region);
                                regions->returnRegion(region);
                            }
                            break;
                    }
                    break;
                case REGION_OVERLAPS_OBJECT:
                    switch (right_comparison) {
                        case REGION_OUTSIDE_OBJECT:
                            region->up();
                            region->downLeft();
                            queue.add(region);
                            break;
                        case REGION_INSIDE_OBJECT:
                            if (queue.size() + 1  + zs->length() < max_z) {
                                generateZValueFromRegion(zs, region);
                                region->up();
                                region->downLeft();
                                queue.add(region);
                            } else {
                                region->up();
                                generateZValueFromRegion(zs, region);
                                regions->returnRegion(region);
                            }
                            break;
                        case REGION_OVERLAPS_OBJECT:
                            if (queue.size() + 1 + zs->length() < max_z) {
                                queue.add(copyRegion(region, regions));
                                region->up();
                                region->downLeft();
                                queue.add(region);
                            } else {
                                region->up();
                                generateZValueFromRegion(zs, region);
                                regions->returnRegion(region);
                            }
                            break;
                    }
                    break;
            }
        }
    }
    while (queue.size() > 0) {
        region = queue.take();
        generateZValueFromRegion(zs, region);
        regions->returnRegion(region);
    }
    zs->sort();
    int32_t merge;
    do {
        merge = false;
        for (int i = 1; i < zs->length(); i++) {
            Z a = zs->at(i - 1);
            Z b = zs->at(i);
            if ((merge = a.siblingOf(b))) {
                zs->set(i - 1, a.parent());
                zs->remove(i);
            }
        }
    } while (merge);
}

Z Space::spatialIndexKey(const double* point) const
{
    uint64_t z_point[_dimensions];
    for (uint32_t d = 0; d < _dimensions; d++) {
        z_point[d] = appToZ(d, point[d]);
    }
    return shuffle(z_point, _z_bits);
}

Z Space::shuffle(const uint64_t* x, uint32_t level) const
{
    int64_t z = 0;
    for (uint32_t d = 0; d < _dimensions; d++) {
        int64_t xd = x[d];
        GEOPHILE_ASSERT(xd >= 0);
        switch (_x_bytes[d]) {
            case 8: z |= _shuffle[7][d][(int)(xd >> 56) & 0xff];
            case 7: z |= _shuffle[6][d][(int)(xd >> 48) & 0xff];
            case 6: z |= _shuffle[5][d][(int)(xd >> 40) & 0xff];
            case 5: z |= _shuffle[4][d][(int)(xd >> 32) & 0xff];
            case 4: z |= _shuffle[3][d][(int)(xd >> 24) & 0xff];
            case 3: z |= _shuffle[2][d][(int)(xd >> 16) & 0xff];
            case 2: z |= _shuffle[1][d][(int)(xd >>  8) & 0xff];
            case 1: z |= _shuffle[0][d][(int)(xd      ) & 0xff];
        }
    }
    return Z(z, level == -1 ? _z_bits : level);
}

uint32_t Space::zBits() const
{
    return _z_bits;
}


const uint32_t* Space::interleave() const
{
    return _interleave;
}

int64_t Space::appToZ(uint32_t d, double x) const
{
    return 
        x == _app_hi[d]
        ? (int64_t) _z_range[d] - 1
        : (int64_t) (((x - _app_lo[d]) / _app_width[d]) * _z_range[d]);
}

double Space::zToApp(uint32_t d, uint64_t z) const
{
    return ((double) z / _z_range[d]) * _app_width[d] + _app_lo[d];
}

Space::Space(uint32_t dimensions,
             const double* lo, 
             const double* hi, 
             const uint32_t* x_bits, 
             const uint32_t* interleave)
{
    GEOPHILE_ASSERT(dimensions > 0);
    GEOPHILE_ASSERT(dimensions <= MAX_DIMENSIONS);
    _dimensions = dimensions;
    _lo = new double[dimensions];
    _hi = new double[dimensions];
    _x_bits = new uint32_t[dimensions];
    _x_bytes = new uint32_t[dimensions];
    _app_lo = new double[dimensions];
    _app_hi = new double[dimensions];
    _app_width = new double[dimensions];
    _z_range = new uint64_t[dimensions];
    _z_bits = 0;
    for (uint32_t d = 0; d < dimensions; d++) {
        GEOPHILE_ASSERT(lo[d] < hi[d]);
        _lo[d] = lo[d];
        _hi[d] = hi[d];
        _x_bits[d] = x_bits[d];
        _x_bytes[d] = (x_bits[d] + 7) / 8;
        _app_lo[d] = lo[d];
        _app_hi[d] = hi[d];
        _app_width[d] = hi[d] - lo[d];
        _z_range[d] = 1 << x_bits[d];
        _z_bits += x_bits[d];
    }
    if (interleave == NULL) {
        useDefaultInterleaving();
    } else {
        _interleave = new uint32_t[_z_bits];
        for (uint32_t z_bit_position = 0; z_bit_position < _z_bits; z_bit_position++) {
            _interleave[z_bit_position] = interleave[z_bit_position];
        }
    }
    // Check interleave
    {
        uint32_t count[dimensions];
        for (uint32_t d = 0; d < dimensions; d++) {
            count[d] = 0;
        }
        for (uint32_t z_bit_position = 0; z_bit_position < _z_bits; z_bit_position++) {
            count[_interleave[z_bit_position]]++;
        }
        for (uint32_t d = 0; d < dimensions; d++) {
            GEOPHILE_ASSERT(_x_bits[d] == count[d]);
        }
    }
    computeShuffleMasks();
}

Space::~Space()
{
    delete [] _lo;
    delete [] _hi;
    delete [] _interleave;
    delete [] _x_bits;
    delete [] _x_bytes;
    delete [] _app_lo;
    delete [] _app_hi;
    delete [] _app_width;
    delete [] _z_range;
    for (uint32_t x_byte_position = 0; x_byte_position < 8; x_byte_position++) {
        for (uint32_t d = 0; d < _dimensions; d++) {
            delete [] _shuffle[x_byte_position][d];
        }
        delete [] _shuffle[x_byte_position];
    }
    delete [] _shuffle;
}

void Space::useDefaultInterleaving()
{
    _interleave = new uint32_t[_z_bits];
    uint32_t count[_dimensions];
    for (uint32_t d = 0; d < _dimensions; d++) {
        count[d] = 0;
    }
    int32_t d = -1;
    uint32_t z_position = 0;
    while (z_position < _z_bits) {
        do {
            d = (d + 1) % _dimensions;
        } while (count[d] == _x_bits[d]);
        _interleave[z_position++] = d;
        count[d]++;
    }
}

void Space::computeShuffleMasks()
{
    // z-value format:
    // - Leading bit is zero.
    // - Last 6 bits is the bit count.
    // - Everything in between is a left-justified bitstring. Bits between the bitstring and 
    // length are zero. A bit count of 0 means a 0-length bitstring, covering the entire space. 
    // The maximum bit count is 57, (the number of bits between the leading 0 and the bit count).
    //
    // Shuffling one bit at a time would be slow. The implementation used in spatialIndexKey(double[]) 
    // should be a lot faster, relying on array subscripting to locate masks which are combined 
    // using bitwise OR. The masks are computed here.
    //
    // First, xz, a mapping from x (coordinate) to z bit positions, is computed. xz[d][p] is the position
    // within the z-value of bit p of x[d], (the coordinate of dimension d). For both x and z, bit 
    // positions are numbered left-to-right starting at 0. x values are right-justified, while z-values 
    // are right-justified.
    //
    // Then the shuffle masks are computed. shuffle[b][d][x] is a mask representing the bits of the 
    // bth byte of x[d] that contribute to the z-value.
    uint32_t max_x_bits = 0;
    for (uint32_t d = 0; d < _dimensions; d++) {
        if (_x_bits[d] > max_x_bits) {
            max_x_bits = _x_bits[d];
        }
    }
    uint32_t x_bit_count[_dimensions];
    uint32_t xz[_dimensions][max_x_bits];
    for (uint32_t d = 0; d < _dimensions; d++) {
        x_bit_count[d] = 0;
        for (uint32_t x_bit_position = 0; x_bit_position < max_x_bits; x_bit_position++) {
            xz[d][x_bit_position] = 0;
        }
    }
    for (uint32_t z_bit_position = 0; z_bit_position < _z_bits; z_bit_position++) {
        int d = _interleave[z_bit_position];
        xz[d][x_bit_count[d]] = z_bit_position;
        x_bit_count[d]++;
    }
    _shuffle = new int64_t**[8];
    for (uint32_t x_byte_position = 0; x_byte_position < 8; x_byte_position++) {
        _shuffle[x_byte_position] = new int64_t*[_dimensions];
        for (uint32_t d = 0; d < _dimensions; d++) {
            _shuffle[x_byte_position][d] = new int64_t[256];
            for (uint32_t mask = 0; mask <= 0xff; mask++) {
                _shuffle[x_byte_position][d][mask] = 0;
            }
        }
    }
    for (int d = 0; d < _dimensions; d++) {
        for (uint32_t x_bit_position = 0; x_bit_position < _x_bits[d]; x_bit_position++) {
            int64_t x_mask = 1L << (_x_bits[d] - x_bit_position - 1);
            int64_t z_mask = 1L << (62 - xz[d][x_bit_position]);
            uint32_t x_byte_left_shift = (_x_bits[d] - x_bit_position - 1) / 8;
            for (uint32_t x_byte = 0; x_byte <= 0xff; x_byte++) {
                // x_partial explores all 256 values of one byte of a coordinate. Outside this one byte,
                // everything in x_partial is zero, which is fine for generating shuffle masks.
                int64_t x_partial = ((int64_t) x_byte) << (8 * x_byte_left_shift);
                if ((x_partial & x_mask) != 0) {
                    _shuffle[x_byte_left_shift][d][x_byte] |= z_mask;
                }
            }
        }
    }
}

void Space::generateZValueFromRegion(ZArray* zs, Region* region) const
{
#if 0
    uint64_t rxlo = region->lo(0);
    uint64_t rxhi = region->hi(0);
    uint64_t rylo = region->lo(1);
    uint64_t ryhi = region->hi(1);
    printf("0x%016lx: (%f : %f, %f : %f)\n",
           region->z().asInteger(),
           zToApp(0, rxlo),
           zToApp(0, rxhi),
           zToApp(1, rylo),
           zToApp(1, ryhi));
#endif
    zs->append(region->z());
}

Region* Space::copyRegion(const Region* region, RegionPool* regions) const
{
    Region* copy = regions->takeRegion();
    copy->copyFrom(region);
    return copy;
}
