/*
 * malloc.h — Darwin compile-time stub for the Solaris/Linux malloc
 * header.
 *
 * Solaris and glibc-based Linux ship <malloc.h> with malloc/free/
 * realloc/calloc prototypes plus vendor extensions (mallinfo, mallopt).
 * Darwin puts the POSIX subset in <stdlib.h> and has no separate
 * <malloc.h> — reaching for it fails at preprocessor time. Since
 * pkgtools only uses the POSIX subset, delegate to <stdlib.h>.
 * -- Heirloom Darwin port.
 */

#ifndef _HEIRLOOM_DARWIN_MALLOC_H
#define _HEIRLOOM_DARWIN_MALLOC_H

#include <stdlib.h>

#endif /* _HEIRLOOM_DARWIN_MALLOC_H */
