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

#include <openssl/opensslv.h>
#include <openssl/safestack.h>
#include <openssl/evp.h>
#include <openssl/x509.h>

/*
 * Generate STACK_OF(EVP_PKEY) type + sk_EVP_PKEY_* inline wrappers.
 * OpenSSL 3.x does this via DEFINE_STACK_OF; the generated inline
 * functions live in this translation unit's scope.
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
 * PKCS12 struct field access — OpenSSL 3 hides the internals. Heirloom
 * reads p12->authsafes; use PKCS12_unpack_authsafes() or return NULL
 * if the field can't be reached. For the paths pkgtools actually uses
 * (verify + parse), the public API PKCS12_parse + PKCS12_verify_mac
 * suffices and internal field access is not required.
 */

#endif /* _HEIRLOOM_OPENSSL3_COMPAT_H */
