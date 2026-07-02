/*
 * libintl.h — Darwin compile-time stub for the GNU gettext internationalisation
 * header.
 *
 * Solaris and glibc-based Linux ship <libintl.h> with gettext/dcgettext/
 * dgettext etc. Darwin ships gettext(1) via Homebrew but does not
 * include <libintl.h> in the SDK. pkgtools' puttext.c reaches for it
 * unconditionally.
 *
 * Provide identity macros so pkgtools compiles: gettext(x) returns x,
 * translations are pass-through. English messages ship as-is.
 * -- Heirloom Darwin port.
 */

#ifndef _HEIRLOOM_DARWIN_LIBINTL_H
#define _HEIRLOOM_DARWIN_LIBINTL_H

#define gettext(msgid)			((char *)(msgid))
#define dgettext(dom, msgid)		((char *)(msgid))
#define dcgettext(dom, msgid, cat)	((char *)(msgid))
#define ngettext(sing, plur, n)		((char *)((n) == 1 ? (sing) : (plur)))
#define dngettext(dom, s, p, n)		((char *)((n) == 1 ? (s) : (p)))
#define dcngettext(dom, s, p, n, c)	((char *)((n) == 1 ? (s) : (p)))
#define textdomain(dom)			((char *)(dom))
#define bindtextdomain(dom, dir)	((char *)(dir))
#define bind_textdomain_codeset(d, c)	((char *)(c))

#endif /* _HEIRLOOM_DARWIN_LIBINTL_H */
