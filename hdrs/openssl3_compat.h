/*
 * openssl3_compat.h — Compatibility shim mapping OpenSSL 1.0.x macros
 * used by Heirloom pkgtools to OpenSSL 3.x primitives.
 *
 * Heirloom's libpkg/{p12lib,security,keystore,pkgweb}.[ch] were
 * authored against OpenSSL 1.0.x and use:
 *
 *   - DECLARE_STACK_OF(TYPE)           — generate STACK_OF(TYPE) type
 *   - SKM_sk_<op>(TYPE, ...)           — per-op stack manipulation
 *   - ERR_STATE.top / errcnt legacy    — direct struct access
 *   - PKCS12 accessor field reads      — direct struct access
 *
 * OpenSSL 3.x replaced this with:
 *
 *   - DEFINE_STACK_OF(TYPE)            — generate type + inline sk_TYPE_*
 *   - sk_TYPE_<op>(...)                — per-type inline wrappers around
 *                                        OPENSSL_sk_<op>
 *   - opaque struct pointers; use      — accessor functions
 *     accessor APIs
 *
 * This header (a) defines DEFINE_STACK_OF for the types Heirloom needs,
 * and (b) provides SKM_sk_<op> compat macros expressed in terms of
 * OpenSSL 3.x sk_TYPE_<op>. Include from p12lib.h before any use.
 *
 * -- Heirloom Darwin port (Phase 5-C4).
 */

#ifndef _HEIRLOOM_OPENSSL3_COMPAT_H
#define _HEIRLOOM_OPENSSL3_COMPAT_H

#include <stdlib.h>	/* NULL, malloc, size_t */
#include <string.h>	/* strlen */
#include <openssl/opensslv.h>
#include <openssl/safestack.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/crypto.h>	/* OPENSSL_malloc */
#include <openssl/pkcs12.h>
#include <openssl/pkcs7.h>
#include <openssl/asn1.h>

/*
 * Generate STACK_OF(TYPE) types + sk_TYPE_* inline wrappers for every
 * type Heirloom pkgtools puts on a stack. OpenSSL 3.x does this via
 * DEFINE_STACK_OF; the generated inline functions live in this
 * translation unit's scope. OpenSSL 3.x ships several of these
 * pre-defined (X509, PKCS12_SAFEBAG, PKCS7, X509_ATTRIBUTE) —
 * DEFINE_STACK_OF is idempotent per header via include-guards, so
 * defining EVP_PKEY (which OpenSSL does not pre-define) covers our
 * needs without collision.
 */
#ifndef SKM_HEIRLOOM_STACK_OF_EVP_PKEY_DEFINED
#define SKM_HEIRLOOM_STACK_OF_EVP_PKEY_DEFINED 1
DEFINE_STACK_OF(EVP_PKEY)
#endif

/*
 * Compat: SKM_sk_<op>(TYPE, ...) → sk_TYPE_<op>(...).
 * The token-paste variant lets the same code text work under both
 * OpenSSL 1.0.x and 3.x since the substitutions expand to real
 * per-type functions in 3.x and to old macro-machinery in 1.0.x.
 */
#define SKM_sk_new_null(T)          sk_##T##_new_null()
#define SKM_sk_new(T, cmp)          sk_##T##_new(cmp)
#define SKM_sk_free(T, st)          sk_##T##_free(st)
#define SKM_sk_zero(T, st)          sk_##T##_zero(st)
#define SKM_sk_num(T, st)           sk_##T##_num(st)
#define SKM_sk_value(T, st, i)      sk_##T##_value((st), (i))
#define SKM_sk_push(T, st, v)       sk_##T##_push((st), (v))
#define SKM_sk_pop(T, st)           sk_##T##_pop(st)
#define SKM_sk_find(T, st, v)       sk_##T##_find((st), (v))
#define SKM_sk_delete(T, st, i)     sk_##T##_delete((st), (i))
#define SKM_sk_delete_ptr(T, st, p) sk_##T##_delete_ptr((st), (p))
#define SKM_sk_insert(T, st, v, i)  sk_##T##_insert((st), (v), (i))
#define SKM_sk_pop_free(T, st, f)   sk_##T##_pop_free((st), (f))
#define SKM_sk_set(T, st, i, v)     sk_##T##_set((st), (i), (v))
#define SKM_sk_shift(T, st)         sk_##T##_shift(st)
#define SKM_sk_unshift(T, st, v)    sk_##T##_unshift((st), (v))
#define SKM_sk_dup(T, st)           sk_##T##_dup(st)
#define SKM_sk_deep_copy(T, st, cp, fr)  sk_##T##_deep_copy((st), (cp), (fr))
#define SKM_sk_sort(T, st)          sk_##T##_sort(st)
#define SKM_sk_is_sorted(T, st)     sk_##T##_is_sorted(st)

