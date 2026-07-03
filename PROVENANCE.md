# PROVENANCE

Chain of custody for the `pkgtools` package (SVR4 package administration) as vendored by
this Darwin port.

> **Not authoritative.** This repository is a downstream port. See
> `NOTICE.md` for the licence patchwork and the scope disclaimer.

## Lineage

```
┌─────────────────────────────────┐  1969-1995
│  Bell Labs Research UNIX        │  Original authors:
│  V1..V7, System III, System V   │    Ken Thompson, Dennis Ritchie,
│  AT&T USG, USL                  │    Brian Kernighan, Bill Joy et al.
└──────────────┬──────────────────┘
               │  Public releases; some code
               │  reaches Caldera in 2002 under
               │  the Ancient UNIX licence.
               ▼
┌─────────────────────────────────┐  1994-2005
│  Sun Microsystems               │  OpenSolaris drops (CDDL-1.0);
│  Solaris / SunOS                │  many SVR4-lineage sources including
│                                 │  the `sh` bourne shell and the pkg
│                                 │  admin utilities became `usr/src/*`.
└──────────────┬──────────────────┘
               │  CDDL-licenced fragments.
               │  Ritter merges these with
               │  Bell Labs / BSD / GNU code.
               ▼
┌─────────────────────────────────┐  2001-2008
│  Gunnar Ritter                  │  Heirloom Project — heirloom.
│  Freiburg im Breisgau, Germany  │  sourceforge.net. Five sub-projects:
│                                 │    sh, devtools, toolchest,
│                                 │    doctools, pkgtools.
│                                 │  Last public release ≈ 2008;
│                                 │  effectively unmaintained since.
└──────────────┬──────────────────┘
               │  Pristine tarballs at
               │  /Volumes/heirloom/original-dist/.
               │  Unpacked into /opt/heirloom/src/original/
               │  before any port work began.
               ▼
┌─────────────────────────────────┐  2026-06-* → present
│  moonman81 (this repository)    │  Darwin port to macOS 26.4 arm64.
│  <i.am.moonman@gmail.com>       │  Prefix /opt/heirloom.
│                                 │  Fixes captured under patches/.
│                                 │  Port scaffolding under scripts/,
│                                 │  hardening/, GNUmakefile,
│                                 │  and the heirloom_flags.h shim.
│                                 │  AI-assisted (see AI-DISCLOSURE.md).
└─────────────────────────────────┘
```

## Verifiable trail

- **Upstream tarballs**: `/opt/heirloom/src/original/heirloom-pkgtools-*/`
  in the workspace repo. SHA-256 recorded in
  `qa-reports/checksums-upstream.txt` (workspace repo).
- **First commit of this repo**: the pristine tarball contents,
  unmodified, as `Initial import from vendor tarball`.
  Use `git log --all --oneline` to see the arc.
- **Patch series**: `patches/` (in code repos) contains individual
  hunks captured with `git format-patch`. Each patch header records
  the reason the change was needed on Darwin.
- **Port authorship**: everything after the initial import is
  authored by moonman81, subject to the disclaimers in `NOTICE.md`
  and `AI-DISCLOSURE.md`. Where AI assistance was used, the
  disclosure names the model + date range.

## Governance

- **No CLA**, no signing key required for contributions. See
  `CONTRIBUTING.md`.
- **No CI budget**. All quality gates run via `pre-commit` locally.
  See `.pre-commit-config.yaml`.
- **Reports committed to git**: any long-running scan output lives
  under `qa-reports/` so its evolution is visible via `git diff`.

## Related repos

- `moonman81/heirloom-sh-darwin`         — Bourne shell.
- `moonman81/heirloom-devtools-darwin`   — make/sccs/yacc/lex/m4.
- `moonman81/heirloom-toolchest-darwin`  — 200+ POSIX/SVR4 utilities.
- `moonman81/heirloom-doctools-darwin`   — troff/eqn/tbl/pic/grap/refer.
- `moonman81/heirloom-pkgtools-darwin`   — pkgadd/pkgrm/pkgchk etc.
- `moonman81/heirloom-workspace-darwin`  — shared hardening, ALM,
                                            QA reports, coverage matrix.

## Citing this port

For scholarly, licensing, or forensic purposes, cite:

- Upstream: `Ritter, Gunnar (2008). The Heirloom Project.
  http://heirloom.sourceforge.net/` for canonical semantics.
- This port: `moonman81 (2026). heirloom-pkgtools-darwin.
  https://github.com/moonman81/heirloom-pkgtools-darwin`
  for the Darwin-specific fixes only.

Do **not** cite this repo as authoritative source; the AI-assisted
port fixes are for compatibility, not for scholarship.
