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

## Modality — version, variant, dialect

Every installed binary in this port honours a shared help / version /
variant / dialect flag set:

- `--help`, `--usage`, `-H`  → invoke `man(1)` on this tool
- `--version`, `-V`          → port banner (built variant + active variant)
- `--variants`               → list installed personality variants
- `--describe-modality`      → full modality matrix
- `--variant=<name>`, `HEIRLOOM_VARIANT=<name>`, `HEIRLOOM_DIALECT=<name>`
  → re-exec the requested variant
- `HEIRLOOM_PORT_VERSION_REQ=<version>` → pin scripts to a port revision

Recognised variants: `default` (SVID3), `posix` (SUS), `posix2001`
(SUS3), `s42` (SVID4-subset), `ucb` (UCB/BSD), `ccs`.

Recognised dialects: `svid3`, `svr3`, `svr4`, `sysv`, `sysv3`, `posix`,
`sus`, `sus2`, `posix2001`, `sus3`, `s42`, `svid4`, `ucb`, `bsd`, `ccs`.

Full reference: `man 7 heirloom-modality`.

## Documentation entry points

| File               | Content                                                 |
| :----------------- | :------------------------------------------------------ |
| `README.md`        | this file                                               |
| `INSTALL.md`       | short-form install guide                                |
| `HOWTO.md`         | narrative install + use walkthrough                     |
| `PROVENANCE.md`    | chain of custody (Bell Labs → AT&T → Sun → Ritter → …)  |
| `BIBLIOGRAPHY.md`  | references (papers, standards, historical texts)        |
| `NOTICE.md`        | licence patchwork + non-authoritative disclaimer        |
| `AI-DISCLOSURE.md` | degree of AI involvement in port authorship             |
| `GRATITUDE.md`     | acknowledgements                                        |
| `CHANGELOG.md`     | port revision history                                   |
| `SECURITY.md`      | vulnerability reporting posture                         |
| `CONTRIBUTING.md`  | how to contribute a patch                               |
| `skills/`          | Heirloom-port skills authored from this work            |
| `patches/`         | git-format-patch series (code repos only)               |
| `man/man7/`        | port-specific man pages (`heirloom-port-*.7`)           |
| `qa-reports/`      | committed QA snapshots (in workspace repo)              |

Every directory in this repo also carries its own `README.md`
describing its purpose in one page.

## Info-format overview

The port ships an Info document at
`/opt/heirloom/share/info/heirloom.info`. Read it with:

```sh
info heirloom
```

## Related repos

- <https://github.com/moonman81/heirloom-sh-darwin>
- <https://github.com/moonman81/heirloom-devtools-darwin>
- <https://github.com/moonman81/heirloom-toolchest-darwin>
- <https://github.com/moonman81/heirloom-doctools-darwin>
- <https://github.com/moonman81/heirloom-pkgtools-darwin>
- <https://github.com/moonman81/heirloom-workspace-darwin>

## Extended Heirloom Darwin Port universe

Three additional companion repos extend this port beyond the five
code + one workspace repos:

- <https://github.com/moonman81/heirloom-vi-darwin> — Ritter's ex/vi
  Darwin patches-only scaffold.
- <https://github.com/moonman81/heirloom-citations-darwin> — canonical
  reference documentation (Bell Labs CSTRs, BSTJ 1978, K&R draft).
- <https://github.com/moonman81/heirloom-ancestors-darwin> — manifests +
  notes for the ancestor source code (V7, 32V, DWB 1.0, PWB, PCC, BSDs).

The full universe map is at
<https://github.com/moonman81/heirloom-workspace-darwin/blob/main/TREE.md>.