/*
 * ERR_STATE.top and .errcnt — OpenSSL 3.x makes ERR_STATE opaque.
 * Heirloom uses these to detect "was any error queued?" — replace
 * with the OpenSSL 3 accessor ERR_peek_error() which returns 0
 * when the queue is empty.
 */
#define HEIRLOOM_OSSL_ERR_ANY()	(ERR_peek_error() != 0)

/*
 * Removed / renamed macros. In OpenSSL 1.0.x these were preprocessor
 * macros expanding to internal helpers. OpenSSL 3.x removed the M_
 * prefix and turned them into real API functions with mostly the
 * same signatures.
 */
#define M_PKCS12_x5092certbag(x)	PKCS12_SAFEBAG_create_cert(x)
#define M_PKCS12_x509crl2certbag(x)	PKCS12_SAFEBAG_create_crl(x)
#define M_PKCS12_pack_authsafes(p12, safes)	PKCS12_pack_authsafes((p12), (safes))
#define M_PKCS12_unpack_authsafes(p12)	PKCS12_unpack_authsafes(p12)

#define M_ASN1_OCTET_STRING_new()	ASN1_OCTET_STRING_new()
#define M_ASN1_OCTET_STRING_free(x)	ASN1_OCTET_STRING_free(x)
#define M_ASN1_OCTET_STRING_set(x, d, l)  ASN1_OCTET_STRING_set((x), (d), (l))
#define M_ASN1_OCTET_STRING_dup(x)	ASN1_OCTET_STRING_dup(x)
#define M_ASN1_STRING_dup(x)		ASN1_STRING_dup(x)
#define M_ASN1_STRING_length(x)		ASN1_STRING_length(x)
#define M_ASN1_STRING_type(x)		ASN1_STRING_type(x)
#define M_ASN1_STRING_data(x)		ASN1_STRING_data(x)
#define M_i2d_PKCS12_BAGS(bags)		i2d_PKCS12_BAGS(bags)

#define M_ASN1_BMPSTRING_new()		ASN1_BMPSTRING_new()
#define M_ASN1_BMPSTRING_free(x)	ASN1_BMPSTRING_free(x)

#define M_PKCS12_certbag2x509(bag)		PKCS12_SAFEBAG_get1_cert(bag)
#define M_PKCS12_unpack_p7data(p7)		PKCS12_unpack_p7data(p7)
#define M_PKCS12_unpack_p7encdata(p7, pass, plen)	\
	PKCS12_unpack_p7encdata((p7), (pass), (plen))
#define M_PKCS12_decrypt_skey(bag, pass, plen)	\
	PKCS12_decrypt_skey((bag), (pass), (plen))
#define M_PKCS12_x5092certbag_and_encrypt(bag, cert)	\
	PKCS12_SAFEBAG_create_cert(cert)

/*
 * asc2uni — encode ASCII string as BMPString (2-byte-per-char, big-
 * endian, high byte 0). OpenSSL 3.x removed the public helper.
 * Reimplement with the same signature: takes (const char *asc,
 * int asclen, unsigned char **uni, int *unilen) — returns 1 success,
 * 0 failure. If asclen < 0 use strlen(asc).
 */
static inline int
heirloom_asc2uni(const char *asc, int asclen,
    unsigned char **uni, int *unilen)
{
	int	i;
	if (asc == NULL || uni == NULL || unilen == NULL)
		return 0;
	if (asclen < 0)
		asclen = (int)strlen(asc);
	*unilen = asclen * 2 + 2;
	*uni = (unsigned char *)OPENSSL_malloc((size_t)*unilen);
	if (*uni == NULL)
		return 0;
	for (i = 0; i < asclen; i++) {
		(*uni)[i * 2]     = 0;
		(*uni)[i * 2 + 1] = (unsigned char)asc[i];
	}
	(*uni)[*unilen - 2] = 0;
	(*uni)[*unilen - 1] = 0;
	return 1;
}
#ifndef asc2uni
#define asc2uni(a, al, u, ul)		heirloom_asc2uni((a), (al), (u), (ul))
#endif
#ifndef OPENSSL_asc2uni
#define OPENSSL_asc2uni(a, al, u, ul)	heirloom_asc2uni((a), (al), (u), (ul))
#endif

