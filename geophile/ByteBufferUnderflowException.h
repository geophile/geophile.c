#ifndef _BYTE_BUFFER_UNDERFLOW_EXCEPTION_H
#define _BYTE_BUFFER_UNDERFLOW_EXCEPTION_H

#include "GeophileException.h"

namespace geophile
{
    class ByteBufferUnderflowException : public GeophileException
    {
    public:
        ByteBufferUnderflowException(const char* message)
            : GeophileException(NULL)
        {}

        ByteBufferUnderflowException(const ByteBufferUnderflowException& e) 
            : GeophileException(e)
        {}
    };
}

#endif
