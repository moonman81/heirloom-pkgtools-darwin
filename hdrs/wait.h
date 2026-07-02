/*
 * wait.h — Darwin compile-time stub for the Solaris/glibc header.
 *
 * Solaris and glibc-based Linux ship <wait.h> which wraps <sys/wait.h>.
 * Darwin (and other BSDs) only ship <sys/wait.h>. Delegate.
 * -- Heirloom Darwin port.
 */

#ifndef _HEIRLOOM_DARWIN_WAIT_H
#define _HEIRLOOM_DARWIN_WAIT_H

#include <sys/wait.h>

#endif /* _HEIRLOOM_DARWIN_WAIT_H */
