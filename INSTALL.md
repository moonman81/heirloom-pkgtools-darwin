# INSTALL — SVR4 package administration (Heirloom Darwin port)

This document is the short-form install guide.  For the narrative
walkthrough (including the reasoning behind each step) read
`HOWTO.md` instead.

> **Not authoritative.**  This is a downstream Darwin port.  See
> `NOTICE.md` for scope and licence.

## Prerequisites

- **macOS 26.4** on Apple Silicon (arm64) — other Darwin releases
  may work but have not been tested by the port maintainer.
- **Xcode Command Line Tools**.  Install with `xcode-select --install`
  if you have not already.
- **Homebrew** (needed for the developer prerequisites; not for the
  built binaries).  See <https://brew.sh>.
- Read + write access to the install prefix (default `/opt/heirloom`).

Additional package prerequisites are listed in `scripts/prereqs-brew.txt`
and `scripts/prereqs-heirloom.txt`.

## Quick install

```sh
git clone https://github.com/moonman81/heirloom-pkgtools-darwin
cd heirloom-pkgtools-darwin
make bootstrap
make configure
make build
sudo make install PREFIX=/opt/heirloom
make verify
```

Substitute a different `PREFIX` if you want to install elsewhere.

## Lifecycle target reference

| Target       | Purpose                                                       |
| :----------- | :------------------------------------------------------------ |
| `bootstrap`  | install brew + upstream prerequisites (idempotent)            |
| `configure`  | verify environment + prepare `$(PREFIX)`                      |
| `build`      | compile via the upstream `makefile`                           |
| `install`    | install into `$(ROOT)$(PREFIX)` + write install manifest      |
| `verify`     | smoke-test the installed binaries                             |
| `uninstall`  | reverse `install` using the recorded manifest                 |
| `status`     | print repo state + installed-file counts                      |
| `snapshot`   | tag the current state in git (`snapshot-YYYY-MM-DD-HHMMSS`)   |
| `lifecycle`  | `bootstrap → configure → build → install → verify` in one go  |
| `clean`      | remove build products (`*.o`, per-tool binaries)              |
| `distclean`  | `clean` + remove generated `Makefile`s                        |

All targets are safe to re-run.  `install` is idempotent; `uninstall`
uses the manifest so it only removes what this port put down.

## Multi-variant install

The port installs up to five personality variants of every tool
(see `HOWTO.md` for the full list). By default, `make install` puts
all installable variants into the appropriate directory. To install
only a specific variant, pass:

```sh
make install VARIANTS=default
make install VARIANTS="default posix2001 ucb"
```

Recognised variant names: `default`, `posix`, `posix2001`, `s42`,
`ucb`, `ccs`.

## Verifying the install

```sh
make verify
```

Runs a per-tool smoke test and confirms:

- every binary starts + prints its version banner
- the shim honours `--help`, `--version`, `--variants`,
  `--describe-modality`
- `HEIRLOOM_VARIANT` re-exec works
- `HEIRLOOM_PORT_VERSION_REQ` pinning aborts when the version does
  not match

## Uninstall

```sh
sudo make uninstall PREFIX=/opt/heirloom
```

`uninstall` reads the manifest at `.install-manifest-pkgtools.txt`
in the repo root, removes each file listed, and does *not* touch
anything the port did not itself install.

## Reporting install problems

Open a GitHub issue at
`https://github.com/moonman81/heirloom-pkgtools-darwin/issues`
with:

- the output of `make status`
- the output of `sw_vers` and `uname -a`
- the exact failing invocation and its full stderr

## See also

- `README.md`         — repo overview
- `HOWTO.md`          — narrative install + use walkthrough
- `PROVENANCE.md`     — chain of custody
- `BIBLIOGRAPHY.md`   — reference list
- `NOTICE.md`         — licence patchwork + disclaimers
- `AI-DISCLOSURE.md`  — degree of AI involvement in port authorship
- `SECURITY.md`       — vulnerability reporting posture
- `man 7 heirloom-modality` — the modality reference
- `info heirloom`     — Info-format overview
