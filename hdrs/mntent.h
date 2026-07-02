/*
 * mntent.h — Darwin compile-time stub for the glibc/Solaris mount-entry
 * header.
 *
 * Solaris has <sys/mntent.h> and Linux has <mntent.h>; both expose the
 * getmntent(3) family for reading /etc/mtab or /etc/mnttab. Darwin
 * exposes getmntinfo(3) via <sys/mount.h> and does not ship <mntent.h>.
 * pkgtools' libinst/mntinfo.c reaches for <sys/mntent.h> unconditionally
 * — with -I../hdrs (via darwin_stat_shim.h) this file resolves both
 * <mntent.h> and <sys/mntent.h> to the same content.
 *
 * The functional shim (getmntent/setmntent/endmntent) is provided as
 * a no-op on Darwin — pkgtools' code path already handles the failure
 * mode. Full Darwin-native mount-table reading via getmntinfo(3) is
 * scheduled for Phase 6.
 *
 * -- Heirloom Darwin port.
 */

#ifndef _HEIRLOOM_DARWIN_MNTENT_H
#define _HEIRLOOM_DARWIN_MNTENT_H

#include <stdio.h>
#include <sys/mount.h>

#define MOUNTED		"/etc/mtab"
#define MNTTAB		"/etc/mnttab"

/* Common mntent options (glibc/Solaris naming) that pkgtools consumes. */
#ifndef MNTOPT_RO
#define MNTOPT_RO	"ro"
#endif
#ifndef MNTOPT_RW
#define MNTOPT_RW	"rw"
#endif
#ifndef MNTOPT_NOSUID
#define MNTOPT_NOSUID	"nosuid"
#endif
#ifndef MNTOPT_REMOUNT
#define MNTOPT_REMOUNT	"remount"
#endif

/* Minimal shim of struct mntent (Linux layout). */
struct mntent {
	char	*mnt_fsname;
	char	*mnt_dir;
	char	*mnt_type;
	char	*mnt_opts;
	int	mnt_freq;
	int	mnt_passno;
};

/* No-op getmntent family: on Darwin the mount table is walked via
 * getmntinfo(3), not this API. Return NULL/error so callers fall
 * through cleanly to a "no entries" behaviour. */
static inline FILE *setmntent(const char *filename, const char *type) {
	(void)filename; (void)type;
	return NULL;
}
static inline struct mntent *getmntent(FILE *fp) {
	(void)fp;
	return NULL;
}
static inline int endmntent(FILE *fp) {
	(void)fp;
	return 0;
}
static inline int hasmntopt(const struct mntent *m, const char *opt) {
	(void)m; (void)opt;
	return 0;
}

#endif /* _HEIRLOOM_DARWIN_MNTENT_H */
