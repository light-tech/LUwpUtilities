// Common configuration header

#ifndef _LUWPUTILITIES_H
#define _LUWPUTILITIES_H

#ifndef STATIC_INLINE
#define STATIC_INLINE static inline
#endif

#ifdef LUU_EXPORT
namespace LUwpUtilities
{
    LUU_EXPORT delegate void ExceptionHandler(Platform::Exception^ error);
}
#endif

#endif