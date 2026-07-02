/*
 * darwin_stat_shim.h — compile-time shim for the Solaris/glibc stat64
 * API on Darwin.
 *
 * Modern macOS SDKs still ship <sys/stat.h> with stat64, fstat64,
 * lstat64, struct stat64 — but marked deprecated because off_t / ino_t
 * have been 64-bit since 10.5 and struct stat is identical to what
 * used to be struct stat64. Heirloom pkgtools was authored against
 * the Solaris large-file API and reaches for stat64 unconditionally
 * from libinst/ocfile.c and libadm/fulldevnm.c.
 *
 * On Darwin the correct move is: use plain stat() and struct stat.
 * This shim maps stat64->stat etc. so no per-file surgery is needed
 * and Darwin's deprecation warnings do not leak into pkgtools' build.
 *
 * Kept in pkgtools/hdrs/ so any pkgtools translation unit that includes
 * a pkgtools header picks it up transitively. -- Heirloom Darwin port.
 */

#ifndef _HEIRLOOM_DARWIN_STAT_SHIM_H
#define _HEIRLOOM_DARWIN_STAT_SHIM_H

#if defined(__APPLE__)

#include <sys/stat.h>
#include <sys/types.h>
/*
 * Darwin: boolean_t is defined by Mach's <mach/i386/boolean.h> etc.
 * pkglib.h and keystore.h use it but the original code path added
 * its own typedef, which we suppress on Darwin (see keystore.h).
 * Pull the Mach umbrella here so boolean_t resolves in every pkgtools
 * TU that compiles with -include darwin_stat_shim.h. -- Heirloom Darwin
 * port.
 */
#include <mach/mach_types.h>

/*
 * On Darwin, struct stat *is* what struct stat64 used to be — same
 * fields, same widths, same semantics. Re-map the 64-suffixed names
 * so pkgtools compiles unchanged.
 */
#ifndef	HEIRLOOM_DARWIN_STAT_SHIM_APPLIED
#define	HEIRLOOM_DARWIN_STAT_SHIM_APPLIED	1

/*
 * Preprocessor token substitution: 'struct stat64' -> 'struct stat',
 * 'stat64(...)' -> 'stat(...)' etc. Order matters: define the type
 * aliases first, then the functions — no different tokens involved,
 * but keeping the group together aids grep-ability.
 */
#define stat64		stat
#define lstat64		lstat
#define fstat64		fstat
#define fstatat64	fstatat

/*
 * dirent64 → dirent. Same rationale as stat64 → stat: Darwin's plain
 * struct dirent already carries 64-bit d_ino/d_off since 10.6.
 * readdir64/readdir64_r were glibc/Solaris large-file API duplicates.
 */
#include <dirent.h>
#define dirent64	dirent
#define readdir64	readdir
#define readdir64_r	readdir_r

/*
 * <sys/sysmacros.h> — Solaris/glibc header providing MAJOR/MINOR/
 * MAKEDEV as uppercase macros (Solaris) and major/minor/makedev
 * (glibc). Darwin puts the lowercase forms in <sys/types.h>; provide
 * uppercase aliases here so any pkgtools call site that expects the
 * Solaris spelling resolves.
 */
#include <sys/types.h>
#ifndef MAJOR
#define MAJOR(x)	major(x)
#endif
#ifndef MINOR
#define MINOR(x)	minor(x)
#endif
#ifndef MAKEDEV
#define MAKEDEV(x, y)	makedev((x), (y))
#endif

/*
 * statvfs64 → statvfs. Darwin's statvfs already returns 64-bit sizes.
 */
#include <sys/statvfs.h>
#define statvfs64	statvfs
#define fstatvfs64	fstatvfs
/*
 * struct statvfs64 → struct statvfs (token substitution: 'statvfs64'
 * expands to 'statvfs' in any context).
 */

/*
 * O_LARGEFILE — glibc/Solaris flag telling open(2) to treat large files
 * with the *64 API. Darwin's open(2) has transparent 64-bit off_t, so
 * the flag is a no-op there. Define to 0 to keep bitwise-OR expressions
 * (open(path, O_RDONLY | O_LARGEFILE, ...)) valid.
 */
#ifndef O_LARGEFILE
#define O_LARGEFILE	0
#endif

/*
 * <malloc.h> — Linux/Solaris put malloc(3) prototypes here. Darwin puts
 * them in <stdlib.h>. pkgtools' vfpops.c and verify.c #include
 * <malloc.h> unconditionally. Include <stdlib.h> so those files find
 * malloc/free/realloc/calloc regardless.
 */
#include <stdlib.h>

/*
 * ENOPKG — Linux errno 65 "package not installed". Darwin's errno.h
 * does not define it. pkgtools uses it as a semantic sentinel for
 * "requested pkg not present"; pick a value that will not collide
 * with Darwin's errno table (max Darwin errno is around 108).
 */
#include <errno.h>
#ifndef ENOPKG
#define ENOPKG		200	/* Heirloom Darwin port: pkg-not-present sentinel */
#endif
#ifndef ECOMM
#define ECOMM		201	/* Heirloom Darwin port: comm-error sentinel */
#endif

/*
 * fgetpwent/fgetgrent — implemented in libpkg/darwin_pwgr.c.
 * Include the shim header so libpkg/ncgrpw.c sees the redirects.
 */
#include "darwin_pwgr_shim.h"

/*
 * memalign(3) — Linux/Solaris deprecated aligned allocator. Darwin has
 * posix_memalign(3) instead; wrap it so pkgtools' vfpops.c compiles.
 * Semantics: memalign returns a pointer or NULL; posix_memalign returns
 * 0/errno and writes the pointer via out-param. Wrap in an inline
 * helper that mimics the memalign return contract.
 */
static inline void *
heirloom_darwin_memalign(size_t align, size_t size)
{
	void *p;
	if (posix_memalign(&p, align, size) != 0)
		return NULL;
	return p;
}
#ifndef memalign
#define memalign(align, size)	heirloom_darwin_memalign((align), (size))
#endif

#endif	/* HEIRLOOM_DARWIN_STAT_SHIM_APPLIED */

#endif	/* __APPLE__ */

#endif	/* _HEIRLOOM_DARWIN_STAT_SHIM_H */
