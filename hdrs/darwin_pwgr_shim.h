/*
 * darwin_pwgr_shim.h — Darwin implementations of glibc-extension
 * fgetpwent(3) and fgetgrent(3).
 *
 * Solaris and glibc both provide fgetpwent/fgetgrent for parsing
 * arbitrary FILE* streams as /etc/passwd or /etc/group. Darwin's
 * pwd(3) / grp(3) subsystems only expose getpwent/getgrent which
 * read from the system database. pkgtools uses the fget* variants
 * in libpkg/ncgrpw.c so it can read passwd/group files rooted
 * elsewhere (an install-root, for example).
 *
 * The implementations here mirror glibc's public behaviour:
 * parse one colon-separated line at a time. Returned structs point
 * into thread-local static buffers. Callers must not free the
 * pointers and must consume/copy fields before the next call.
 *
 * -- Heirloom Darwin port.
 */

#ifndef _HEIRLOOM_DARWIN_PWGR_SHIM_H
#define _HEIRLOOM_DARWIN_PWGR_SHIM_H

#if defined(__APPLE__)

#include <pwd.h>
#include <grp.h>
#include <stdio.h>

/*
 * Provide the prototypes here; the implementations live in
 * libpkg/darwin_pwgr.c (linked into libpkg.a alongside ncgrpw.o).
 */
struct passwd *heirloom_darwin_fgetpwent(FILE *stream);
struct group  *heirloom_darwin_fgetgrent(FILE *stream);

#ifndef fgetpwent
#define fgetpwent(fp)	heirloom_darwin_fgetpwent(fp)
#endif
#ifndef fgetgrent
#define fgetgrent(fp)	heirloom_darwin_fgetgrent(fp)
#endif

#endif	/* __APPLE__ */

#endif /* _HEIRLOOM_DARWIN_PWGR_SHIM_H */
