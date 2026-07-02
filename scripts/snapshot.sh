#!/bin/sh
#
# snapshot.sh — tag the current git state as a release snapshot.
# Uses semver + timestamp for uniqueness.

set -eu

if [ ! -d .git ]; then
	printf 'not a git repo\n' >&2
	exit 1
fi

if [ -n "$(git status --porcelain)" ]; then
	printf 'REFUSE: working tree is dirty — commit or stash first\n' >&2
	git status --short >&2
	exit 1
fi

# Compose a snapshot tag: snapshot-YYYYMMDD-HHMMSS
ts=$(date -u '+%Y%m%d-%H%M%SZ')
tag="snapshot-$ts"

if git tag | grep -q "^$tag\$"; then
	printf 'tag %s already exists\n' "$tag" >&2
	exit 1
fi

head=$(git rev-parse --short refs/heads/main)
git tag -a "$tag" -m "Snapshot at $head — automated via make snapshot"

printf 'snapshot: %s @ %s\n' "$tag" "$head" >&2
printf 'to push: git push origin %s\n' "$tag" >&2
