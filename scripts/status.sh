#!/bin/sh
#
# status.sh — report installation state of this package.
#
# Usage: status.sh <prefix> <pkg-name>

set -eu

PREFIX="${1:-/opt/heirloom}"
PKG="${2:-heirloom}"
MANIFEST=".install-manifest-$PKG.txt"

if tty >/dev/null 2>&1; then
	C_BOLD='\033[1m'; C_OK='\033[32m'; C_WARN='\033[33m'
	C_FAIL='\033[31m'; C_RESET='\033[0m'
else
	C_BOLD=''; C_OK=''; C_WARN=''; C_FAIL=''; C_RESET=''
fi

printf '%bHeirloom %s status%b\n' "$C_BOLD" "$PKG" "$C_RESET"
printf '  prefix:  %s\n' "$PREFIX"
printf '  cwd:     %s\n' "$(pwd)"
printf '\n'

# ---- Repo state ----

printf '%bRepo state%b\n' "$C_BOLD" "$C_RESET"
if [ -d .git ]; then
	head=$(git rev-parse --short refs/heads/main 2>/dev/null || echo unknown)
	subject=$(git log -1 --format='%s' 2>/dev/null || echo '(no commits)')
	printf '  HEAD:    %s %s\n' "$head" "$subject"
	if [ -n "$(git status --porcelain 2>/dev/null)" ]; then
		printf '  status:  %bdirty%b (uncommitted changes)\n' "$C_WARN" "$C_RESET"
	else
		printf '  status:  %bclean%b\n' "$C_OK" "$C_RESET"
	fi
	tag=$(git describe --tags --exact-match 2>/dev/null || echo '(no tag on HEAD)')
	printf '  tag:     %s\n' "$tag"
else
	printf '  (not a git repo)\n'
fi
printf '\n'

# ---- Installed state ----

printf '%bInstalled state%b\n' "$C_BOLD" "$C_RESET"
if [ -f "$MANIFEST" ]; then
	count=$(wc -l < "$MANIFEST" | tr -d ' ')
	printf '  manifest: %s (%s files listed)\n' "$MANIFEST" "$count"
	# Verify each manifest entry
	present=0; missing=0
	while IFS= read -r path || [ -n "$path" ]; do
		[ -z "$path" ] && continue
		if [ -e "$path" ]; then
			present=$((present+1))
		else
			missing=$((missing+1))
		fi
	done < "$MANIFEST"
	if [ "$missing" -eq 0 ]; then
		printf '  files:    %b%s/%s present%b\n' "$C_OK" "$present" "$count" "$C_RESET"
	else
		printf '  files:    %b%s/%s present, %s missing%b\n' \
			"$C_WARN" "$present" "$count" "$missing" "$C_RESET"
	fi
else
	printf '  manifest: %bnone%b — package not installed via this repo\n' \
		"$C_WARN" "$C_RESET"
fi
printf '\n'

# ---- Prefix inventory ----

if [ -d "$PREFIX" ]; then
	printf '%b%s inventory%b\n' "$C_BOLD" "$PREFIX" "$C_RESET"
	for d in bin bin/s42 bin/posix bin/posix2001 ucb ccs/bin lib share/man/5man; do
		if [ -d "$PREFIX/$d" ]; then
			n=$(find "$PREFIX/$d" -maxdepth 1 -mindepth 1 2>/dev/null | wc -l | tr -d ' ')
			printf '  %-24s %s entries\n' "$d/" "$n"
		fi
	done
else
	printf '%b%s%b does not exist\n' "$C_WARN" "$PREFIX" "$C_RESET"
fi
