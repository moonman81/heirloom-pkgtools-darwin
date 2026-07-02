# GRATITUDE

This repository exists only because generations of exceptional
programmers, mathematicians, and licence-holders chose to make their
work available for others to build on. What follows is an attempt at
honest thanks — inevitably incomplete, but sincerely meant.

## To the upstream maintainer

**Gunnar Ritter** (Freiburg im Breisgau, Germany) — for authoring and
patiently maintaining the Heirloom Project over years of unpaid work,
for the discipline of preserving traditional Unix behaviour when the
industry was busy replacing it, and for the "there is really nothing
to keep secret about it" spirit in the LICENSE/README that made this
port possible.

The upstream project has been unmaintained since 2008. This downstream
port would be impossible without his prior generosity. Any error or
misjudgement in the port is on the porter, not on him.

## To the corpus authors

Unix is not one person's work. Heirloom is a stitching-together of
material from many hands, over many decades. In rough chronological
and dispositional order:

**Bell Labs Research Unix (1970s onwards)** — Ken Thompson and Dennis
Ritchie for `sh`, `cp`, `mv`, `rm`, `cat`, `ed`, and the shape of
everything that followed. Doug McIlroy for pipes and for the design
principles that still hold. Rob Pike for `troff` maintenance, `pic`,
and a lifetime of clarity. Brian Kernighan for `awk` (with Alfred
Aho and Peter Weinberger), `pic`, `grap` (with Jon Bentley), `troff`
maintenance, and every book. Joseph Ossanna for the original `nroff`
and `troff`. Steve Johnson for `yacc`. Mike Lesk for `lex`, `tbl`,
`refer`, and `uucp`. Lorinda Cherry for `eqn` (with Kernighan) and
`bc`/`dc`. Robert Morris for `dc`. Stu Feldman for `make`. Steve
Bourne for `sh`. Bill Joy, Ken Arnold, and colleagues at Berkeley
for `vi`, `csh`, `more`, `curses`, and countless small utilities.
Marshall Kirk McKusick for the Berkeley Fast File System that shaped
how these tools handle files.

**Plan 9 (Bell Labs, 1980s–90s)** — Rob Pike, Ken Thompson, Dennis
Ritchie, Dave Presotto, Phil Winterbottom, Tom Duff, Sean Dorward,
and others. Heirloom uses `pic`, `grap`, `mpm`, and portions of
their troff work.

**Berkeley CSRG (Computer Systems Research Group, UCB)** — the entire
4BSD-era faculty and student body. Special mention to Kirk McKusick
for the 4.4BSD release under BSD licence.

**Sun Microsystems + OpenSolaris (2005 onwards)** — the SunOS/Solaris
Unix engineering community whose CDDL-released code is the largest
single input to Heirloom. Individual attribution is preserved in the
per-file OpenSolaris copyright headers.

**Caldera / SCO** — for the 2002 Ancient Unix release that made v6,
v7, and 32V source code redistributable. This is why we still have
the original Bell Labs code, not just a rewrite.

**MINIX (Andrew Tanenbaum et al)** — for the MINIX utility
collection Ritter drew from. Tanenbaum's "The Design of the UNIX
Operating System" and his generosity in releasing MINIX's source
educated many of the people whose work we are still using.

**Info-ZIP** — for the compression codes.

**GNU Project / Free Software Foundation** — the GNU-licensed corners
of Heirloom (`awk`, `libuxre`) are courtesy of the FSF's decision to
make regular-expression libraries and later awk lineages freely
redistributable.

## To the mathematicians and typography luminaries

**Donald Knuth** — for `TeX`, for `The Art of Computer Programming`,
for the aesthetic sensibility that shaped how we think about
typography, and (through Frank Liang, his student) for the
hyphenation-pattern algorithm the Heirloom `libhnj` implements. The
`hyph_en_US.dic`, `hyph_fr_FR.dic`, and other pattern files derive
directly from Knuth's `hyphen.tex`.

**Frank Liang** — for the actual hyphenation dictionaries derived
from `TeX`.

**Aho, Weinberger, Kernighan** — for `awk`, and for the pattern of
"small language for one task" that Heirloom encodes across `bc`,
`dc`, `sed`, `awk`, `eqn`, `pic`, `tbl`, `grap`, `refer`.

## To the modern toolchain

The port itself sits on top of a modern stack, whose maintainers
deserve equal thanks:

- **The LLVM / Clang project** — for the compiler that caught the
  LP64 and C23 bugs upstream had accumulated over decades.
- **Homebrew and its maintainers** — for the entire package
  ecosystem that made brewing 17 QA tools painless.
- **The OpenSSL project** — for OpenSSL 3.x and its principled
  approach to opaque types, and specifically for their integration
  of NIST FIPS 203 / 204 / 205 post-quantum algorithms as
  first-class citizens.
- **NIST** and the wider cryptographic-standards community — for
  standardising ML-KEM (Kyber), ML-DSA (Dilithium), SLH-DSA
  (SPHINCS+) so downstream ports can adopt them without inventing
  parameters.
- **Apple's Darwin team** — for keeping the BSD userland alive
  enough that a Heirloom port has a target to hit at all.
- **The `pre-commit` project** — for the framework that lets a
  no-CI-budget project still have real quality gates.

## To the AI

This port was developed with substantial AI assistance (see
`AI-DISCLOSURE.md`). The AI model — Anthropic's Claude — was itself
trained on works by many of the authors named above, and on decades
of open-source code from millions of contributors whose names are
not in this file. That corpus was made possible by choices those
programmers made about how to license their work — choices that
compound over generations.

If the AI's contributions here are useful, it is because those
contributions rest on that corpus. The gratitude in this file
extends by transitivity to the countless contributors to the
broader open-source commons whose work made the AI's assistance
possible.

## To the porter's peers

To every downstream distributor (Debian, Fedora, FreeBSD, NetBSD,
OpenBSD, DragonFly, Homebrew, MacPorts, Void, NixOS, and others)
who packaged Heirloom for their users — for keeping this code
reachable when upstream went quiet.

To every reader of this file — for taking the time.

## Naming absences

Any name that belongs on this list and is missing is missing because
of the porter's ignorance, not indifference. If you know of a name
that should be here, please open an issue and it will be added.

## In closing

_"For something distributed as widely as Unix code, any license that
requires more than naming the author would only cause annoyance."_
— Gunnar Ritter, `LICENSE/README`, 22 September 2003.

We name them here, gratefully.
