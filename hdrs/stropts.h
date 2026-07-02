/*
 * stropts.h — Darwin compile-time stub for the SVR4 STREAMS header.
 *
 * STREAMS was Sun's kernel abstraction for TTY, network, and pipe
 * subsystems. Darwin does not use STREAMS; it inherits BSD sockets
 * and native TTY. pkgtools' getpass.c uses I_PUSH / I_POP ioctls to
 * push the ldterm module, but on Darwin the terminal line discipline
 * is unconditionally in effect — the STREAMS calls are no-ops.
 *
 * Provide minimal token coverage: the constants get 0 values, and
 * ioctl(fd, I_PUSH, "ldterm") at runtime returns -1/ENOTTY on Darwin
 * which the caller silently ignores. -- Heirloom Darwin port.
 */

#ifndef _HEIRLOOM_DARWIN_STROPTS_H
#define _HEIRLOOM_DARWIN_STROPTS_H

#include <sys/ioctl.h>

#ifndef I_PUSH
#define I_PUSH		0	/* unused on Darwin */
#endif
#ifndef I_POP
#define I_POP		0	/* unused on Darwin */
#endif
#ifndef I_LOOK
#define I_LOOK		0
#endif
#ifndef I_FLUSH
#define I_FLUSH		0
#endif

#endif /* _HEIRLOOM_DARWIN_STROPTS_H */
