#ifndef _CURSOR_H
#define _CURSOR_H

#include "Record.h"

namespace geophile
{
    typedef enum 
    {
        // The cursor has been created, but has never been used to retrieve a record. If the key used 
        // to create the cursor is present, then both next() and previous() will retrieve the 
        // associated record. This state is also used when a cursor is repositioned using goTo().
        NEVER_USED,

        // The cursor has been created and used to retrieve at least one record. next() and 
        // previous() move the cursor before retrieving a record.
        IN_USE,

        // The cursor has run off one end. A call to next() or previous() will return null.
        DONE
    } CursorState;

    class Cursor
    {
    public:

        virtual Record next() = 0;

        virtual Record previous() = 0;

        virtual void goTo(const SpatialObjectKey& key) = 0;

        virtual void close()
        {
            _current.setEOF();
            _state = DONE;
        }

        ~Cursor()
        {}

        const Record& current()
        {
            return _current;
        }

    protected:

        void current(Z z, SpatialObject* spatial_object)
        {
            _current.set(z, spatial_object);
        }

        void current(const Record& record)
        {
            _current = record;
        }

        CursorState state()
        {
            return _state;
        }

        void state(CursorState newState)
        {
            _state = newState;
        }

        Cursor()
            : _state(NEVER_USED)
        {}

    private:
        Record _current;
        CursorState _state;
    };
}

#endif
