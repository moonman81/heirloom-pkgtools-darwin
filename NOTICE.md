# NOTICE — READ FIRST

## Sole purpose of this repository

The **sole purpose** of this repository is to publish a **macOS / Darwin
(arm64) port** of Gunnar Ritter's Heirloom Packaging Tools. It is
maintained by an individual downstream porter for personal Darwin use
and shared publicly for the benefit of other Darwin users who need the
same tools working on modern macOS.

## THIS IS NOT AN AUTHORITATIVE SOURCE

**Do not use this repository as the authoritative source for Heirloom.**
It is a **derivative port**, not the canonical upstream. If you want the
original, authoritative Heirloom source, go to:

- **Original project:** <http://heirloom.sourceforge.net>
- **Original author + upstream maintainer:** Gunnar Ritter, Freiburg
  i. Br., Germany. <gunnarr@acm.org>. (The upstream project has been
  unmaintained since 2008; the SourceForge tarballs remain the
  canonical reference.)

Anyone building for a platform other than Darwin, or wanting the
pristine reference implementation for any purpose (academic study,
compatibility testing, legal citation, security review), should go
directly to upstream — **not here**.

## No warranty. No guarantee of originality. No guarantee of fitness.

**Use at your own risk.**

- **No warranty.** This code is provided **as-is**. No express or
  implied warranty of any kind — merchantability, fitness for a
  particular purpose, non-infringement, or otherwise — accompanies
  this repository. The publisher assumes no liability for damages
  arising from its use.
- **No guarantee of originality.** The publisher makes no claim to
  the original Heirloom code and cannot warrant the originality,
  authenticity, or provenance of any file. All upstream licences and
  copyright headers remain unchanged; check them for the authoritative
  attribution of each file.
- **No guarantee of fitness.** The port has been sanity-tested on the
  porter's own Darwin machine; that is the extent of validation. It
  has **not** been through formal QA, certification, or third-party
  security review. Do not rely on it for anything safety-, security-,
  or business-critical without independent evaluation.

## This repository publishes only the Darwin port patches

The commit at commit 1 (`3789e49`) is a **byte-exact
reproduction** of the upstream **heirloom-pkgtools-070227** tarball. Every
subsequent commit is a Darwin port patch with a documented rationale.
The publisher (`moonman81` on GitHub) claims authorship **only** of
the port patches; all original Heirloom code retains Gunnar Ritter's
attribution and its per-file licence header.

Port scope, as of publication:

- Darwin (macOS 26 arm64) portability (LP64, C23-clean, Mach + libproc
  where Solaris used `/proc` or kvm, Darwin-native mount-table walking,
  etc.)
- OpenSSL 3.x migration for the crypto code paths (where applicable
  to this package)
- Post-quantum crypto readiness for signature paths (where applicable)
- Y2K / Y2038 / large-file audit
- CWE / OWASP / ATT&CK / CAPEC-mapped hardening pass + legacy
  round-trip regression tests

The scope is **narrow**: whatever a single porter needed to get the
tools working on a personal Darwin machine, plus quality gates to
verify legacy artefacts (cpio, tar, SCCS, SVR4 pkg) still round-trip
correctly. There is no roadmap, no release cadence, and no promise
of ongoing maintenance beyond the porter's personal use.

## AI involvement in this port

Development of the port patches used substantial AI assistance. See
`AI-DISCLOSURE.md` in this repository for an honest breakdown of what
was human-directed and what was AI-generated, and the review posture
applied.

## Licensing

Heirloom uses a **per-file, patchwork licence** structure. See
`LICENSE/` (or top-level `LICENSE` file) in this repository for the
full set — unchanged from upstream. The main components:

- **`OPENSOLARIS.LICENSE`** — Common Development and Distribution
  Licence (CDDL) 1.0. Applies to the many `.c` files sourced from
  OpenSolaris. File-scoped.
- **`CALDERA.LICENSE`** — Caldera's 2002 licence for original Bell Labs
  Research Unix source code (v6, v7, 32V). BSD-style.
- **`LUCENT`** — Lucent Public Licence for Plan 9 material.
- **`COPYING`** / **`COPYING.LGPL`** — GNU General Public Licence and
  Lesser GPL for `awk` (in toolchest) and `libuxre` corners.
- **New code by Gunnar Ritter** — zlib-style: use freely, retain the
  copyright notice.

**Patch attribution + licensing**: port patches carry inline
`-- Heirloom Darwin port` comments identifying the delta from vendor.
These patches are offered under the **same licence as the file they
modify** — i.e. a patch to a CDDL-licensed OpenSolaris file is CDDL;
a patch to a zlib-licensed Ritter file is zlib. New files introduced
by the port (e.g. `mntinfo_darwin.c`, `p12lib_openssl3.c`,
`hdrs/darwin_stat_shim.h`, `libcommon/sys/mtio.h`) are offered under
**zlib-style** matching Ritter's convention for new code in
Heirloom.

## If you are the upstream maintainer or a licence-holder

If you have concerns about attribution, licensing, or the framing of
this repository, please open a GitHub issue. All commits are
individually revertible; per-file licence headers are unchanged from
upstream; the port patches can be redirected upstream via
`git format-patch` at any time.

## Third-party trademarks

**Unix** is a registered trademark of The Open Group. **Solaris** and
**OpenSolaris** are (historic) trademarks of Sun Microsystems / Oracle.
**Plan 9** is a trademark of Lucent Technologies. **macOS** and
**Darwin** are trademarks of Apple Inc. This repository makes no claim
to any such trademark; usage is descriptive-only per fair use.
