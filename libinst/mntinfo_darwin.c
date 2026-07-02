/*
 * mntinfo_darwin.c — Darwin-native replacement for OpenSolaris mntinfo.c.
 *
 * The upstream mntinfo.c reads /etc/mnttab via getmntent(3) on Solaris
 * and /proc/mounts via a getmntent shim on Linux. Neither approach is
 * available on Darwin: macOS has no mnttab file, and getmntent is a
 * glibc/Solaris extension. Darwin's native mount-table walker is
 * getmntinfo(3), which returns an array of struct statfs — richer than
 * mntent, and always in sync with the running kernel.
 *
 * This TU reproduces the public API surface of the OpenSolaris
 * mntinfo.c so that pkgtools' libinst / libpkg / pkgcmds consumers
 * link and run without further Darwin-awareness. Semantics are
 * preserved where equivalent macOS concepts exist; where Solaris
 * concepts (client mounts, srvr_map, cachefs) don't map, this
 * implementation returns "no such feature" cleanly rather than
 * fabricating behaviour.
 *
 * Consumers (pkgadd, pkgrm, pkgchk, pkginstall, dryrun, pkgobjmap,
 * installf, is_local_host) that need only filesystem-size checks
 * and mount-point resolution work end-to-end. Consumers that would
 * push nfs mounts into a client alternate-root return a documented
 * "unsupported on this build" indication.
 *
 * -- Heirloom Darwin port (Phase 5-C1).
 */

#if defined(__APPLE__)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <netdb.h>

#include <pkgstrct.h>
#include <pkginfo.h>
#include <pkglib.h>
#include "install.h"
#include "libinst.h"
#include "libadm.h"

/*
 * fs table — external globals matching OpenSolaris signatures.
 * Owned here. Populated lazily by ensure_fs_tab().
 */
int		fs_tab_used  = 0;
int		fs_tab_alloc = 0;
struct fstable	**fs_tab     = NULL;

/*
 * Populate the internal fs_tab from Darwin's getmntinfo(3). Called
 * lazily on first query. Idempotent.
 */
static int
ensure_fs_tab(void)
{
	struct statfs	*mnts;
	int		nmnts, i;
	struct fstable	*entry;

	if (fs_tab_used > 0)
		return 0;	/* already populated */

	nmnts = getmntinfo(&mnts, MNT_NOWAIT);
	if (nmnts <= 0)
		return -1;

	fs_tab = calloc((size_t)nmnts, sizeof(struct fstable *));
	if (fs_tab == NULL)
		return -1;
	fs_tab_alloc = nmnts;

	for (i = 0; i < nmnts; i++) {
		entry = calloc(1, sizeof(struct fstable));
		if (entry == NULL)
			return -1;

		entry->name    = strdup(mnts[i].f_mntonname);
		entry->namlen  = (int)strlen(entry->name);
		entry->fstype  = strdup(mnts[i].f_fstypename);
		entry->bsize   = (fsblkcnt_t)mnts[i].f_bsize;
		entry->frsize  = (fsblkcnt_t)mnts[i].f_bsize;
		entry->bfree   = (fsblkcnt_t)mnts[i].f_bavail;
		entry->bused   = (fsblkcnt_t)(mnts[i].f_blocks - mnts[i].f_bfree);
		entry->ffree   = (fsblkcnt_t)mnts[i].f_ffree;
		entry->fused   = (fsblkcnt_t)(mnts[i].f_files - mnts[i].f_ffree);
		entry->writeable    = (mnts[i].f_flags & MNT_RDONLY) ? 0 : 1;
		entry->write_tested = 1;
		entry->mounted      = 1;
		/*
		 * Remote if f_fstypename is nfs / smbfs / afpfs / webdav.
		 * f_mntfromname carries "host:/path" for nfs; use it as
		 * remote_name for compatibility with pkgobjmap.c.
		 */
		if (strncmp(mnts[i].f_fstypename, "nfs", 3) == 0 ||
		    strncmp(mnts[i].f_fstypename, "smbfs", 5) == 0 ||
		    strncmp(mnts[i].f_fstypename, "afpfs", 5) == 0 ||
		    strncmp(mnts[i].f_fstypename, "webdav", 6) == 0) {
			entry->remote      = 1;
			entry->served      = 1;
			entry->remote_name = strdup(mnts[i].f_mntfromname);
		} else {
			entry->remote      = 0;
			entry->served      = 0;
			entry->remote_name = NULL;
		}
		entry->srvr_map   = 0;
		entry->cl_mounted = 0;
		entry->mnt_failed = 0;

		fs_tab[i] = entry;
	}
	fs_tab_used = nmnts;
	return 0;
}

/*
 * Return the fstable entry that is the longest mount-point prefix of
 * `path`. Populates *fsys_value with its index if non-NULL.
 */
