/*
 * sys/sysmacros.h — Darwin compile-time stub for the Solaris/glibc
 * header of the same name.
 *
 * Solaris exposes MAJOR/MINOR/MAKEDEV (uppercase) here; glibc exposes
 * major/minor/makedev (lowercase). Darwin puts the lowercase set in
 * <sys/types.h> and does not ship <sys/sysmacros.h>. pkgtools reaches
 * for this header from libadm/fulldevnm.c and libadm/devtab.c
 * unconditionally.
 *
 * darwin_stat_shim.h (which pkgtools compiles with -include) already
 * provides the MAJOR/MINOR/MAKEDEV macro aliases when needed. This
 * stub simply lets the '#include <sys/sysmacros.h>' line succeed.
 *
 * -- Heirloom Darwin port.
 */

#ifndef _HEIRLOOM_DARWIN_SYS_SYSMACROS_H
#define _HEIRLOOM_DARWIN_SYS_SYSMACROS_H

#include <sys/types.h>

#endif /* _HEIRLOOM_DARWIN_SYS_SYSMACROS_H */
