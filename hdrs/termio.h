/*
 * termio.h — Darwin compile-time stub for the SVR3 termio header.
 *
 * termio was the pre-POSIX SVR3 terminal I/O API. termios (POSIX)
 * superseded it in the late 1980s. Darwin ships only <termios.h>;
 * pkgtools' getpass.c still reaches for <termio.h> unconditionally.
 *
 * The struct termio and struct termios layouts are similar enough
 * that consumers of the c_lflag / ICANON / ECHO bits work identically
 * after re-mapping to termios equivalents. Provide the POSIX header
 * here so #include <termio.h> succeeds. -- Heirloom Darwin port.
 */

#ifndef _HEIRLOOM_DARWIN_TERMIO_H
#define _HEIRLOOM_DARWIN_TERMIO_H

#include <termios.h>

/*
 * Solaris' TCGETA/TCSETA/TCSETAW/TCSETAF ioctls (used with struct termio)
 * map onto POSIX tcgetattr/tcsetattr for the (struct termios *) form.
 * getpass.c uses these through ioctl(); the pkgtools code path already
 * handles ENOTTY gracefully.
 */
#ifndef TCGETA
#define TCGETA		TIOCGETA
#endif
#ifndef TCSETA
#define TCSETA		TIOCSETA
#endif
#ifndef TCSETAW
#define TCSETAW		TIOCSETAW
#endif
#ifndef TCSETAF
#define TCSETAF		TIOCSETAF
#endif

/* Alias struct termio to struct termios — layouts are compatible for the
 * fields getpass.c cares about (c_lflag, c_cc). */
#define termio		termios

#endif /* _HEIRLOOM_DARWIN_TERMIO_H */
