---
name: heirloom-non-authoritative-downstream-port
description: "Framing discipline for a downstream Darwin/macOS (or other platform) port of Gunnar Ritter's Heirloom Project. Publisher claims authorship only of the port patches; upstream heirloom.sourceforge.net remains the authoritative source. NOTICE.md, README.md, AI-DISCLOSURE.md, and GRATITUDE.md must all state this posture explicitly. No warranty, no guarantee of originality, no guarantee of fitness. Applies to any port/fork of unmaintained-but-important legacy code."
gate: 3
version: "1.0.0"
author: moonman81
tags: [heirloom, downstream-port, attribution, framing, non-authoritative]
depends_on:
  - heirloom-per-file-license-patchwork
allowed-tools:
  - Read
  - Write
when_to_use: "Invoke when publishing a downstream port of Heirloom or any similarly-unmaintained legacy upstream (Bell Labs Ancient Unix, MINIX utilities, Plan 9, etc.). Triggers: 'publish port to GitHub', 'not an authoritative source', 'downstream fork', 'legacy code republication', 'NOTICE.md for a port', 'attribution posture'."
---

# Non-authoritative downstream port — framing discipline

## The problem

When an unmaintained upstream (Heirloom hasn't seen a release since
2008) needs to keep working on modern platforms, someone
has to port it. But publishing that port on GitHub creates a
confusion risk: a first-time reader might mistake your port for the
canonical Heirloom.

The framing discipline below prevents that confusion, honestly
represents the AI-assisted authorship of many ports, and preserves
attribution to the original authors and licence-holders.

## The four files

Every repo in the port publishes:

1. **`README.md`** — one-paragraph overview. First line explicitly
   says "downstream port; not an authoritative source".

2. **`NOTICE.md`** — three sections:
   - **Sole purpose** = macOS/Darwin (or your platform) port.
   - **Not authoritative** — canonical upstream is
     `heirloom.sourceforge.net`; anyone wanting the reference for
     academic study, legal citation, security review, or
     compatibility testing should go there directly, not here.
   - **No warranty. No guarantee of originality. No guarantee of
     fitness.** As-is.

3. **`AI-DISCLOSURE.md`** — honest account of AI involvement in
   producing the port patches. Distinguish human-directed decisions
   from AI-generated content; state the review posture; state what
   this means for anyone considering production use.

4. **`GRATITUDE.md`** — thanks to the original authors, licence-
   holders, and luminaries whose corpus you built on. Include: the
   upstream maintainer (Ritter), Bell Labs authors (Thompson,
   Ritchie, McIlroy, Pike, Kernighan, Ossanna, Lesk, Cherry, Bourne,
   Aho, Weinberger, Johnson, Morris, Feldman), Plan 9, Berkeley
   CSRG, Sun/OpenSolaris, Caldera, MINIX, Info-ZIP, GNU. Also thank
   the modern toolchain that made the port possible.

## What TO claim

- Authorship of the port patches themselves.
- Judgement / discipline in how you selected + reviewed the
  contributions.
- Publishing choices — which repos, which naming, which visibility.

## What NOT to claim

- Authorship of Heirloom itself. That belongs to Ritter and the
  many upstream contributors.
- Authorship of the code the AI wrote — disclose it as AI-generated,
  not as your own.
- Fitness for purpose. You have not run enterprise QA; do not imply
  you have.
- Vendor-level warranty. This is a personal port; state so.

## The `bug_report.md` + `attribution_concern.md` templates

Every repo should carry an issue template that makes it easy for:
- **`bug_report.md`** — normal users to report Darwin-specific bugs
  and only Darwin-specific bugs (upstream bugs go to
  heirloom.sourceforge.net, not here).
- **`attribution_concern.md`** — the upstream maintainer, licence-
  holders, or their estates to reach the publisher. Mark as
  priority-high.

## The pre-commit gate

`.pre-commit-config.yaml` should include a `preserve-notice-files`
local hook that refuses deletion of `README.md`, `NOTICE.md`,
`AI-DISCLOSURE.md`, `GRATITUDE.md`, or `LICENSE`. Deletion of any of
these should be a deliberate act with a documented reason, not an
accidental commit.

## Why this matters

The alternative — publishing without this framing — invites at least
three failure modes:
- **Confusion of authority.** Someone cites your fork instead of
  upstream in a paper or a court case.
- **Warranty claim.** Someone treats the code as production-ready
  because you didn't disclaim.
- **Attribution grievance.** An original author or licence-holder
  finds the repo and reasonably objects to the framing.

All three are avoidable with 4 files and about 500 words of writing.

## Applies more broadly than Heirloom

The pattern generalises to any downstream port of an unmaintained
upstream: MINIX, Plan 9, Ancient Unix, DECUS libraries, TENEX
utilities, TOPS-20 sources, old BSD releases. When the upstream is
quiet and the code is important, someone will republish; do it
honestly.
