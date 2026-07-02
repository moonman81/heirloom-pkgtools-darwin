#!/bin/sh
# install-man.sh — install repo-local man pages under PREFIX.
# Called from GNUmakefile after the upstream install target.
set -eu
PREFIX="${1:-/opt/heirloom}"

if [ ! -d man ]; then
	exit 0
fi

MANDIR="$PREFIX/share/man"
if [ ! -w "$(dirname "$MANDIR")" ] 2>/dev/null && [ ! -d "$MANDIR" ]; then
	printf 'install-man: cannot write %s — skipping\n' "$MANDIR" >&2
	exit 0
fi

for section_dir in man/man*/; do
	[ -d "$section_dir" ] || continue
	section=$(basename "$section_dir")
	mkdir -p "$MANDIR/$section"
	for page in "$section_dir"*.[0-9] "$section_dir"*.[0-9][a-z]; do
		[ -f "$page" ] || continue
		cp "$page" "$MANDIR/$section/"
	done
done

# HOWTO.md alongside the man pages
if [ -f HOWTO.md ]; then
	mkdir -p "$PREFIX/share/doc/heirloom-$(basename "$(pwd)")"
	cp HOWTO.md "$PREFIX/share/doc/heirloom-$(basename "$(pwd)")/HOWTO.md"
fi

printf 'install-man: installed man pages under %s\n' "$MANDIR" >&2
