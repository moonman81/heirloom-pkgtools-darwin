# patches/

Every Darwin port patch as a standalone `.patch` file, plus a
cumulative `.diff` for consumers who prefer a single blob.

- `MANIFEST.md` — index + application instructions.
- `NNNN-*.patch` — per-commit patch files, `git format-patch` output.
  Numbered by application order; apply with `git am NNNN-*.patch`.
- `cumulative.diff` — full vendor → current diff. Apply with
  `patch -p1 < cumulative.diff` on a pristine `heirloom-pkgtools-070227` tarball.

The patches here are the same content as the equivalent range of
git commits on the repository's `main` branch — this directory is
a **materialised view of the port patches**, useful for:

- **Downstream porters** who want to apply the Darwin patches to
  their own vendor drop without cloning the whole repo.
- **Upstream contribution** — the patches are already in
  `git format-patch` form; submit via `git send-email` or copy
  into a maintainer's inbox.
- **Provenance audit** — visible-in-repo evidence that the port
  consists exactly of these patches, no more, no less.

## What's NOT in patches/

- The vendor baseline (`heirloom-pkgtools-070227`) — that's in commit 1 of this
  repo, verbatim from the upstream tarball.
- Housekeeping (README, NOTICE, CHANGELOG, etc.) — those are
  additive docs, not port patches, and would obscure the actual
  Darwin work if included.
- ALM Makefile + `scripts/` — additive infrastructure, not
  a modification of the vendor code.
