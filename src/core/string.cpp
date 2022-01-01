
//#include <cstdio>
//
//#ifdef WIN32
//#   define WIN32_LEAN_AND_MEAN
//#   include <windows.h>
//#endif
//
//#include <cstdarg>
//
//#include "core.h"

#include "../bigfx.h"

namespace bigfx
{

String String::Printf(char const *format, ...)
{
    String ret;

    va_list ap;
    va_start(ap, format);
    ret = vaPrintf(format, ap);
    va_end(ap);

    return ret;
}

String String::vaPrintf(char const *format, va_list ap)
{
    String ret;

    va_list ap2;
    va_copy(ap2, ap);

    /* vsnprintf() tells us how many character we need, and we need to
     * add one for the terminating null byte. */
    int needed = vsnprintf(nullptr, 0, format, ap2) + 1;
    va_end(ap2);
    va_copy(ap2, ap);

    ((Super &)ret).reserve(needed);
    ret.m_count = needed;

    vsnprintf(&ret[0], needed, format, ap);
    va_end(ap2);

    return ret;
}

} /* namespace bigfx */