static struct fstable *
find_fs_for_path(const char *path, short *fsys_value)
{
	int	i, best = -1;
	size_t	best_len = 0, path_len;

	if (ensure_fs_tab() < 0 || path == NULL)
		return NULL;

	path_len = strlen(path);
	for (i = 0; i < fs_tab_used; i++) {
		size_t nlen = (size_t)fs_tab[i]->namlen;
		if (nlen > path_len)
			continue;
		if (strncmp(fs_tab[i]->name, path, nlen) != 0)
			continue;
		/* full match, or match followed by '/', or root '/' */
		if (path[nlen] == '\0' || path[nlen] == '/' || nlen == 1) {
			if (nlen > best_len) {
				best     = i;
				best_len = nlen;
			}
		}
	}
	if (best < 0)
		return NULL;
	if (fsys_value)
		*fsys_value = (short)best;
	return fs_tab[best];
}

/* ---------------- indexed getters ---------------- */

struct fstable *
get_fs_entry(short n)
{
	if (ensure_fs_tab() < 0 || n < 0 || n >= fs_tab_used)
		return NULL;
	return fs_tab[n];
}

fsblkcnt_t
get_blk_size_n(short n)
{
	struct fstable *e = get_fs_entry(n);
	return e ? e->bsize : 0;
}

fsblkcnt_t
get_frag_size_n(short n)
{
	struct fstable *e = get_fs_entry(n);
	return e ? e->frsize : 0;
}

fsblkcnt_t
get_blk_free_n(short n)
{
	struct fstable *e = get_fs_entry(n);
	return e ? e->bfree : 0;
}

fsblkcnt_t
get_blk_used_n(short n)
{
	struct fstable *e = get_fs_entry(n);
	return e ? e->bused : 0;
}

void
set_blk_used_n(short n, fsblkcnt_t value)
{
	struct fstable *e = get_fs_entry(n);
	if (e)
		e->bused = value;
}

fsblkcnt_t
get_inode_free_n(short n)
{
	struct fstable *e = get_fs_entry(n);
	return e ? e->ffree : 0;
}

fsblkcnt_t
get_inode_used_n(short n)
{
	struct fstable *e = get_fs_entry(n);
	return e ? e->fused : 0;
}

char *
get_source_name_n(short n)
{
	struct fstable *e = get_fs_entry(n);
	return (e && e->remote_name) ? e->remote_name : (e ? e->name : NULL);
}

char *
get_fs_name_n(short n)
{
	struct fstable *e = get_fs_entry(n);
	return e ? e->name : NULL;
}

int
is_remote_fs_n(short n)
{
	struct fstable *e = get_fs_entry(n);
	return e ? (int)e->remote : 0;
}

int
is_served_n(short n)
{
	struct fstable *e = get_fs_entry(n);
	return e ? (int)e->served : 0;
}

int
is_mounted_n(short n)
{
	struct fstable *e = get_fs_entry(n);
	return e ? (int)e->mounted : 0;
}

int
is_fs_writeable_n(short n)
{
	struct fstable *e = get_fs_entry(n);
	return e ? (int)e->writeable : 0;
}

/* ---------------- path-based queries ---------------- */

int
is_remote_fs(char *path, short *fsys_value)
{
	struct fstable *e = find_fs_for_path(path, fsys_value);
	return e ? (int)e->remote : 0;
}

int
is_served(char *path, short *fsys_value)
{
	struct fstable *e = find_fs_for_path(path, fsys_value);
	return e ? (int)e->served : 0;
}

int
is_mounted(char *path, short *fsys_value)
{
	struct fstable *e = find_fs_for_path(path, fsys_value);
	return e ? (int)e->mounted : 0;
}

int
is_fs_writeable(char *path, short *fsys_value)
{
	struct fstable *e = find_fs_for_path(path, fsys_value);
	return e ? (int)e->writeable : 0;
}

/* ---------------- table maintenance ---------------- */

int
get_mntinfo(int map_client, char *vfstab_file)
{
	(void)map_client;
	(void)vfstab_file;
	/*
	 * OpenSolaris used this to (re-)populate the fs table optionally
	 * mounting anything from /etc/vfstab. Darwin has no vfstab; we
	 * simply ensure the table is current with getmntinfo(3). Return
	 * 0 on success matching upstream contract.
	 */
	return ensure_fs_tab();
}

int
load_fsentry(struct fstable *fs_entry, char *name, char *fstype,
    char *remote_name)
{
	if (fs_entry == NULL)
		return 1;
	fs_entry->name        = name ? strdup(name) : NULL;
	fs_entry->namlen      = name ? (int)strlen(name) : 0;
	fs_entry->fstype      = fstype ? strdup(fstype) : NULL;
	fs_entry->remote_name = remote_name ? strdup(remote_name) : NULL;
	return 0;
}

