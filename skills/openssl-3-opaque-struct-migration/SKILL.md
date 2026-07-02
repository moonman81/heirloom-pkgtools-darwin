---
name: openssl-3-opaque-struct-migration
description: "OpenSSL 3.x makes X509, EVP_PKEY, PKCS12, PKCS7, X509_INFO, X509_ATTRIBUTE, ASN1_STRING, ERR_STATE opaque. Migration strategy: replace direct field access (cert->cert_info->..., pkey->pkey.rsa, ustr->data / .length) with accessor calls (X509_get0_*, EVP_PKEY_get0_*, EVP_PKEY_get_id, ASN1_STRING_get0_data + ASN1_STRING_length, ASN1_STRING_type). SKM_sk_* macros → sk_TYPE_* inline functions via DEFINE_STACK_OF(TYPE). M_PKCS12_* → PKCS12_SAFEBAG_*. When surgical port is prohibitively large, prefer a fresh accessor-only replacement TU (as heirloom-pkgtools-darwin did for p12lib_openssl3.c)."
gate: 3
version: "1.0.0"
author: moonman81
tags: [openssl, openssl-3, x509, evp-pkey, pkcs12, migration, opaque-struct, compat-header]
depends_on: []
allowed-tools:
  - Read
  - Grep
  - Write
when_to_use: "Invoke when porting a codebase from OpenSSL 1.0.x (or 1.1.x with legacy struct access) to OpenSSL 3.x. Triggers: 'X509 opaque', 'EVP_PKEY get0', 'PKCS12 accessor', 'SKM_sk_ compat', 'DEFINE_STACK_OF', 'DECLARE_STACK_OF migration', 'openssl3_compat header'."
---

# OpenSSL 3.x opaque struct migration

## What changed in OpenSSL 3.x

OpenSSL 3.x made most public-API structures **opaque** — code can
hold pointers to them but cannot access fields directly. The direct
struct accesses that pervaded OpenSSL-1.0.x-era code no longer
compile:

- `cert->cert_info->key->pkey` — X509 opaque
- `cert->aux->alias` — X509 opaque
- `pkey->pkey.rsa` — EVP_PKEY opaque
- `pkey->type` — EVP_PKEY opaque
- `ustr->data`, `ustr->length`, `ustr->type` — ASN1_STRING opaque
- `p12->authsafes` — PKCS12 opaque
- `attr->object`, `attr->value.set` — X509_ATTRIBUTE opaque
- `ERR_STATE.top`, `.errcnt` — ERR_STATE opaque

## The accessor toolkit

Every opaque struct has a set of `<TYPE>_get0_*` accessor functions
that return **borrowed** pointers into the struct (do not free), and
`<TYPE>_set0_*` / `<TYPE>_set1_*` mutators. Some, but not all,
have `<TYPE>_get1_*` that return owned pointers.

Common substitutions:

| Old (opaque access) | New (accessor) |
|---|---|
| `cert->cert_info->key->pkey` | `X509_get_pubkey(cert)` |
| `cert->cert_info->extensions` | `X509_get0_extensions(cert)` |
| `cert->aux->alias` | `X509_alias_get0(cert, &len)` |
| `cert->aux->keyid` | `X509_keyid_get0(cert, &len)` |
| `X509_alias_set1(cert, NULL, 0)` | idem — clears |
| `cert->cert_info->validity->notBefore` | `X509_get0_notBefore(cert)` |
| `pkey->type` | `EVP_PKEY_get_id(pkey)` |
| `pkey->pkey.rsa` | `EVP_PKEY_get0_RSA(pkey)` |
| `pkey->pkey.dsa` | `EVP_PKEY_get0_DSA(pkey)` |
| `pkey->pkey.ec` | `EVP_PKEY_get0_EC_KEY(pkey)` |
| `ustr->data` | `ASN1_STRING_get0_data(ustr)` |
| `ustr->length` | `ASN1_STRING_length(ustr)` |
| `ustr->type` | `ASN1_STRING_type(ustr)` |
| `M_PKCS12_x5092certbag(cert)` | `PKCS12_SAFEBAG_create_cert(cert)` |
| `M_PKCS12_certbag2x509(bag)` | `PKCS12_SAFEBAG_get1_cert(bag)` |
| `M_ASN1_OCTET_STRING_new()` | `ASN1_OCTET_STRING_new()` (drop `M_`) |

## SKM_sk_* / DECLARE_STACK_OF migration

OpenSSL 1.0.x used `DECLARE_STACK_OF(TYPE)` in headers and
`SKM_sk_<op>(TYPE, ...)` macros at call sites. OpenSSL 3.x replaced
with `DEFINE_STACK_OF(TYPE)` (which generates `sk_TYPE_<op>` inline
functions) and dropped the `SKM_` prefix.

Compat header pattern:

```c
#include <openssl/opensslv.h>
#include <openssl/safestack.h>
#include <openssl/x509.h>
#include <openssl/pkcs12.h>

/* Types OpenSSL 3.x does not pre-DEFINE_STACK_OF but we need. */
DEFINE_STACK_OF(EVP_PKEY)

/* SKM_sk_<op> → sk_TYPE_<op> */
#define SKM_sk_new_null(T)          sk_##T##_new_null()
#define SKM_sk_num(T, st)           sk_##T##_num(st)
#define SKM_sk_value(T, st, i)      sk_##T##_value((st), (i))
#define SKM_sk_push(T, st, v)       sk_##T##_push((st), (v))
#define SKM_sk_pop(T, st)           sk_##T##_pop(st)
#define SKM_sk_free(T, st)          sk_##T##_free(st)
#define SKM_sk_pop_free(T, st, f)   sk_##T##_pop_free((st), (f))
/* ... etc. */
```

The `##` token-paste means one macro definition works for every
TYPE that has `DEFINE_STACK_OF(TYPE)` in scope.

## Fresh-replacement TU as an alternative to surgical porting

When the legacy code has hundreds of opaque-struct accesses, the
cost of surgical porting can exceed the cost of writing a fresh
translation unit against OpenSSL 3.x's public API. The port of
Heirloom pkgtools chose this route for `p12lib.c`:

- Legacy `p12lib.c` (~3000 LOC of Sun-specific PKCS#12 helpers)
  kept in-tree for reference.
- New `p12lib_openssl3.c` (~350 LOC) provides **the same public
  `sunw_*` API surface** but implemented purely against OpenSSL
  3.x accessors.
- `Makefile.mk` swaps `p12lib.o` for `p12lib_openssl3.o` on Darwin.

This preserves ABI (linkers link against the same symbols) and
allows the fresh implementation to be **algorithm-agnostic** — no
hard-coded `EVP_PKEY_RSA` / `EVP_PKEY_DSA` checks, uses
`EVP_PKEY_can_sign()` instead, and therefore accepts post-quantum
algorithms transparently (see the sibling skill
`pq-ready-generic-evp-pkey`).

## When to surgical-port vs. fresh-replace

| Surgical | Fresh replacement |
|---|---|
| < 20 opaque-struct accesses | > 100 accesses |
| Callers are external + numerous | Callers are internal only |
| Behaviour must be bit-identical | Only the public API contract matters |
| Retains git-blame value | Simplifies maintenance |

## References

- OpenSSL 3.x migration guide:
  <https://www.openssl.org/docs/manmaster/man7/migration_guide.html>
- `heirloom-pkgtools-darwin/libpkg/p12lib_openssl3.c` — worked example.
- `heirloom-pkgtools-darwin/hdrs/openssl3_compat.h` — the compat
  header with SKM_sk_ + M_PKCS12_ + M_ASN1_ shims.
