#!/bin/sh
#
# uninstall.sh — remove files listed in the install manifest.
# Refuses if the manifest is missing; refuses paths outside PREFIX.
#
# Usage: uninstall.sh <prefix> <pkg-name>

set -eu

PREFIX="${1:-/opt/heirloom}"
PKG="${2:-heirloom}"
MANIFEST=".install-manifest-$PKG.txt"

if tty >/dev/null 2>&1; then
	C_OK='\033[32m'; C_WARN='\033[33m'; C_FAIL='\033[31m'; C_RESET='\033[0m'
else
	C_OK=''; C_WARN=''; C_FAIL=''; C_RESET=''
fi

if [ ! -f "$MANIFEST" ]; then
	printf '%b%s%b\n' "$C_FAIL" \
		"REFUSE: manifest $MANIFEST not found — cannot safely uninstall." \
		"$C_RESET" >&2
	printf 'Either run "make install" first (which writes the manifest),\n' >&2
	printf 'or hand-remove files from %s if you know what to remove.\n' "$PREFIX" >&2
	exit 1
fi

printf 'Reading manifest %s ...\n' "$MANIFEST" >&2
count=$(wc -l < "$MANIFEST" | tr -d ' ')
printf 'Manifest lists %s file(s).\n' "$count" >&2

# Sanity: refuse any path not under PREFIX
if grep -v "^$PREFIX" "$MANIFEST" | grep -qE '^/'; then
	printf '%b%s%b\n' "$C_FAIL" \
		"REFUSE: manifest contains paths outside $PREFIX" "$C_RESET" >&2
	grep -v "^$PREFIX" "$MANIFEST" | head -5 >&2
	exit 1
fi

removed=0
skipped=0
while IFS= read -r path || [ -n "$path" ]; do
	[ -z "$path" ] && continue
	if [ -f "$path" ] || [ -L "$path" ]; then
		rm -f "$path"
		removed=$((removed+1))
	else
		skipped=$((skipped+1))
	fi
done < "$MANIFEST"

# Clean out any newly-empty directories under PREFIX
find "$PREFIX" -type d -empty -delete 2>/dev/null || true

printf '%b✓ uninstall complete: %s removed, %s skipped (no longer present)%b\n' \
	"$C_OK" "$removed" "$skipped" "$C_RESET" >&2
printf 'Manifest %s kept for audit; delete manually if desired.\n' "$MANIFEST" >&2
