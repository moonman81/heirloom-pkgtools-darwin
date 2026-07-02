<!--
Thanks for the contribution. Please fill in this template — it saves
review-round-trip time. If you have not already, read CONTRIBUTING.md.
-->

## Summary

<!-- One or two sentences: what does this PR change, and why? -->

## Scope check

<!-- Confirm this PR is IN SCOPE per CONTRIBUTING.md. Out-of-scope
     items belong upstream at heirloom.sourceforge.net. -->

- [ ] Darwin-specific change (not applicable to other Unix targets)
- [ ] Preserves upstream `LICENSE/` files unchanged
- [ ] Preserves per-file copyright headers unchanged
- [ ] Does not reflow / rewrap upstream style (blame stays with Ritter)

## Pre-commit hooks

- [ ] `pre-commit install` performed on this clone
- [ ] `pre-commit install --hook-type pre-push` performed
- [ ] `pre-commit run --all-files` passes locally
- [ ] `pre-commit run --hook-stage manual --all-files` run — hardening
      reports refreshed and committed (if any changed)

## Legacy artefact preservation

<!-- If this touches cpio / tar / SCCS / SVR4 pkg / troff format
     handling, tick this box after re-running the roundtrip suite. -->

- [ ] `pre-commit run --hook-stage manual roundtrip-suite` passes
- [ ] Not applicable — no on-disk / on-tape format touched

## Licence declaration

<!-- Tick one. -->

- [ ] Patch inherits its target file's licence (default)
- [ ] New file(s) added under zlib-style, matching Ritter's convention
- [ ] Other — explain in the PR body

## AI-DISCLOSURE

<!-- Per AI-DISCLOSURE.md posture: disclose any AI involvement in
     producing this patch. -->

- [ ] No AI assistance was used in authoring this patch
- [ ] AI assistance was used — describe below (which tool, which
      parts, what review posture applied)

<details><summary>Details</summary>

...

</details>

## Testing / evidence

<!-- What did you do to convince yourself this works? Compile output,
     smoke-test transcript, hardening report diff, benchmark, etc. -->

## Related issue

<!-- Link to any related GitHub issue: `Fixes #NN` / `Related #NN`. -->
