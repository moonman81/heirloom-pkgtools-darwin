# HOWTO — install and use Heirloom Packaging Tools on Darwin

A narrative guide covering the whole port lifecycle from clone to
uninstall. If you want just the terse reference, read the man pages
under `man/`; if you want the reasoning behind design choices, read
`NOTICE.md`, `AI-DISCLOSURE.md`, and (in the workspace repo) `PORT.md`.

## 1. First-time install

### 1.1. Clone

```sh
git clone https://github.com/moonman81/heirloom-pkgtools-darwin
cd heirloom-pkgtools-darwin
```

### 1.2. Bootstrap

```sh
make bootstrap
```

This will:

- confirm Xcode Command Line Tools are installed (auto-triggers the
  installer if not)
- confirm Homebrew is installed (won't auto-install brew; that's
  your policy call — see <https://brew.sh> if you need it)
- install all required Homebrew formulae (list at
  `scripts/prereqs-brew.txt`)
- install optional formulae, best-effort
- install `pre-commit` and wire up the git hooks
- check `/opt/heirloom` writability and print the exact `sudo`
  commands to create it once with your user as owner

If `bootstrap` reports that `/opt/heirloom` is not writable, run the
`sudo install -d ... && sudo chown ...` commands it prints, then
re-run `make bootstrap` (it's idempotent).

### 1.3. Configure

```sh
make configure
```

Validates the toolchain, checks the prefix, verifies companion
Heirloom packages are already installed (if this package depends on
any), and shows `mk.config` prefix settings. Prints the first hard
failure it finds; fix, then re-run.

### 1.4. Build

```sh
make build
```

Delegates to the upstream `makefile`, which is the same build system
Gunnar Ritter maintained. Nothing in `heirloom-pkgtools-darwin` alters
the upstream build system beyond `mk.config` values and the source
patches captured under `patches/`.

### 1.5. Install

```sh
make install
```

Installs into `/opt/heirloom` (or wherever `PREFIX` points) and
writes `.install-manifest-pkgtools.txt` at the repo root. That
manifest is consumed by `make uninstall`; don't delete it.

For a staging build (e.g. into a tarball root without touching the
live prefix):

```sh
make install ROOT=/tmp/pkgroot
```

### 1.6. Verify

```sh
make verify
```

Runs a package-specific set of smoke tests against the installed
binaries. Should complete cleanly in a few seconds.

### 1.7. Do it all in one command

```sh
make lifecycle
```

Runs bootstrap + configure + build + install + verify end-to-end.

## 2. Everyday rebuilds

Once bootstrap has run at least once, day-to-day iteration is:

```sh
# after editing source
make build
make install
make verify
```

Or, for a clean rebuild:

```sh
make clean
make build install verify
```

## 3. Quality assurance

**All QA runs at pre-commit time — there is no CI budget.** Once you
have run `pre-commit install` (which `make bootstrap` does), every
commit and every push runs the configured hooks locally:

| Stage | Runs | Cost |
|---|---|---|
| pre-commit | file hygiene, typo check on docs+shell, LICENSE/NOTICE preservation | fast |
| pre-push | shellcheck, cppcheck on changed C, `-Wformat-security` on changed C | seconds |
| manual (opt-in) | full roundtrip suite + 17-tool lint sweep + format-string audit whole tree | minutes |

Run the manual tier before tagging a release:

```sh
make test-manual         # same as: pre-commit run --hook-stage manual --all-files
```

Reports land in `hardening/*/report-*.txt` (in the workspace repo) —
these are checked into git so `git diff <old>..<new> -- hardening/`
shows the QA delta between two releases.

## 4. Snapshots + releases

```sh
make snapshot            # git tag snapshot-YYYYMMDD-HHMMSSZ
git push origin <tag>    # publish it
```

Snapshots are lightweight; use them freely as automation markers.
Semantic-version tags (`v0.1.0` etc.) are reserved for release
milestones documented in `CHANGELOG.md`.

## 5. Uninstall

```sh
make uninstall
```

Reads `.install-manifest-pkgtools.txt`, removes every listed file
and symlink, and prunes empty directories under `PREFIX`. The
manifest is kept in place for audit; delete it by hand if desired.

`make uninstall` refuses if the manifest is missing (there is no
safe way to know what to remove) or if the manifest contains paths
outside `PREFIX`.

## 6. Status + inventory

```sh
make status
```

Reports repo state (HEAD SHA, dirty/clean, tag on HEAD), install
state (manifest present? how many files still there?), and prefix
inventory (per-personality-dir entry counts).

## 7. Standalone patch application

If you want to apply the Darwin port patches to your own pristine
vendor drop without cloning this repo:

```sh
# 1. Get the vendor tarball
curl -O http://heirloom.sourceforge.net/.../heirloom-pkgtools-070227.tar.bz2
tar xjf heirloom-pkgtools-070227.tar.bz2
cd heirloom-pkgtools-070227

# 2. Initialise git so you can apply the patches
git init && git add -A && git commit -m "vendor: pristine heirloom-pkgtools-070227"

# 3. Apply every port patch in order
git am /path/to/heirloom-pkgtools-darwin/patches/*.patch
```

Or apply the cumulative single-file diff without git:

```sh
cd heirloom-pkgtools-070227
patch -p1 < /path/to/heirloom-pkgtools-darwin/patches/cumulative.diff
```

See `patches/MANIFEST.md` for the per-patch index.

## 8. Contributing back

- Read `CONTRIBUTING.md` for the scope discipline (what's in scope
  here vs. what should go upstream).
- Read `AI-DISCLOSURE.md` before contributing — the honesty posture
  applies to your contributions too.
- Non-Darwin changes should go upstream at
  <http://heirloom.sourceforge.net>, not here.

## 9. Getting help

- **Bug reports (Darwin-specific)** — GitHub issue, `bug_report.md` template.
- **Attribution / licensing concerns** — GitHub issue,
  `attribution_concern.md` template. Prioritised.
- **Security vulnerabilities** — GitHub Security Advisory (private
  channel). See `SECURITY.md`.

## 10. Related documentation

- `README.md` — one-paragraph overview.
- `NOTICE.md` — legal, licensing, attribution.
- `GRATITUDE.md` — thanks to the corpus authors.
- `AI-DISCLOSURE.md` — honest AI-involvement account.
- `CHANGELOG.md` — release history.
- `CONTRIBUTING.md` — contribution scope + PR discipline.
- `SECURITY.md` — vuln-reporting posture.
- `man/*.7` — per-command reference.
- `patches/MANIFEST.md` — per-patch index.
- Workspace repo `PORT.md` — full decisions record for the port.

## Modality — version, variant, dialect

Every installed binary in this port honours a shared set of flags
and environment variables that let you pick which SVR4 personality
you want at invocation time:

| Flag                       | Purpose                                        |
| :------------------------- | :--------------------------------------------- |
| `--help`, `--usage`, `-H`  | invoke `man(1)` on this tool                   |
| `--version`, `-V`          | print the port banner + built + active variant |
| `--variants`               | list installed personality variants            |
| `--describe-modality`      | print the modality matrix for this tool        |
| `--variant=<name>`         | re-exec the requested variant                  |
| `--dialect=<name>`         | human-friendly synonym for `--variant`         |

Environment variables (highest wins first):

1. `HEIRLOOM_VARIANT=<name>`
2. `HEIRLOOM_DIALECT=<name>`
3. `SYSV3` (Ritter's classic SVID3 selector)
4. `HEIRLOOM_PORT_VERSION_REQ=<version-string>` — pin a script to a
   specific port revision; the tool exits with `EX_CONFIG (78)` if
   the running port version does not match.

Variants (directory-based):

| Variant name  | Path                              | Behavioural style        |
| :------------ | :-------------------------------- | :----------------------- |
| `default`     | `$PREFIX/bin/<tool>`              | SVID3                    |
| `posix`       | `$PREFIX/bin/posix/<tool>`        | POSIX/SUS                |
| `posix2001`   | `$PREFIX/bin/posix2001/<tool>`    | POSIX-2001/SUS3          |
| `s42`         | `$PREFIX/bin/s42/<tool>`          | SVID4 subset             |
| `ucb`         | `$PREFIX/ucb/<tool>`              | UCB / BSD                |
| `ccs`         | `$PREFIX/ccs/bin/<tool>`          | CCS (`make`, `sccs`, …)  |

Recognised dialects (mapped to variants):

- `svid3`, `svr3`, `svr4`, `sysv`, `sysv3` → `default`
- `posix`, `sus`, `sus2`                     → `posix`
- `posix2001`, `sus3`                        → `posix2001`
- `s42`, `svid4`                             → `s42`
- `ucb`, `bsd`                               → `ucb`
- `ccs`                                      → `ccs`

Full detail: `man 7 heirloom-modality`.

### Examples

```sh
# Get the manual page for a tool via the shim.
ls --help

# Check the port revision.
pkgadd --version

# See which variants are installed for cp.
cp --variants

# Force BSD-flavour behaviour for one invocation.
HEIRLOOM_DIALECT=bsd ls -la

# Same via long flag.
ls --variant=ucb -la

# Pin a script to a specific port revision.
HEIRLOOM_PORT_VERSION_REQ=1.1.0-darwin-arm64 sh script.sh
```

## Info-format documentation

Alongside the man pages, the port ships an Info-format guide at
`/opt/heirloom/share/info/heirloom.info`. Read it with:

```sh
info heirloom
```

The Info document is a companion, not a replacement, for the per-tool
man pages. When in doubt, prefer `man <tool>`.

## References

- Man pages:            `/opt/heirloom/share/man/5man/`
- Info-format guide:    `/opt/heirloom/share/info/heirloom.info`
- Skills catalogue:     `skills/<skill-name>/SKILL.md`
- Provenance:           `PROVENANCE.md`
- Bibliography:         `BIBLIOGRAPHY.md`
- Coverage matrix:      workspace repo's `hardening/COVERAGE-MATRIX.md`
