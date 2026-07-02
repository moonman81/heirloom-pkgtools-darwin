/*
 * darwin_pwgr.c — Darwin implementations of glibc-extension
 * fgetpwent(3) and fgetgrent(3).
 *
 * See hdrs/darwin_pwgr_shim.h for the rationale. These implementations
 * parse one colon-separated /etc/passwd or /etc/group line at a time.
 * Returned structs point into function-local static buffers whose
 * lifetime extends until the next call — same semantics as glibc's
 * fgetpwent/fgetgrent.
 *
 * Not thread-safe (matches glibc's guarantee for the non-_r variants).
 *
 * -- Heirloom Darwin port.
 */

#if defined(__APPLE__)

#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define	HEIRLOOM_PWGR_LINE_MAX	4096
#define	HEIRLOOM_PWGR_GRMEM_MAX	1024

static char *
next_field(char **cursor, char sep)
{
	char *start = *cursor;
	char *p = start;

	if (start == NULL)
		return NULL;
	while (*p != '\0' && *p != sep)
		p++;
	if (*p == sep) {
		*p = '\0';
		*cursor = p + 1;
	} else {
		*cursor = NULL;
	}
	return start;
}

struct passwd *
heirloom_darwin_fgetpwent(FILE *fp)
{
	static char		buf[HEIRLOOM_PWGR_LINE_MAX];
	static struct passwd	pw;
	char			*cursor, *field;

	if (fp == NULL) {
		errno = EINVAL;
		return NULL;
	}

	for (;;) {
		if (fgets(buf, sizeof buf, fp) == NULL)
			return NULL;
		/* Skip comments and blank lines. */
		if (buf[0] == '#' || buf[0] == '\n' || buf[0] == '\0')
			continue;
		break;
	}
	/* Strip trailing newline. */
	buf[strcspn(buf, "\n")] = '\0';

	cursor = buf;
	pw.pw_name = next_field(&cursor, ':');
	pw.pw_passwd = next_field(&cursor, ':');
	field = next_field(&cursor, ':');
	pw.pw_uid = field ? (uid_t)strtoul(field, NULL, 10) : 0;
	field = next_field(&cursor, ':');
	pw.pw_gid = field ? (gid_t)strtoul(field, NULL, 10) : 0;
	pw.pw_gecos = next_field(&cursor, ':');
	pw.pw_dir = next_field(&cursor, ':');
	pw.pw_shell = cursor;	/* remaining tail, may be NULL */
	if (pw.pw_shell == NULL)
		pw.pw_shell = "";
	/* Fields Darwin's struct passwd carries but glibc's does not — zero. */
	pw.pw_change = 0;
	pw.pw_expire = 0;
	pw.pw_class = "";
	return &pw;
}

struct group *
heirloom_darwin_fgetgrent(FILE *fp)
{
	static char		buf[HEIRLOOM_PWGR_LINE_MAX];
	static char		*members[HEIRLOOM_PWGR_GRMEM_MAX];
	static struct group	gr;
	char			*cursor, *field, *memlist;
	size_t			nmembers;

	if (fp == NULL) {
		errno = EINVAL;
		return NULL;
	}

	for (;;) {
		if (fgets(buf, sizeof buf, fp) == NULL)
			return NULL;
		if (buf[0] == '#' || buf[0] == '\n' || buf[0] == '\0')
			continue;
		break;
	}
	buf[strcspn(buf, "\n")] = '\0';

	cursor = buf;
	gr.gr_name = next_field(&cursor, ':');
	gr.gr_passwd = next_field(&cursor, ':');
	field = next_field(&cursor, ':');
	gr.gr_gid = field ? (gid_t)strtoul(field, NULL, 10) : 0;
	memlist = cursor;

	nmembers = 0;
	while (memlist != NULL && *memlist != '\0'
			&& nmembers < HEIRLOOM_PWGR_GRMEM_MAX - 1) {
		members[nmembers++] = next_field(&memlist, ',');
	}
	members[nmembers] = NULL;
	gr.gr_mem = members;
	return &gr;
}

#endif	/* __APPLE__ */
