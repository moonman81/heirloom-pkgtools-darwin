# libpkg

core pkg data-structure library.

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

- **C sources**: canonize.c, ckparam.c, ckvolseq.c, cvtpath.c, darwin_openssl_stubs.c, darwin_pwgr.c, dbsql.c, devtype.c, dstream.c, fmkdir.c, gpkglist.c, gpkgmap.c (+26 more)
- **Headers**: cfext.h, dbsql.h, dbtables.h, keystore.h, nhash.h, p12lib.h, pkgerr.h, pkglib.h, pkglibmsgs.h, pkglocale.h, pkgweb.h
- **Build**: Makefile, Makefile.mk

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
