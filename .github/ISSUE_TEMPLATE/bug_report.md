---
name: Bug report (Darwin-specific)
about: A bug that meaningfully affects Darwin users of this port
title: '[bug] '
labels: bug
---

<!--
Before filing:
  * Check whether the same bug reproduces with pristine upstream
    Heirloom on a non-Darwin system. If yes, report upstream at
    http://heirloom.sourceforge.net instead — this repo is
    Darwin-only.
  * Check whether the bug is already listed in `CHANGELOG.md` or in
    the `hardening/` reports.
  * Do NOT file security-sensitive bugs here — use GitHub Security
    Advisory (see SECURITY.md).
-->

## What happens

<!-- A clear, minimal description of the observed behaviour. -->

## What you expected

<!-- What behaviour would have been correct. -->

## Reproduction

- Repository commit: <!-- `git rev-parse HEAD` -->
- macOS version: <!-- `sw_vers` -->
- Homebrew OpenSSL version: <!-- `openssl version` if applicable -->
- Build path: <!-- e.g. `make phase3` -->
- Exact command that triggers: <!-- fill in -->

## Output / diagnostics

<!-- Paste the compiler error, runtime output, or stack trace here.
     For crashes, an lldb backtrace helps. -->

```
```

## Additional context

<!-- Anything else — did the pre-commit hooks pass? Have you tried
     `pre-commit run --hook-stage manual --all-files`? -->