/*
 * OpenSSL 3.x removed the uni2asc() helper. Provide a minimal
 * reimplementation: strip the high byte of each 16-bit BMPString
 * character into a null-terminated ASCII buffer. Callers free() the
 * result. Signature matches OpenSSL 1.0.x libwanboot: OPENSSL_uni2asc
 * takes (const unsigned char *uni, int unilen). Our helper accepts
 * either.
 */
static inline char *
heirloom_uni2asc(const unsigned char *uni, int unilen)
{
	int	i, out_len = unilen / 2;
	char	*asc = (char *)OPENSSL_malloc((size_t)out_len + 1);
	if (asc == NULL || uni == NULL)
		return asc;
	for (i = 0; i < out_len; i++)
		asc[i] = (char)uni[i * 2 + 1];	/* low byte of BE BMP */
	asc[out_len] = '\0';
	return asc;
}
#ifndef uni2asc
#define uni2asc(u, l)			heirloom_uni2asc((u), (l))
#endif
#ifndef OPENSSL_uni2asc
#define OPENSSL_uni2asc(u, l)		heirloom_uni2asc((u), (l))
#endif

/*
 * X.509 aux field access. OpenSSL 3.x hides X509->cert_info->... and
 * X509->aux. The public accessors return only the byte-string and
 * length, not the ASN1_STRING wrapper. Heirloom code does:
 *
 *   if (cert->aux != NULL && cert->aux->alias != NULL &&
 *       cert->aux->alias->type == V_ASN1_UTF8STRING) {
 *       str = utf82ascstr(cert->aux->alias);
 *
 * On OpenSSL 3.x the alias comes back as a UTF-8 byte string via
 * X509_alias_get0(); the type check was always redundant because
 * X509 stores the alias only as UTF-8. Provide compat macros:
 *   x509_aux_has_alias(cert)   → true if there's an alias
 *   x509_aux_alias_data(cert)  → const unsigned char* into the alias
 *   x509_aux_alias_length(cert) → int length
 *   x509_aux_has_keyid(cert)   → true if there's a keyid
 *   x509_aux_keyid_data(cert)  → const unsigned char*
 *   x509_aux_keyid_length(cert)→ int length
 */
static inline int
heirloom_x509_aux_alias_length(const X509 *cert)
{
	int len = 0;
	(void)X509_alias_get0((X509 *)cert, &len);
	return len;
}
static inline const unsigned char *
heirloom_x509_aux_alias_data(const X509 *cert)
{
	int len = 0;
	return X509_alias_get0((X509 *)cert, &len);
}
static inline int
heirloom_x509_aux_has_alias(const X509 *cert)
{
	int len = 0;
	return X509_alias_get0((X509 *)cert, &len) != NULL && len > 0;
}

static inline int
heirloom_x509_aux_keyid_length(const X509 *cert)
{
	int len = 0;
	(void)X509_keyid_get0((X509 *)cert, &len);
	return len;
}
static inline const unsigned char *
heirloom_x509_aux_keyid_data(const X509 *cert)
{
	int len = 0;
	return X509_keyid_get0((X509 *)cert, &len);
}
static inline int
heirloom_x509_aux_has_keyid(const X509 *cert)
{
	int len = 0;
	return X509_keyid_get0((X509 *)cert, &len) != NULL && len > 0;
}

/*
 * EVP_PKEY internal-type access. OpenSSL 3.x hides pkey->pkey.rsa /
 * .dsa / .ec — code uses EVP_PKEY_get0_RSA(pkey) / _DSA / _EC_KEY,
 * or EVP_PKEY_get_id(pkey) for the ID.
 *
 * Heirloom pattern is typically:
 *   if (pkey->type == EVP_PKEY_RSA) ...
 * → replace with EVP_PKEY_get_id(pkey) == EVP_PKEY_RSA.
 *
 * And save-as-DER via i2d_PrivateKey(pkey, ...): unchanged in 3.x.
 */
static inline int
heirloom_evp_pkey_type(const EVP_PKEY *pkey)
{
	return EVP_PKEY_get_id(pkey);
}

/*
 * PKCS12 struct field access — OpenSSL 3 hides the internals. Heirloom
 * reads p12->authsafes; use PKCS12_unpack_authsafes() or return NULL
 * if the field can't be reached. For the paths pkgtools actually uses
 * (verify + parse), the public API PKCS12_parse + PKCS12_verify_mac
 * suffices and internal field access is not required.
 */

#endif /* _HEIRLOOM_OPENSSL3_COMPAT_H */
