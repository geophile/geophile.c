#include "Space.h"
#include "Region.h"

using namespace geophile;

const Space* Region::space() const
{
    checkInitialized();
    return _space;
}

uint32_t Region::level() const
{
    checkInitialized();
    return _level;
}

uint64_t Region::lo(uint32_t d) const
{
    checkInitialized();
    return _lo[d];
}

uint64_t Region::hi(uint32_t d) const
{
    checkInitialized();
    return _hi[d];
}

uint32_t Region::isPoint() const
{
    checkInitialized();
    return _level == _space->zBits();
}

void Region::downLeft()
{
    checkInitialized();
    uint32_t d = _interleave[_level];
    _hi[d] &= ~(ONE << --_x_bit_position[d]);
    _level++;
}

void Region::downRight()
{
    checkInitialized();
    uint32_t d = _interleave[_level];
    _lo[d] |= ONE << --_x_bit_position[d];
    _level++;
}

void Region::up()
{
    checkInitialized();
    _level--;
    uint32_t d = _interleave[_level];
    _lo[d] &= ~(ONE << _x_bit_position[d]);
    _hi[d] |= ONE << _x_bit_position[d];
    _x_bit_position[d]++;
}

Z Region::z() const
{
    checkInitialized();
    return _space->shuffle(_lo, _level);
}

void Region::initialize(const Space* space, 
                        const uint64_t* lo, 
                        const uint64_t* hi, 
                        uint32_t level)
{
    _space = space;
    _interleave = space->interleave();
    _level = level;
    memcpy(_lo, lo, space->dimensions() * sizeof(uint64_t));
    memcpy(_hi, hi, space->dimensions() * sizeof(uint64_t));
    for (uint32_t d = 0; d < space->dimensions(); d++) {
        _x_bit_position[d] = 0;
    }
    for (uint32_t z_bit_position = space->zBits() - 1; 
         z_bit_position >= level; 
         z_bit_position--) {
        uint32_t d = _interleave[z_bit_position];
        _x_bit_position[d]++;
    }
}

void Region::copyFrom(const Region* region)
{
    _space = region->_space;
    _interleave = region->_interleave;
    _level = region->_level;
    memcpy(_lo, region->_lo, _space->dimensions() * sizeof(uint64_t));
    memcpy(_hi, region->_hi, _space->dimensions() * sizeof(uint64_t));
    memcpy(_x_bit_position, 
           region->_x_bit_position, 
           _space->dimensions() * sizeof(uint32_t));
}

Region::Region()
{}

void Region::checkInitialized() const
{
    GEOPHILE_ASSERT(_space != NULL);
}
