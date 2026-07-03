# libpkgdb

embedded SQLite fork for the pkg install database.

## Where this fits

This directory is part of `moonman81/heirloom-pkgtools-darwin`, the
Darwin port of the pkgtools package from Gunnar Ritter's Heirloom
Project. See the repo root `README.md`, `PROVENANCE.md`, and
`NOTICE.md` for context.

**Not authoritative.** Upstream is
`http://heirloom.sourceforge.net/` (unmaintained since ≈ 2008).
Port fixes here are for macOS 26.4 arm64 compatibility, not for
new feature work.

## Contents

- **C sources**: attach.c, auth.c, btree.c, btree_rb.c, build.c, copy.c, darwin_placeholder.c, delete.c, encode.c, expr.c, func.c, hash.c (+17 more)
- **Headers**: btree.h, hash.h, heirloom_flags.h, os.h, pager.h, sqlite.h, sqliteInt.h, vdbe.h
- **Build**: Makefile, Makefile.mk
- **Subdirs**: tool

## Modality

Every installed binary honours the shared help / version / variant
/ dialect flag set:

- `--help`, `--usage`, `-H`  → man page
- `--version`, `-V`          → port banner (built variant + active variant)
- `--variants`               → list personality variants installed
- `--describe-modality`      → full modality matrix
- `--variant=<name>`, `HEIRLOOM_VARIANT=<name>`, `HEIRLOOM_DIALECT=<name>`
  → re-exec into the requested personality binary

See `heirloom_flags.h` (in each source directory) for the shared shim.

## Licence

Per-file patchwork — CDDL-1.0 / Caldera / Lucent / GPL-2.0-or-later /
LGPL-2.0-or-later / zlib. See headers on each source file and the
per-package `NOTICE.md`.
