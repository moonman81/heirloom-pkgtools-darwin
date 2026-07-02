# Heirloom Packaging Tools — macOS / Darwin port

A **downstream** Darwin (macOS 26 arm64) port of Gunnar Ritter's
Heirloom **Packaging Tools**, heirloom-pkgtools-070227 release.

**This repository is not an original work.** It is not an authoritative
source. See `NOTICE.md` and `AI-DISCLOSURE.md` in the repo root for
the full disclosure of provenance, licensing, and AI involvement.

- **Upstream (authoritative):** <http://heirloom.sourceforge.net>
- **Upstream author:** Gunnar Ritter <gunnarr@acm.org> (unmaintained
  since 2008; upstream SourceForge tarballs remain the canonical
  reference)

## Contents

- Commit 1 (`3789e49`) — **pristine `heirloom-pkgtools-070227`** upstream tarball,
  extracted verbatim. No source modification.
- All subsequent commits — Darwin port patches, one logical change per
  commit with an inline rationale. See `git log`.

## Building on Darwin

Adjust `mk.config` (or `build/mk.config` for the toolchest) to point at
your desired install prefix (default is `/opt/heirloom`), then `make`.

Refer to the upstream `README` file inside this repository for the
build documentation authored by Gunnar Ritter. The workspace repo
<https://github.com/moonman81/heirloom-workspace-darwin> ships a
top-level Makefile that drives the five packages together.

## Licensing

Per-file, patchwork. See `LICENSE/` (or top-level `LICENSE` file) for
the full set — unchanged from upstream. Port patches inherit their
target file's licence.

## Warranty

**None.** As-is. No guarantee of originality, fitness, or safety. See
`NOTICE.md`.
