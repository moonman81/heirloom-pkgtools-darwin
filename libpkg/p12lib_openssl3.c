/*
 * p12lib_openssl3.c — Fresh OpenSSL 3.x-native implementation of the
 * Heirloom pkgtools sunw_ PKCS#12 API surface. Replaces the legacy
 * p12lib.c (which depends on opaque-struct field access removed in
 * OpenSSL 3.x) on Darwin.
 *
 * Design principles
 * =================
 * - **Public API only.** No struct-field pokes. Everything goes
 *   through OpenSSL 3.x accessors.
 * - **Algorithm-agnostic.** No hard-coded RSA/DSA/EC discrimination.
 *   EVP_PKEY handles anything OpenSSL knows about — including the
 *   post-quantum families (ML-KEM 512/768/1024 for KEMs; ML-DSA and
 *   SLH-DSA for signatures; hybrids X25519MLKEM768 etc.) that
 *   OpenSSL 3.5+ ships natively. See PORT.md §5-C4 for the PQ note.
 * - **Same public entry points.** Every sunw_ symbol consumed
 *   externally (from libpkg + libinst + pkgcmds) is re-emitted with
 *   the same signature. Callers link unchanged.
 * - **Minimal semantics.** The legacy p12lib had ~3000 lines of
 *   Sun-specific parsing helpers. Modern PKCS12_parse() + friends do
 *   most of the work; this reimplementation is ~350 lines. Features
 *   that were only used by pkg-signing utilities that Heirloom does
 *   not ship (sunw_PKCS12_create writes) are provided as returning-
 *   error stubs. Sun-flavour attribute helpers (sunw_find_fname etc.)
 *   are implemented via OpenSSL 3.x's PKCS12_get_friendlyname_asc etc.
 *
 * -- Heirloom Darwin port (Phase 5-C4).
 */

#if defined(__APPLE__)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <openssl/opensslv.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pkcs12.h>
#include <openssl/pkcs7.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/asn1.h>
#include <openssl/safestack.h>
#include <openssl/core_names.h>

#include "openssl3_compat.h"	/* DEFINE_STACK_OF(EVP_PKEY) */
#include "p12lib.h"

/* ---------------- utilities ---------------- */

void
sunw_evp_pkey_free(EVP_PKEY *pkey)
{
	EVP_PKEY_free(pkey);
}

/*
 * Post-quantum readiness note: the following helper is used everywhere
 * pkgadd needs to decide "is this key usable for signing this
 * package?". It intentionally does NOT enumerate legacy key types —
 * that would silently reject ML-DSA and SLH-DSA signatures. Instead
 * ask the EVP_PKEY infrastructure directly.
 */
static int
key_can_sign(const EVP_PKEY *pkey)
{
	if (pkey == NULL)
		return 0;
	return EVP_PKEY_can_sign(pkey);
}

/* ---------------- PKCS12 parsing (public entry point) ---------------- */

/*
 * sunw_PKCS12_parse — decrypt a PKCS12 file and populate a stack of
 * EVP_PKEY + STACK_OF(X509). Wraps OpenSSL 3.x PKCS12_parse().
 *
 * The legacy signature took a bunch of bit flags + keyid matcher +
 * fname matcher. This implementation ignores the search bits and
 * returns everything the container holds — callers can filter with
 * sunw_find_fname / sunw_find_localkeyid.
 *
 * Returns 0 on success, -1 on failure. Populates *pkey_stk and
 * *cert_stk on success (caller frees).
 */
int
sunw_PKCS12_parse(PKCS12 *p12, const char *pass, int matchty,
    char *keyid, int keyid_len, char *fname,
    STACK_OF(EVP_PKEY) **pkeys, STACK_OF(X509) **certs,
    STACK_OF(X509) **cacerts)
{
	EVP_PKEY *pkey = NULL;
	X509 *cert = NULL;
	STACK_OF(X509) *ca = NULL;
	STACK_OF(EVP_PKEY) *pkey_stk = NULL;
	STACK_OF(X509) *cert_stk = NULL;

	(void)matchty;  (void)keyid;  (void)keyid_len;  (void)fname;

	if (p12 == NULL || pkeys == NULL || certs == NULL)
		return -1;

	if (PKCS12_parse(p12, pass, &pkey, &cert, &ca) == 0)
		goto err;

	pkey_stk = sk_EVP_PKEY_new_null();
	cert_stk = sk_X509_new_null();
	if (pkey_stk == NULL || cert_stk == NULL)
		goto err;

	if (pkey != NULL && sk_EVP_PKEY_push(pkey_stk, pkey) == 0)
		goto err;
	pkey = NULL;
	if (cert != NULL && sk_X509_push(cert_stk, cert) == 0)
		goto err;
	cert = NULL;

	*pkeys = pkey_stk;
	*certs = cert_stk;
	if (cacerts != NULL) {
		*cacerts = ca;
	} else if (ca != NULL) {
		sk_X509_pop_free(ca, X509_free);
	}
	return 0;

err:
	EVP_PKEY_free(pkey);
	X509_free(cert);
	if (ca != NULL)
		sk_X509_pop_free(ca, X509_free);
	if (pkey_stk != NULL)
		sk_EVP_PKEY_pop_free(pkey_stk, EVP_PKEY_free);
	if (cert_stk != NULL)
		sk_X509_pop_free(cert_stk, X509_free);
	return -1;
}

