# AI-DISCLOSURE

This port was developed with substantial AI assistance. This file gives
an honest account of what was human-directed and what was AI-generated,
so consumers of the repository can make their own risk assessment.

## Publisher

**Publisher (human):** `moonman81` on GitHub. Directs the port,
approves each phase and each change, reviews the diffs, decides which
findings to fix, defers, or reject, and takes final responsibility for
what is committed and published.

## AI assistant

**AI (assistant):** Anthropic Claude (Opus 4.7, 1M-context) driving
Claude Code, an interactive CLI. The AI produced the bulk of the
patch text, wrote the shims (`darwin_stat_shim.h`, `mntinfo_darwin.c`,
`p12lib_openssl3.c`, etc.), ran the compilers and interpreted their
output, executed the tests, and drafted the documentation and commit
messages.

## Interaction shape

The port was **not** an autonomous AI run. It was an interactive
pair-programming session where the human:

- Named the goal ("port Heirloom to Darwin", then progressively:
  "audit for Y2K/Y2038/LFS", "path C (full pkgtools port)", "add PQ
  crypto", "install every brew linter", "split into per-package
  repos", etc.).
- Answered scoping questions before the AI proceeded (install prefix,
  utmpx strategy, personality set, licence framing, repo naming,
  visibility).
- Reviewed the AI's proposed patches and phase reports.
- Interrupted and course-corrected several times (e.g. rejecting a
  parallel build attempt; asking for interpretation before
  refactoring; requiring backward-compat and future-config
  discipline).
- Made the scope calls at bailout points (option A/B/C decisions for
  pkgtools, D1/D2/D3 decisions after Phase 5, etc.).

## Concretely — what was AI-generated vs human-directed

### Human-directed

- **Every phase boundary**, decision point, scope decision, and
  release gate.
- **Every architectural choice** the port makes (six personalities,
  `/opt/heirloom` prefix, libproc for `ps`/`whodo`, rewriting mntinfo
  Darwin-native instead of shimming, using fresh
  `p12lib_openssl3.c` rather than surgical port of legacy p12lib.c,
  supporting PQ crypto by removing algorithm gating).
- **Every deferral decision** (`sunw_PKCS12_create`,
  make/vroot race conditions, full AFL fuzzing).
- **The framing of this NOTICE + AI-DISCLOSURE** — the fact of AI
  involvement, the need for a "not authoritative" disclaimer, the
  demand for warranty disclaimers.

### AI-generated (with human review before commit)

- **Almost all C code** in the port patches and the new Darwin-native
  files (`mntinfo_darwin.c`, `p12lib_openssl3.c`,
  `hdrs/darwin_stat_shim.h`, `libcommon/sys/mtio.h`,
  `libpkg/darwin_pwgr.c`, `libpkg/darwin_openssl_stubs.c`).
- **Every inline `-- Heirloom Darwin port` explanatory comment**.
- **All commit messages** (the human approved them by not asking for
  changes).
- **PORT.md** and all `hardening/*/COVERAGE.md` documents.
- **The lint-sweep + roundtrip + fuzz-seed shell scripts**.
- **This NOTICE.md and this AI-DISCLOSURE.md**.

### Assisted / mixed

- **Bug-finding** was interactive: the AI ran the compiler, the human
  approved the fix approach, the AI wrote the patch. The three
  behaviour bugs surfaced during porting (`id.c argc<optind>1`,
  `n7.c` LP64 NULL+j, three CWE-134 format-string sites) were
  spotted by AI-driven compiler runs and confirmed via human review.
- **Testing** was AI-driven but the test **design** (round-trip
  legacy artefacts; CWE/OWASP/ATT&CK/CAPEC framework mapping) was
  human-directed.

## Review posture

- **Each phase's changes were reviewed by the human before the next
  phase started.** No accumulation of unreviewed diffs.
- **Each commit was proposed by the AI, shown to the human, and
  committed only after implicit approval** (the human did not
  request changes before proceeding).
- **No independent human line-by-line audit** of the AI-produced C
  code has been performed. The human's review posture is "read the
  diff, spot-check for obvious errors, trust the compile-and-test
  gate to catch regressions". This is standard pair-programming
  discipline, not formal review.
- **Post-commit automated quality gates** (ASan + UBSan on cpio
  round-trip, clang `-Wformat-security`, cppcheck, splint,
  flawfinder, clang-tidy, codespell, shellcheck, iwyu) all report
  clean-or-documented-deferred. These are secondary checks, not a
  substitute for careful human review.

## What this means for you

- **If you plan to run this code in production**: don't, without
  your own independent review. The AI-generated shim layer
  (particularly `p12lib_openssl3.c` and `mntinfo_darwin.c`) is
  ~800 lines of unreviewed C touching security-critical paths
  (PKCS#12 parsing, mount-table walking). ASan+UBSan on happy-path
  inputs is not sufficient evidence of correctness on adversarial
  inputs.
- **If you plan to submit patches upstream to Heirloom**: mention the
  AI-assisted origin so Gunnar Ritter (or a future maintainer) can
  apply their own review posture.
- **If you plan to cite this repository academically**: cite it as an
  AI-assisted Darwin port; the pristine reference is upstream at
  `heirloom.sourceforge.net`.
- **If you find a bug**: open a GitHub issue. The human publisher
  will triage; the AI may participate in the fix.

## Model attribution + toolchain

- **Model:** Claude Opus 4.7 (1M-context), Anthropic. Knowledge
  cutoff Jan 2026. Interactive session across multiple conversation
  windows in early July 2026.
- **Harness:** Claude Code CLI.
- **Human environment:** macOS 26.4.1 (Darwin 25.4.0) on arm64;
  Apple clang 21; Homebrew OpenSSL 3.6.2.
- **The AI does not have persistent state across sessions.** Any
  future update to this repository from a fresh AI session will not
  have direct memory of this development history; it will rely on
  git log, PORT.md, and the memory notes stored by the human's
  Claude Code instance.

## In short

This is a **useful, honestly-labelled, AI-assisted port** intended for
individual Darwin use. It is not a warranty-backed vendor deliverable
and should not be treated as one.
