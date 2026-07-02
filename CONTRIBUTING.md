# Contributing

Thanks for considering a contribution. Before you invest much effort,
please note the **scope discipline** of this repository — many changes
useful in a general Unix port are **out of scope here** and are best
directed upstream instead.

## Read these first

- **`README.md`** — plain overview.
- **`NOTICE.md`** — sole purpose (Darwin port), not authoritative,
  no warranty / originality / fitness guarantee, licence patchwork,
  upstream attribution to Gunnar Ritter.
- **`AI-DISCLOSURE.md`** — how the port was developed with AI
  assistance and what that means for review posture.
- **`CHANGELOG.md`** — release history.

## In scope

Changes that make this repository more useful for **Darwin users** of
Heirloom, without breaking upstream compatibility:

- ✅ Additional Darwin portability fixes (LP64, C23, modern clang
  warnings)
- ✅ Additional OpenSSL 3.x accessor migrations
- ✅ Additional post-quantum crypto integration
- ✅ Additional hardening — CWE Top 25 / OWASP Top 10 / MITRE ATT&CK
  / CAPEC remediation
- ✅ Additional legacy round-trip tests
- ✅ Additional fuzz seeds for parsers
- ✅ Documentation improvements
- ✅ Bug reports (see issue templates)

## Out of scope

Direct these upstream at <http://heirloom.sourceforge.net>:

- ❌ Non-Darwin portability
- ❌ New utility features or CLI flags
- ❌ Style / formatting reflows of upstream code
- ❌ Rewrites of upstream utilities in a modern style
- ❌ Removing / re-licensing any upstream `LICENSE/`, `COPYING`,
  `NOTICE`, or per-file copyright header

## Quality assurance

**All QA in this project runs at pre-commit time — there is no CI
budget** (no GitHub Actions minutes). You are expected to have
`pre-commit` installed and to have run it before pushing:

```
# One-time per clone
pip install --user pre-commit    # or: brew install pre-commit
pre-commit install --install-hooks
pre-commit install --install-hooks --hook-type pre-push
```

The hook tiers are:

| Stage | Hooks | When |
|---|---|---|
| **pre-commit** (default) | whitespace, EOF, large-file, merge-marker, YAML/TOML/JSON syntax, codespell, LICENSE + NOTICE preservation | Every commit — fast |
| **pre-push** | shellcheck, cppcheck on changed C, `-Wformat-security` on changed C | Every `git push` |
| **manual** | full roundtrip suite, full 17-tool lint sweep, format-string audit across whole tree | Before tagging a release: `pre-commit run --hook-stage manual --all-files` |

If any pre-commit hook fails, fix locally before pushing. Reviewers
will not merge a PR whose author skipped hooks — the review posture
is "the tooling caught the mechanical issues; I focus on the
substantive ones".

## How to propose a change

1. **Open an issue first** for anything non-trivial. Discussion first,
   PR after — saves your time if the change is out of scope.
2. **One logical change per commit.** Match the existing commit-message
   style — short subject in imperative mood, body with rationale,
   inline `-- Heirloom Darwin port` comment in the code where the
   change is not self-explanatory.
3. **Preserve legacy artefact interop.** If your change touches
   `cpio`, `tar`, SCCS, or SVR4 pkg, ensure the roundtrip suite
   still passes: `pre-commit run --hook-stage manual roundtrip-suite`.
4. **Update `CHANGELOG.md`.** Add your change under the `[Unreleased]`
   section following Keep-a-Changelog convention.
5. **Run the hooks locally** — see above.
6. **Disclose AI involvement** if applicable. If you used an AI
   assistant to author part of your patch, note it in the PR
   description. Follow the honesty posture set by `AI-DISCLOSURE.md`.

## Licensing your contribution

Contributions inherit their **target file's** licence — a patch to a
CDDL-licensed OpenSolaris file is CDDL, a patch to a zlib-licensed
Ritter file is zlib. New files should be zlib-style (matching
Ritter's convention for new Heirloom code) unless there is a strong
reason otherwise.

By submitting a PR, you confirm that:

- You have the right to license your contribution under the terms
  above.
- You have disclosed any AI involvement in producing the patch.
- You accept that the port is maintained "at porter's discretion" —
  no guaranteed merge, no SLA on review.

## Review posture

- Maintainer reviews are **human-attentive but not exhaustive**.
  Pre-commit hooks catch mechanical errors; behavioural correctness
  of C changes is judged case-by-case against the compiler +
  roundtrip + ASan+UBSan output.
- For anything security-critical (setuid utilities, crypto paths,
  archive parsing), expect a slower review and possibly a request
  for a fuzzing pass.
- Expect the maintainer to sometimes redirect a PR upstream to
  Ritter's SourceForge project (or its inheritors) rather than
  merging here.

## Getting help

- Open a GitHub issue.
- Reference `PORT.md` (workspace repo) for the port-wide decisions
  register.
