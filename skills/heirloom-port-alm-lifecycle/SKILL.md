---
name: heirloom-port-alm-lifecycle
description: "Application Lifecycle Management pattern for a Heirloom-style downstream port: GNUmakefile wrapper (takes precedence over upstream lowercase makefile) with bootstrap / configure / build / install / verify / uninstall / status / snapshot / lifecycle targets. Prerequisites captured in scripts/prereqs-brew.txt + scripts/prereqs-heirloom.txt as data. Install writes .install-manifest-<pkg>.txt for later uninstall. Verify runs a package-specific smoke test. All QA at pre-commit time (no CI budget model)."
gate: 3
version: "1.0.0"
author: moonman81
tags: [heirloom, alm, makefile, gnumakefile, pre-commit, bootstrap, install-manifest, no-ci]
depends_on:
  - heirloom-non-authoritative-downstream-port
allowed-tools:
  - Read
  - Write
  - Bash
when_to_use: "Invoke when scoping the developer + user lifecycle for a downstream port that needs to build, install, and uninstall reproducibly on a target platform. Triggers: 'make bootstrap', 'install manifest', 'lifecycle target', 'no CI budget', 'pre-commit as sole QA gate', 'GNUmakefile wrapping upstream makefile', 'idempotent uninstall'."
---

# Heirloom port ALM lifecycle

## The pattern

Each per-package repo carries:

- **`GNUmakefile`** — GNU make prefers this over `makefile`, so it
  takes precedence and adds lifecycle targets while delegating actual
  compile to the upstream `makefile`. BSD make + non-GNU systems
  continue to see the upstream `makefile` directly.
- **`scripts/`** — shell scripts implementing each verb.
- **`scripts/prereqs-brew.txt`** — Homebrew formulae, one per line.
  Data, not code.
- **`scripts/prereqs-brew-optional.txt`** — best-effort formulae.
- **`scripts/prereqs-heirloom.txt`** — companion Heirloom packages
  that must be installed first. Format `pkg-name:/absolute/check-path`.

## The eight verbs

| Verb | Purpose |
|---|---|
| `bootstrap` | Xcode CLT + Homebrew + brew formulae + pre-commit + companion Heirloom check + `/opt/heirloom` writability |
| `configure` | Validate toolchain, prefix, mk.config, companion packages; refuse if any hard prereq fails |
| `build` (`all`) | Delegate to upstream `makefile` |
| `install` | Delegate to upstream `install` target + write `.install-manifest-<pkg>.txt` |
| `verify` | Package-specific smoke tests on installed binaries |
| `uninstall` | Consume manifest; remove listed files; refuse if manifest missing or paths outside PREFIX |
| `status` | Report repo state + install state + prefix inventory |
| `snapshot` | git-tag `snapshot-YYYYMMDD-HHMMSSZ` if working tree clean |
| `lifecycle` | Run bootstrap + configure + build + install + verify in sequence |

Plus QA:

| Verb | Purpose |
|---|---|
| `test` | pre-commit fast + push tiers |
| `test-manual` | pre-commit manual tier (roundtrip + full lint sweep) |

## The install-manifest discipline

Standard `make install` deposits files under `PREFIX` but doesn't tell
you which files. `make uninstall` therefore has no safe way to know
what to remove.

Fix: after the upstream install runs, `scripts/write-manifest.sh`
enumerates every file + symlink under `ROOT$(PREFIX)` and writes them
to `.install-manifest-<pkg>.txt` at the repo root. `make uninstall`
then reverses using that manifest.

Safety rules the uninstall enforces:
- Refuse if the manifest is missing.
- Refuse if the manifest contains any path outside `PREFIX`.
- Never touch `/System`, `/usr`, or Homebrew paths.
- Keep the manifest in place after uninstall for audit trail.

## No-CI QA model

Some projects have no GitHub Actions budget. The port assumes this
and places **all QA at pre-commit time** on the contributor's
machine, tiered so routine commits stay fast:

- **pre-commit** (default) — file hygiene + codespell on docs+shell
- **pre-push** — shellcheck + cppcheck on changed C +
  `-Wformat-security` on changed C
- **manual** (opt-in) — full roundtrip suite + 17-tool lint sweep +
  format-string audit across whole tree

Reports land in `hardening/*/report-*.txt` **committed to git** — so
`git diff <old>..<new> -- hardening/` surfaces QA delta between
releases without needing an external report store.

## Workspace repo orchestrates 5 companions

For a 5-package Heirloom suite (sh + devtools + toolchest + doctools +
pkgtools), a **workspace repo** carries a top-level Makefile that
drives `phase1..phase5` over sibling companion repos:

```make
phase1:
    @$(MAKE) -C $(SIBLINGS)/heirloom-sh-darwin build install
phase2:
    @$(MAKE) -C $(SIBLINGS)/heirloom-devtools-darwin build install
# ... etc.
```

Plus `make clone-companions` to fetch missing sibling repos from
GitHub in one step.

## Why not one big monorepo

Per-package repos each carry only their own port patches → easier
upstream submission, cleaner git blame, smaller working copy for the
common case where a contributor only cares about one utility. The
workspace repo aggregates without duplicating.

## Idempotency

Every verb is designed to be safely re-runnable:

- `bootstrap` skips already-installed formulae + hooks.
- `configure` is read-only (never modifies).
- `install` writes a fresh manifest each time (previous is overwritten).
- `uninstall` skips files already removed.

## Prerequisites-as-data

The `prereqs-*.txt` files hold plain-text lists — one dep per line,
`#` comments allowed. Users can `cat`, `grep`, `wc`, diff between
releases. No YAML, no JSON, no schema. Ritter-style.
