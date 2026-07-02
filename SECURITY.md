# Security policy

## Repository scope + limitations

This is a **downstream Darwin port** of Gunnar Ritter's Heirloom
Project. **It is not an authoritative source** — see `NOTICE.md`.
For vulnerabilities in the original Heirloom code that also affect
upstream, please report to upstream at
<http://heirloom.sourceforge.net> as well. The upstream project has
been unmaintained since 2008; downstream distributors (Debian, Fedora,
Homebrew's copies where applicable) may also carry patches.

**No warranty. No guarantee of fitness.** See `NOTICE.md`.

## Supported versions

Only the current `main` branch is under any form of maintenance.
There are no numbered releases yet; there may never be. Best-effort
security responses on `main`; historical commits receive **no**
security backports.

## Reporting a vulnerability

If you find a vulnerability that meaningfully affects Darwin users
of this port:

1. **Open a GitHub Security Advisory** on this repository — this
   creates a private discussion channel:
   `Security` tab → `Report a vulnerability`.
2. If GHSA is not available for any reason, open a public GitHub
   issue **only if** the vulnerability is already public elsewhere
   (upstream CVE, security-list post).
3. **Do not** post exploit details on the public issue tracker for
   novel findings — coordinate through the GHSA channel first.

### What to include

- Affected file + commit SHA + line numbers if possible
- Reproduction: input that triggers the issue + observed vs
  expected behaviour
- CVSS or CWE mapping if you have one
- Whether the same issue affects upstream heirloom.sourceforge.net
  (many will — see the port scope)
- Suggested fix, if any

### Response posture

- **Best-effort.** This project has one maintainer, no funding, no
  SLA. Expect a first response within about a week; a fix within
  a few weeks; a public disclosure once the fix is merged.
- **No bug bounty.** The publisher does not offer financial rewards.
- Credit will be given in `CHANGELOG.md` for accepted reports
  unless the reporter requests anonymity.

## What's already in scope for hardening

The port ships a `hardening/` suite (in the workspace repo, and
referenced from the code repos) that covers:

- **CWE Top 25** — buffer overflow (CWE-119/120/125/787), format
  string (CWE-134), integer overflow (CWE-190), UAF (CWE-416), NULL
  deref (CWE-476), path traversal (CWE-22), command injection
  (CWE-78), incorrect perms (CWE-732)
- **OWASP Top 10** — A01 broken access control (setuid `su`,
  spool ownership), A03 injection, A08 software & data integrity
- **MITRE ATT&CK v14** — T1548 setuid abuse, T1574.006 dylib
  hijacking, T1059.004 shell abuse
- **CAPEC 3.9** — CAPEC-69 targeted-privilege programs, CAPEC-100
  buffer overflow, CAPEC-135 format-string injection, CAPEC-159
  library redirection, CAPEC-471 search-order hijacking

See `hardening/COVERAGE-MATRIX.md` (workspace repo) for the full map
of each check to concrete framework entries.

## Known deferred items with security relevance

Documented in `PORT.md` and `hardening/*/COVERAGE.md`:

- `devtools/make/vroot/` chmod/chown/readlink race conditions
  (CWE-362 / CWE-367) — 79 flawfinder level-5 findings. Inherent to
  SVR4 make's virtual-root abstraction; not fixable without a design
  rewrite. **Do not run `heirloom-make` in a directory tree an
  untrusted user can write to.**
- `sunw_PKCS12_create` (pkgtools) — pkg-signing side is a stub;
  read side is real. **Do not use this build for creating signed
  packages.**
- `nawk`, `oawk`, `bc` — no recursion-depth bounding (CWE-674). A
  crafted script triggers stack-overflow SIGSEGV. **Apply `ulimit -s`
  before feeding these to untrusted scripts.**

## Cryptographic posture

- pkgtools uses OpenSSL 3.x via `p12lib_openssl3.c`. Post-quantum
  key algorithms (ML-KEM, ML-DSA, SLH-DSA, hybrids) are accepted
  transparently since OpenSSL 3.5+ handles them natively; the port
  has no algorithm gating.
- Signature verification depends on OpenSSL's `PKCS12_parse` +
  `X509_check_private_key` — no custom crypto is implemented by the
  port.
- **Do not trust this port for high-value signature verification
  without independent review.** The AI-assisted p12lib_openssl3.c
  has not been through formal audit.

## Historical context

Heirloom is decades-old C code. Many CWE Top 25 patterns (K&R
`strcpy`/`sprintf`, unchecked buffer arithmetic) are present by
design. The 5,900+ flawfinder findings are overwhelmingly
size-bounded internal string manipulation. Individual triage of these
is a large undertaking; the port has fixed only the cases where
attacker-controlled data reaches the unsafe primitive.

**If your threat model requires modern-C safety guarantees, use
Homebrew coreutils + POSIX awk + system troff instead.**
