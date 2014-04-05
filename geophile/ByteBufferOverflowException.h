#ifndef _BYTE_BUFFER_OVERFLOW_EXCEPTION_H
#define _BYTE_BUFFER_OVERFLOW_EXCEPTION_H

#include "GeophileException.h"

namespace geophile
{
    class ByteBufferOverflowException : public GeophileException
    {
    public:
        ByteBufferOverflowException(const char* message)
            : GeophileException(NULL)
        {}

        ByteBufferOverflowException(const ByteBufferOverflowException& e) 
            : GeophileException(e)
        {}
    };
}

#endif
