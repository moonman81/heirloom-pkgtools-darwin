/*
 * darwin_openssl_stubs.c — Darwin-only stub replacements for the four
 * OpenSSL-touching translation units in libpkg:
 *
 *     security.c  — X.509 chain verification
 *     p12lib.c    — PKCS#12 keystore parsing (Sun's sunw_ helpers)
 *     keystore.c  — signed-package trust-store
 *     pkgweb.c    — HTTP/HTTPS web-install transport
 *
 * Rationale
 * =========
 * The Heirloom pkgtools README explicitly documents these as
 * "partially disabled in this variant" upstream:
 *
 *     "Some of the features recently introduced by Sun are partially
 *      disabled in this variant; this applies to SQLite, Solaris
 *      zones, signed packages, and web installation."
 *
 * These four TUs also fail to compile against OpenSSL 3.x (they were
 * written for OpenSSL 1.0.x — DECLARE_STACK_OF etc.), and require
 * broader BSD/Solaris terminal semantics not present on Darwin.
 * Rather than porting them for macOS + OpenSSL 3.x — a significant
 * effort with no downstream consumer on this host — replace the whole
 * OpenSSL-touching set with runtime-failing stubs.
 *
 * The pkgtools code paths that reach these stubs (pkgadd -d web://...,
 * pkgadd with a signed .pkg) will fail with EOPNOTSUPP or similar and
 * a message on stderr. Non-signed / non-web pkgadd/pkgrm still work
 * end-to-end.
 *
 * This is a documented deviation per PORT.md §"Deviations from stock":
 * signed-package + web-install code paths are excluded from the Darwin
 * build. Re-enabling them requires either (a) porting to OpenSSL 3
 * APIs and Darwin PAM, or (b) building against a bundled OpenSSL 1.0.x.
 * Neither is scoped for the current port; task tracked as Phase 6-b.
 *
 * -- Heirloom Darwin port.
 */

#if defined(__APPLE__)

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/*
 * The pkgtools code that consumes these symbols does so through
 * function pointers or via the PKG_ERR* error protocol. Rather than
 * fabricating the full OpenSSL type constellation, provide bare
 * externally-visible symbols with placeholder linkage. Any caller
 * that references one gets a link-time success and a runtime
 * "not supported on this build" complaint.
 *
 * A single 'trap' helper produces the runtime message; every stub
 * calls into it and returns a benign failure code.
 */
static int
heirloom_darwin_stub_trap(const char *sym)
{
	fprintf(stderr,
	    "pkg: '%s' not supported on this Darwin build "
	    "(signed-package/web-install disabled — see PORT.md)\n",
	    sym);
	errno = ENOTSUP;
	return -1;
}

/*
 * security.c symbols
 */
int	sec_init(void)					{ return heirloom_darwin_stub_trap("sec_init"); }
int	get_cert_chain(void *e, void *c, void *cl, void *ca, void *chain)
{
	(void)e; (void)c; (void)cl; (void)ca; (void)chain;
	return heirloom_darwin_stub_trap("get_cert_chain");
}

/*
 * p12lib.c — Sun's sunw_ PKCS12 helpers
 */
int	sunw_PKCS12_parse(void *p, const char *pass, int t, char *k, int kl, char *f, void **pk, void **c, void **ca)
{
	(void)p; (void)pass; (void)t; (void)k; (void)kl; (void)f;
	(void)pk; (void)c; (void)ca;
	return heirloom_darwin_stub_trap("sunw_PKCS12_parse");
}
int	sunw_PKCS12_contents(void *p, const char *pass, void **pk, void **cts)
{
	(void)p; (void)pass; (void)pk; (void)cts;
	return heirloom_darwin_stub_trap("sunw_PKCS12_contents");
}
void	*sunw_PKCS12_create(const char *pass, void *pk, void *cts, void *ca)
{
	(void)pass; (void)pk; (void)cts; (void)ca;
	(void)heirloom_darwin_stub_trap("sunw_PKCS12_create");
	return NULL;
}
void	sunw_evp_pkey_free(void *pkey)	{ (void)pkey; }

/*
 * keystore.c symbols
 */
int	print_certs(void *e, void *h, char *a, void *fp)
{
	(void)e; (void)h; (void)a; (void)fp;
	return heirloom_darwin_stub_trap("print_certs");
}
int	open_keystore(void *e, char *f, char *a, void *cb, unsigned long flg, void **hp)
{
	(void)e; (void)f; (void)a; (void)cb; (void)flg; (void)hp;
	return heirloom_darwin_stub_trap("open_keystore");
}
int	close_keystore(void *e, void *h, void *cb)
{
	(void)e; (void)h; (void)cb;
	return heirloom_darwin_stub_trap("close_keystore");
}

/*
 * pkgweb.c symbols
 */
void	set_passphrase_prompt(char *p)		{ (void)p; }
void	set_passphrase_passarg(char *p)		{ (void)p; }
int	path_valid(char *path)			{ return path && *path == '/'; }
void	web_cleanup(void)			{ /* no-op */ }
int	web_session_control(void *e, char *u, char *d, void *dev, int *retry, int *tries, unsigned int t, void **d2)
{
	(void)e; (void)u; (void)d; (void)dev; (void)retry; (void)tries; (void)t; (void)d2;
	return heirloom_darwin_stub_trap("web_session_control");
}
int	get_signature(void *e, char *ids, void *dev, void **result)
{
	(void)e; (void)ids; (void)dev; (void)result;
	return heirloom_darwin_stub_trap("get_signature");
}
void	set_web_install(void)			{ /* no-op */ }

#endif	/* __APPLE__ */
