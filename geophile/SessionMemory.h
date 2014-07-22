#ifndef _SESSION_MEMORY_H
#define _SESSION_MEMORY_H

#include "OutputArray.h"
#include "SessionMemoryBase.h"

namespace geophile
{
    class RegionPool;
    class Space;
    class SpatialObject;
    class ZArray;

    /*
     * Maintains memory used in various SpatialIndex operations,
     * avoiding repeated allocation/deallocation..
     */
    template <class SOR>
        class SessionMemory : public SessionMemoryBase
    {
    public:
        /*
         * Returns an internally-maintained OutputArray that accumulates output
         * from SpatialIndex retrievals.
         */
        OutputArray<SOR>* output()
        {
            return _output;
        }

        /*
         * Clears the internally-maintained OutputArray returned by output().
         */
        void clearOutput()
        {
            _output->clear();
        }

        /*
         * Destructor
         */
        virtual ~SessionMemory()
        {
            delete _output;
            _output = NULL;
        }

        /*
         * Constructor
         */
        SessionMemory()
            : SessionMemoryBase(),
              _output(new OutputArray<SOR>())
        {}

    private:
        OutputArray<SOR>* _output;
    };
}

#endif
