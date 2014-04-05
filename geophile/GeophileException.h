#ifndef _GEOPHILE_EXCEPTION_H
#define _GEOPHILE_EXCEPTION_H

#include <string.h>
#include <stdlib.h>
#include <exception>

namespace geophile
{
    class GeophileException : public std::exception
    {
    public:
        virtual const char* what() const throw()
        {
            return _message;
        }

        GeophileException(const char* message)
            : _message(message == NULL ? NULL : strdup(message))
        {}

        GeophileException(const GeophileException& e) 
            : _message(e._message == NULL ? NULL : strdup(e._message))
        {}

        virtual ~GeophileException() throw()
        {
            free(_message);
        }

    private:
        char* _message;
    };
}

#endif
