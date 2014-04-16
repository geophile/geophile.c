#ifndef _OUTPUT_ARRAY_BASE_H
#define _OUTPUT_ARRAY_BASE_H

namespace geophile
{
    /*
     * OutputArrayBase provides limimted operations,
     * independent of the SOR type parameter.
     */
    class OutputArrayBase
    {
        friend class SessionMemoryBase;

    public:
        /*
         * Returns the number of SpatialObjects in this OutputArray.
         */
        uint32_t length() const
        {
            return _n;
        }

        /*
         * Clears this OutputArray.
         */
        void clear()
        {
            _n = 0;
        }

    public: // Used internally and in testing
        virtual ~OutputArrayBase()
        {}

        OutputArrayBase()
            : _capacity(INITIAL_CAPACITY),
              _n(0)
        {}

    protected:
        static const uint32_t INITIAL_CAPACITY = 100;

    protected:
        uint32_t _capacity;
        uint32_t _n;
    };
}

#endif