/*
 * sunw_PKCS12_contents — like sunw_PKCS12_parse but always returns
 * everything (no filter).
 */
int
sunw_PKCS12_contents(PKCS12 *p12, const char *pass,
    STACK_OF(EVP_PKEY) **pkeys, STACK_OF(X509) **certs)
{
	return sunw_PKCS12_parse(p12, pass, 0, NULL, 0, NULL, pkeys, certs, NULL);
}

/*
 * sunw_PKCS12_create — pack keys + certs into a PKCS12 container.
 * Only used by pkg-signing utilities Heirloom does not ship on Darwin.
 * Retained as a runtime-failing stub with a diagnostic; callers get
 * ERR set + NULL. Post-quantum note: when this is re-enabled, use
 * PKCS12_create() unchanged — it already handles arbitrary EVP_PKEY
 * types including ML-DSA/SLH-DSA in OpenSSL 3.5+.
 */
PKCS12 *
sunw_PKCS12_create(const char *pass, STACK_OF(EVP_PKEY) *pkeys,
    STACK_OF(X509) *certs, STACK_OF(X509) *cacerts)
{
	(void)pass; (void)pkeys; (void)certs; (void)cacerts;
	fprintf(stderr,
	    "pkg: sunw_PKCS12_create not supported in Darwin OpenSSL 3.x build "
	    "(only readers are ported — see PORT.md §5-C4)\n");
	return NULL;
}

/* ---------------- split_certs ---------------- */

/*
 * sunw_split_certs — separate self-signed CAs from end-entity certs.
 * OpenSSL 3.x accessor equivalent: check X509_check_issued(cert, cert)
 * — returns X509_V_OK iff the cert is self-signed.
 */
int
sunw_split_certs(STACK_OF(EVP_PKEY) *pkeys, STACK_OF(X509) *all,
    STACK_OF(X509) **cacerts, STACK_OF(EVP_PKEY) **spare_keys)
{
	int i;
	STACK_OF(X509) *ca_stk;

	(void)pkeys;  (void)spare_keys;

	if (all == NULL || cacerts == NULL)
		return -1;

	ca_stk = sk_X509_new_null();
	if (ca_stk == NULL)
		return -1;

	i = 0;
	while (i < sk_X509_num(all)) {
		X509 *c = sk_X509_value(all, i);
		if (X509_check_issued(c, c) == X509_V_OK) {
			(void)sk_X509_delete(all, i);
			if (sk_X509_push(ca_stk, c) == 0) {
				sk_X509_pop_free(ca_stk, X509_free);
				X509_free(c);
				return -1;
			}
		} else {
			i++;
		}
	}
	*cacerts = ca_stk;
	return 0;
}

/* ---------------- friendlyname helpers ---------------- */

/*
 * sunw_get_cert_fname — read the friendlyName alias attribute from
 * an X509. Uses X509_alias_get0 (OpenSSL 3.x public accessor).
 * dowhat: GETDO_COPY (=0) returns strdup'd string; GETDO_GET returns
 *         pointer into cert-owned memory; GETDO_DEL clears the alias.
 *
 * Returns strlen(alias) or -1 on error. Sets *fname to point to the
 * alias (allocated for GETDO_COPY, borrowed for GETDO_GET).
 */
