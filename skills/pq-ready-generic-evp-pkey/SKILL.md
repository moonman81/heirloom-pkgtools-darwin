---
name: pq-ready-generic-evp-pkey
description: "Post-quantum-crypto readiness pattern for OpenSSL 3.x code: never gate on specific key-type IDs (EVP_PKEY_RSA / EVP_PKEY_DSA / EVP_PKEY_EC), instead ask EVP_PKEY infrastructure directly — EVP_PKEY_can_sign() for signing capability, EVP_PKEY_get_id() for type inspection when needed, EVP_PKEY_get_size() for buffer sizing. OpenSSL 3.5+ ships ML-KEM 512/768/1024 (FIPS 203), SLH-DSA-SHAKE-256f (FIPS 205), and hybrids (X25519MLKEM768, SecP256r1MLKEM768) as first-class citizens. Algorithm-agnostic code accepts them without change."
gate: 3
version: "1.0.0"
author: moonman81
tags: [openssl, openssl-3, post-quantum, ml-kem, ml-dsa, slh-dsa, fips-203, fips-204, fips-205, evp-pkey]
depends_on:
  - openssl-3-opaque-struct-migration
allowed-tools:
  - Read
  - Grep
  - Write
when_to_use: "Invoke when writing or auditing crypto code that handles user-supplied private keys / certificates / signatures, or when checking whether a codebase will accept post-quantum algorithms as they roll out. Triggers: 'PQ crypto', 'ML-KEM', 'ML-DSA', 'SLH-DSA', 'FIPS 203', 'FIPS 205', 'algorithm-agnostic key handling', 'EVP_PKEY_can_sign', 'hybrid crypto readiness'."
---

# PQ-ready via generic EVP_PKEY

## The design principle

OpenSSL 3.x's EVP_PKEY layer is **algorithm-agnostic by
construction**: any key algorithm the loaded providers know about
is exposed via the same EVP_PKEY object handle. This includes:

- **Classical:** RSA, DSA, ECDSA, EdDSA, X25519, X448.
- **Post-quantum (NIST-finalised, in OpenSSL 3.5+ default provider):**
  - **ML-KEM-512 / 768 / 1024** (FIPS 203 — Kyber-based key
    encapsulation)
  - **SLH-DSA-SHAKE-256f** (FIPS 205 — SPHINCS+-based signatures)
  - **ML-DSA-44 / 65 / 87** (FIPS 204 — Dilithium-based signatures,
    once OpenSSL merges them; currently via oqs-provider)
- **Hybrid modes:** X25519MLKEM768, X448MLKEM1024, SecP256r1MLKEM768,
  SecP384r1MLKEM1024.

Provided your crypto code touches EVP_PKEY through **public
accessors and capability queries**, it accepts these algorithms
with no code change.

## What to write

```c
/* Instead of: */
if (EVP_PKEY_type(pkey->type) == EVP_PKEY_RSA ||
    EVP_PKEY_type(pkey->type) == EVP_PKEY_DSA) {
    // sign this key
}

/* Write: */
if (EVP_PKEY_can_sign(pkey)) {
    // sign this key — works for RSA, DSA, EC, ML-DSA, SLH-DSA, ...
}
```

`EVP_PKEY_can_sign()` asks the provider "can this key produce a
signature?" — no hard-coded algorithm list.

## What NOT to write

Any switch on specific key-type IDs to decide whether to accept:

```c
/* ANTI-PATTERN — silently rejects ML-DSA-signed inputs */
switch (EVP_PKEY_get_id(pkey)) {
    case EVP_PKEY_RSA: /* accept */ break;
    case EVP_PKEY_DSA: /* accept */ break;
    case EVP_PKEY_EC:  /* accept */ break;
    default: /* REJECT */ return -1;
}
```

If the code must gate on some property, gate on the **property**,
not the algorithm.

## Concrete capability queries in the OpenSSL 3 EVP layer

| Question | API |
|---|---|
| "Can this key sign?" | `EVP_PKEY_can_sign(pkey)` |
| "Can this key encrypt / decrypt?" | `EVP_PKEY_can_encrypt(pkey)` |
| "Can this key perform key agreement?" | `EVP_PKEY_can_derive(pkey)` |
| "Can this key key-encapsulate?" | (implicit — use `EVP_PKEY_encapsulate_init` and check return) |
| "What size is a signature by this key?" | `EVP_PKEY_get_size(pkey)` |
| "How many bits of security?" | `EVP_PKEY_get_security_bits(pkey)` |

## PKCS#12 keystore reads accept PQ transparently

`PKCS12_parse()`, `PEM_read_PrivateKey()`, `X509_check_private_key()`
all delegate to the EVP layer. If OpenSSL knows the algorithm,
these calls succeed. In `heirloom-pkgtools-darwin/libpkg/p12lib_openssl3.c`,
this is why the port is PQ-ready — the code never asks "is this an
RSA key?", it just delegates.

## Signing side is currently stubbed in this port

`sunw_PKCS12_create()` (the write side — packaging a key + cert
into a p12 file) is stubbed on Darwin in this port. When re-enabled,
the same principle applies: `PKCS12_create()` accepts any EVP_PKEY
type OpenSSL knows about, including PQ, without special-casing.

## OpenSSL version requirements

- **3.5+** — native ML-KEM, hybrid modes. Homebrew's `openssl@3`
  formula (currently 3.6.2 on macOS as of the port's baseline) ships
  these.
- **3.0..3.4** — need `oqs-provider` for PQ algorithms:
  `brew install liboqs oqs-provider` and load via `openssl.cnf`.
- **1.1.x and 1.0.x** — no PQ. Not supported by this port.

## Migration path from a pre-PQ port

If you have an OpenSSL 3.x port that hard-codes `EVP_PKEY_RSA`:

1. Grep for `EVP_PKEY_RSA`, `EVP_PKEY_DSA`, `EVP_PKEY_EC`,
   `EVP_PKEY_type`, `pkey->type`.
2. For each site, decide: is this really about "which crypto
   algorithm" or about "does this key support the operation I'm
   about to do"?
3. Replace the latter with `EVP_PKEY_can_sign` /
   `EVP_PKEY_can_derive` etc.
4. Leave the former (rare) alone.

## References

- OpenSSL manual: `EVP_PKEY_can_sign(3)`, `EVP_PKEY_get_id(3)`.
- NIST FIPS 203 (ML-KEM), FIPS 204 (ML-DSA), FIPS 205 (SLH-DSA).
- `heirloom-pkgtools-darwin/libpkg/p12lib_openssl3.c` — worked example.