void
fs_tab_free(void)
{
	int i;
	if (fs_tab == NULL)
		return;
	for (i = 0; i < fs_tab_used; i++) {
		if (fs_tab[i] == NULL)
			continue;
		free(fs_tab[i]->name);
		free(fs_tab[i]->fstype);
		free(fs_tab[i]->remote_name);
		free(fs_tab[i]);
	}
	free(fs_tab);
	fs_tab       = NULL;
	fs_tab_used  = 0;
	fs_tab_alloc = 0;
}

int
fs_tab_init(char *root_path, char *vfstab_file)
{
	(void)root_path;
	(void)vfstab_file;
	return ensure_fs_tab();
}

/* ---------------- Solaris-specific concepts: return "unsupported" ---------------- */

/*
 * Client mounts, server maps, cachefs — none of these have Darwin
 * equivalents. Return failure sentinels; consumers already handle
 * "no such client mount" as a legitimate outcome.
 */
int mount_client(void) { return -1; }
int unmount_client(void) { return -1; }

int use_srvr_map_n(short n)
{ (void)n; return 0; }

int use_srvr_map(char *path, short *fsys_value)
{ (void)path; (void)fsys_value; return 0; }

char *server_map(char *client_path, short client_fsys)
{ (void)client_path; (void)client_fsys; return NULL; }

int is_remote_src(char *source)
{
	/*
	 * "host:/path" is the Solaris + Darwin nfs syntax; detect the
	 * colon-then-slash pattern.
	 */
	char *c;
	if (source == NULL) return 0;
	if ((c = strchr(source, ':')) != NULL && c[1] == '/')
		return 1;
	return 0;
}

int is_local_host(char *hostname)
{
	char lbuf[_POSIX_HOST_NAME_MAX + 1];
	if (hostname == NULL || *hostname == '\0')
		return 1;
	if (gethostname(lbuf, sizeof lbuf) == 0
	    && strcmp(lbuf, hostname) == 0)
		return 1;
	if (strcmp(hostname, "localhost") == 0)
		return 1;
	return 0;
}

char *get_mount_point(short fsys)
{
	struct fstable *e = get_fs_entry(fsys);
	return e ? e->name : NULL;
}

char *get_remote_path(short fsys)
{
	struct fstable *e = get_fs_entry(fsys);
	return (e && e->remote_name) ? e->remote_name : NULL;
}

char *get_server_host(short fsys)
{
	struct fstable *e = get_fs_entry(fsys);
	char *rn, *c;
	static char host_buf[_POSIX_HOST_NAME_MAX + 1];

	if (e == NULL || (rn = e->remote_name) == NULL)
		return NULL;
	c = strchr(rn, ':');
	if (c == NULL)
		return NULL;
	{
		size_t len = (size_t)(c - rn);
		if (len >= sizeof host_buf)
			len = sizeof host_buf - 1;
		memcpy(host_buf, rn, len);
		host_buf[len] = '\0';
	}
	return host_buf;
}

int hasopt(char *option_list, char *option)
{
	/*
	 * Test whether a comma-separated option string contains `option`.
	 * Matches the OpenSolaris semantics for mount-option parsing.
	 */
	char *p, *end;
	size_t opt_len;
	if (option_list == NULL || option == NULL)
		return 0;
	opt_len = strlen(option);
	p = option_list;
	while (p != NULL && *p != '\0') {
		end = strchr(p, ',');
		size_t seg_len = end ? (size_t)(end - p) : strlen(p);
		if (seg_len == opt_len && strncmp(p, option, opt_len) == 0)
			return 1;
		p = end ? end + 1 : NULL;
	}
	return 0;
}

/* Legacy internal helpers: leave as no-ops. */
int  already_mounted(char *mp)                            { (void)mp; return 0; }
void construct_mt(struct fstable *e)                      { (void)e; }
void construct_vfs(struct fstable *e)                     { (void)e; }
int  fs_tab_ent_comp(const void *a, const void *b)
{
	const struct fstable *fa = *(const struct fstable * const *)a;
	const struct fstable *fb = *(const struct fstable * const *)b;
	return strcmp(fa->name, fb->name);
}
short resolved_fsys(char *path)
{
	short fsys_idx = -1;
	(void)find_fs_for_path(path, &fsys_idx);
	return fsys_idx;
}

/*
 * fsys() — return the fs_tab index for the mount point that owns
 * `path`. Matches OpenSolaris semantics: returns -1 if not found.
 */
short fsys(char *path)
{
	short fsys_idx = -1;
	(void)find_fs_for_path(path, &fsys_idx);
	return fsys_idx;
}
char *path_part(char *fsys)                               { return fsys; }
int  really_write(char *fsys)                             { (void)fsys; return 1; }
int  mod_existing(char *path, char *fstype)               { (void)path; (void)fstype; return 0; }

#endif	/* __APPLE__ */