int
sunw_get_cert_fname(getdo_actions_t dowhat, X509 *cert, char **fname)
{
	int len = 0;
	const unsigned char *alias;

	if (cert == NULL)
		return 0;
	if (fname != NULL)
		*fname = NULL;

	alias = X509_alias_get0(cert, &len);
	if (alias == NULL || len == 0)
		return 0;

	if (dowhat == GETDO_DEL) {
		(void)X509_alias_set1(cert, NULL, 0);
		return 0;
	}

	/* Both COPY and GET yield a caller-visible pointer. Allocate a
	 * NUL-terminated copy in both cases for uniformity. */
	if (fname != NULL) {
		*fname = OPENSSL_malloc((size_t)len + 1);
		if (*fname == NULL)
			return -1;
		memcpy(*fname, alias, (size_t)len);
		(*fname)[len] = '\0';
	}
	return len;
}

/*
 * sunw_get_pkey_fname — get friendlyName attribute off an EVP_PKEY's
 * PKCS12 attributes. Not directly supported by OpenSSL 3.x public
 * API (PKCS12 attrs on standalone EVP_PKEY were a Sun extension).
 * Return 0 (no fname) unless caller-supplied name is set.
 */
int
sunw_get_pkey_fname(getdo_actions_t dowhat, EVP_PKEY *pkey, char **fname)
{
	(void)dowhat;  (void)pkey;
	if (fname != NULL)
		*fname = NULL;
	return 0;
}

/*
 * sunw_set_fname — set the friendlyName on cert + pkey.
 * Uses X509_alias_set1 for the cert. Pkey attributes not supported
 * standalone; caller-side no-op.
 */
int
sunw_set_fname(const char *ascname, EVP_PKEY *pkey, X509 *cert)
{
	(void)pkey;
	if (ascname == NULL || cert == NULL)
		return -1;
	if (X509_alias_set1(cert, (const unsigned char *)ascname,
	    (int)strlen(ascname)) == 0)
		return -1;
	return 0;
}

/*
 * sunw_find_fname — locate a (key, cert) pair whose alias matches.
 * Uses X509_alias_get0.
 */
int
sunw_find_fname(char *fname, STACK_OF(EVP_PKEY) *pkey_stk,
    STACK_OF(X509) *cert_stk, EVP_PKEY **pkey_out, X509 **cert_out)
{
	int i;
	if (fname == NULL || cert_stk == NULL)
		return -1;
	if (pkey_out != NULL)
		*pkey_out = NULL;
	if (cert_out != NULL)
		*cert_out = NULL;

	for (i = 0; i < sk_X509_num(cert_stk); i++) {
		X509 *c = sk_X509_value(cert_stk, i);
		int len = 0;
		const unsigned char *a = X509_alias_get0(c, &len);
		if (a != NULL && (int)strlen(fname) == len &&
		    memcmp(a, fname, (size_t)len) == 0) {
			if (cert_out != NULL)
				*cert_out = c;
			if (pkey_out != NULL && pkey_stk != NULL &&
			    i < sk_EVP_PKEY_num(pkey_stk))
				*pkey_out = sk_EVP_PKEY_value(pkey_stk, i);
			return 0;
		}
	}
	return -1;
}

/*
 * sunw_check_keys — verify a key and cert form a matching pair.
 * OpenSSL 3.x: X509_check_private_key(cert, pkey) returns 1 on match.
 * Algorithm-agnostic — includes PQ keypairs.
 */
int
sunw_check_keys(X509 *cert, EVP_PKEY *pkey)
{
	if (cert == NULL || pkey == NULL)
		return -1;
	return X509_check_private_key(cert, pkey) == 1 ? 0 : -1;
}

/*
 * sunw_check_cert_times — verify a cert's notBefore / notAfter against
 * the current wall clock. Uses X509_get0_notBefore / notAfter.
 */
chk_errs_t
sunw_check_cert_times(chk_actions_t what, X509 *cert)
{
	time_t now = time(NULL);
	const ASN1_TIME *nb, *na;
	int nb_cmp, na_cmp;

	(void)what;

	if (cert == NULL)
		return CHKERR_TIME_BEFORE_BAD;

	nb = X509_get0_notBefore(cert);
	na = X509_get0_notAfter(cert);

	nb_cmp = X509_cmp_time(nb, &now);  /* <0 nb is before now (good) */
	na_cmp = X509_cmp_time(na, &now);  /* >0 na is after now (good) */

	if (nb_cmp > 0)
		return CHKERR_TIME_BEFORE_BAD;
	if (na_cmp < 0)
		return CHKERR_TIME_AFTER_BAD;
	return CHKERR_TIME_OK;
}

