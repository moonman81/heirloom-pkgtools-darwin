---
name: heirloom-per-file-license-patchwork
description: "Discipline for maintaining Heirloom's per-file, patchwork licence structure — CDDL (OpenSolaris) / Caldera Ancient Unix / Lucent (Plan 9) / GPL / LGPL / zlib (new Ritter code) coexisting across the same source tree. Port patches inherit their target file's licence. New files added by a Darwin port use zlib-style matching Ritter's convention. Never edit per-file copyright headers; never consolidate licences; document any exception."
gate: 3
version: "1.0.0"
author: moonman81
tags: [heirloom, licensing, cddl, caldera, lucent, gpl, lgpl, zlib, downstream-port]
depends_on: []
allowed-tools:
  - Read
  - Grep
  - Write
when_to_use: "Invoke when contributing to a Heirloom-derived repository, when writing a NOTICE / LICENSE file for a downstream port, or when auditing an inherited codebase for licence hygiene. Triggers: 'Heirloom licence', 'CDDL patchwork', 'per-file copyright', 'downstream Unix port', 'Gunnar Ritter licensing', 'zlib-style new code', 'do not consolidate licences'."
---

# Heirloom per-file license patchwork

## What Heirloom does

Gunnar Ritter's Heirloom Project deliberately preserves the **original
per-file licence** of every source file it draws from. There is **no
single project-level licence**. Instead, the tree contains a `LICENSE/`
directory (or a top-level `LICENSE` file) enumerating each licence
individually, and every source file carries its own copyright header.

Common licences in the mix:

- **CDDL 1.0** (`OPENSOLARIS.LICENSE`) — for the many `.c` files
  drawn from Sun's OpenSolaris release.
- **Caldera 2002 Ancient Unix** (`CALDERA.LICENSE`) — for original
  Bell Labs Research Unix source (v6, v7, 32V) redistributed under
  Caldera's terms.
- **Lucent Public Licence** (`LUCENT`) — for Plan 9-derived material
  (`pic`, `grap`, portions of `mpm`).
- **GPL / LGPL** (`COPYING`, `COPYING.LGPL`) — for `awk` (in
  toolchest) and `libuxre`.
- **zlib-style** (Ritter's own new code) — "use freely; retain the
  copyright notice."

Rationale (Ritter, `LICENSE/README`, 22 September 2003):

> "For something distributed as widely as Unix code, any license that
> requires more than naming the author would only cause annoyance."

## Rules for a Darwin-port maintainer

1. **Do not modify any per-file copyright header.** They document
   provenance. Reflowing them breaks `git blame` back to the original
   author.
2. **Do not consolidate licences.** Attempting to relicense the tree
   under a single umbrella (e.g. "all MIT now") requires permission
   from every licence-holder — impossible for a decades-old project.
3. **Do not remove any file from `LICENSE/`.** Not even one that
   looks unused. Downstream reviewers use them for attribution.
4. **Port patches inherit their target file's licence.** A patch to a
   CDDL-licensed OpenSolaris file is CDDL; a patch to a zlib-licensed
   Ritter file is zlib. State this explicitly in `CONTRIBUTING.md`.
5. **New files added by the port use zlib-style** matching Ritter's
   convention. Include a brief zlib-style header at the top:
   ```
   /* Copyright (c) YYYY <author>. Placed under zlib-style licence
    * matching Heirloom convention for new files. Use freely; retain
    * this notice. */
   ```
6. **Document any exception in `NOTICE.md`**, not silently.

## What a downstream port MUST publish

- The upstream `LICENSE/` directory verbatim.
- A `NOTICE.md` explaining the patchwork + which files carry which
  licence.
- Per-patch inline `-- Heirloom Darwin port` comments identifying the
  delta from vendor.

## Pattern in git

- Commit 1 = pristine vendor tarball extraction. Untouched
  `LICENSE/` tree.
- Later commits = port patches. **Never touch `LICENSE/*`** in a port
  commit — CI / pre-commit hook should refuse it (see
  `preserve-notice-files` local hook in the standard
  `.pre-commit-config.yaml`).

## Anti-patterns

- ❌ "Let's replace `LICENSE/` with a single `LICENSE` at the root
  under MIT" — you can't; the source is not yours to relicense.
- ❌ "Delete the CALDERA licence file; nobody uses those files
  anyway" — Caldera code IS still used in the tree; grep for
  `#include <sys/mtio.h>` or similar Bell Labs idioms.
- ❌ "Add SPDX-License-Identifier: GPL-3.0-or-later to every file" —
  the individual files are NOT GPL-3.0; adding an SPDX tag would
  misrepresent their licence.

## Compatible port examples

- `heirloom-workspace-darwin` and its 5 companion repos all follow
  this discipline. Every commit preserves `LICENSE/`; every new port
  file carries a zlib-style header; `NOTICE.md` enumerates the
  patchwork.

## Upstream

- Gunnar Ritter, `LICENSE/README` (22 September 2003).
- <http://heirloom.sourceforge.net> — canonical upstream.
