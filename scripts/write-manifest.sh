#!/bin/sh
#
# write-manifest.sh — capture what was installed under $PREFIX to a
# manifest file at the repo root. Consumed by uninstall.sh.
#
# Usage: write-manifest.sh <prefix> <pkg-name>

set -eu

PREFIX="${1:-/opt/heirloom}"
PKG="${2:-heirloom}"
MANIFEST=".install-manifest-$PKG.txt"

MARKER="/tmp/heirloom-install-marker.$$"
if [ -f "$MARKER" ]; then
	# Marker was created by pre-install hook; enumerate files newer
	find "$PREFIX" -type f -newer "$MARKER" 2>/dev/null | sort > "$MANIFEST"
	find "$PREFIX" -type l -newer "$MARKER" 2>/dev/null | sort >> "$MANIFEST"
	rm -f "$MARKER"
else
	# Fallback: enumerate everything under prefix (over-broad; upstream
	# make install did not leave a timestamp trail we can trust).
	find "$PREFIX" -type f 2>/dev/null | sort > "$MANIFEST"
	find "$PREFIX" -type l 2>/dev/null | sort >> "$MANIFEST"
fi

count=$(wc -l < "$MANIFEST" | tr -d ' ')
printf 'manifest: %s files/links → %s\n' "$count" "$MANIFEST" >&2