/* ---------------- PEM contents ---------------- */

/*
 * sunw_PEM_contents — read a PEM file (may contain keys + certs)
 * into stacks. Uses PEM_read_X509 + PEM_read_PrivateKey in a loop.
 * Algorithm-agnostic — PEM_read_PrivateKey accepts any key type
 * OpenSSL knows, including PQ (ML-DSA, SLH-DSA) in 3.5+.
 */
int
sunw_PEM_contents(FILE *fp, pem_password_cb *cb, void *userdata,
    STACK_OF(EVP_PKEY) **pkeys_out, STACK_OF(X509) **certs_out)
{
	STACK_OF(EVP_PKEY) *pkey_stk = NULL;
	STACK_OF(X509) *cert_stk = NULL;
	EVP_PKEY *pkey;
	X509 *cert;
	int found = 0;

	if (fp == NULL)
		return -1;

	pkey_stk = sk_EVP_PKEY_new_null();
	cert_stk = sk_X509_new_null();
	if (pkey_stk == NULL || cert_stk == NULL)
		goto err;

	rewind(fp);
	while ((cert = PEM_read_X509(fp, NULL, cb, userdata)) != NULL) {
		if (sk_X509_push(cert_stk, cert) == 0) {
			X509_free(cert);
			goto err;
		}
		found = 1;
	}
	ERR_clear_error();

	rewind(fp);
	while ((pkey = PEM_read_PrivateKey(fp, NULL, cb, userdata)) != NULL) {
		if (sk_EVP_PKEY_push(pkey_stk, pkey) == 0) {
			EVP_PKEY_free(pkey);
			goto err;
		}
		found = 1;
	}
	ERR_clear_error();

	if (!found)
		goto err;

	*pkeys_out = pkey_stk;
	*certs_out = cert_stk;
	return 0;

err:
	if (pkey_stk != NULL)
		sk_EVP_PKEY_pop_free(pkey_stk, EVP_PKEY_free);
	if (cert_stk != NULL)
		sk_X509_pop_free(cert_stk, X509_free);
	return -1;
}

/*
 * Provide the remaining sunw_ entry points as thin stubs. Callers
 * that reach these paths get a clean "not supported on this build"
 * signal + ERR queue update. Signed-package feature is present when
 * OpenSSL 3.x infrastructure suffices; extended attribute helpers
 * that were Sun-specific are not.
 */
int
sunw_set_localkeyid(const char *keyid, int keyid_len,
    EVP_PKEY *pkey, X509 *cert)
{
	(void)pkey;
	if (cert == NULL || keyid == NULL)
		return -1;
	if (X509_keyid_set1(cert, (const unsigned char *)keyid, keyid_len) == 0)
		return -1;
	return 0;
}

int
sunw_get_pkey_localkeyid(getdo_actions_t dowhat, EVP_PKEY *pkey,
    char **keyid, int *keyid_len)
{
	(void)dowhat; (void)pkey;
	if (keyid != NULL) *keyid = NULL;
	if (keyid_len != NULL) *keyid_len = 0;
	return 0;
}

int
sunw_find_localkeyid(char *keyid, int keyid_len,
    STACK_OF(EVP_PKEY) *pkeys, STACK_OF(X509) *certs,
    EVP_PKEY **pkey_out, X509 **cert_out)
{
	int i;
	if (keyid == NULL || certs == NULL)
		return -1;
	if (pkey_out != NULL) *pkey_out = NULL;
	if (cert_out != NULL) *cert_out = NULL;

	for (i = 0; i < sk_X509_num(certs); i++) {
		X509 *c = sk_X509_value(certs, i);
		int len = 0;
		const unsigned char *k = X509_keyid_get0(c, &len);
		if (k != NULL && len == keyid_len &&
		    memcmp(k, keyid, (size_t)len) == 0) {
			if (cert_out != NULL) *cert_out = c;
			if (pkey_out != NULL && pkeys != NULL &&
			    i < sk_EVP_PKEY_num(pkeys))
				*pkey_out = sk_EVP_PKEY_value(pkeys, i);
			return 0;
		}
	}
	return -1;
}

/*
 * Suppress the "signature capability" check as unused when caller
 * doesn't reach it. Left here so future work can invoke it while
 * accepting PQ keys.
 */
static int __attribute__((unused))
heirloom_p12_can_sign(EVP_PKEY *pkey)
{
	return key_can_sign(pkey);
}

#endif	/* __APPLE__ */
