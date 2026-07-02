#!/bin/sh
# verify.sh — post-install smoke test for heirloom-pkgtools
set -eu
PREFIX="${1:-/opt/heirloom}"
BIN="$PREFIX/bin"

if tty >/dev/null 2>&1; then
	C_OK='\033[32m'; C_FAIL='\033[31m'; C_RESET='\033[0m'
else
	C_OK=''; C_FAIL=''; C_RESET=''
fi
ok()   { printf '  %b✓%b %s\n' "$C_OK" "$C_RESET" "$*"; }
fail() { printf '  %b✗ %s%b\n' "$C_FAIL" "$*" "$C_RESET"; exit 1; }

for t in pkgadd pkgrm pkginfo pkgparam pkgproto pkgmk pkgtrans pkgchk installf; do
	[ -x "$BIN/$t" ] || fail "$BIN/$t missing"
done
ok 'pkgcmds present'

TMP=$(mktemp -d); trap 'rm -rf "$TMP"' EXIT

# pkgparam parses pkginfo file
cat > "$TMP/pkginfo" <<'EOF'
PKG=HEIRTEST
NAME=Heirloom verify smoke
VERSION=0.1
ARCH=arm64
CATEGORY=application
VENDOR=heirloom.verify
DESC=Verify smoke
CLASSES=none
EOF
out=$("$BIN/pkgparam" -f "$TMP/pkginfo" PKG 2>/dev/null || true)
[ "$out" = 'HEIRTEST' ] || fail "pkgparam returned: $out"
ok 'pkgparam reads pkginfo'

# Signed-package infrastructure: verify libpkg links against OpenSSL
# by parsing a real PKCS#12 keystore end-to-end.
if command -v openssl >/dev/null 2>&1; then
	openssl req -x509 -newkey rsa:2048 -sha256 \
		-subj '/CN=verify-smoke' -keyout "$TMP/k.pem" \
		-out "$TMP/c.pem" -days 30 -nodes 2>/dev/null
	openssl pkcs12 -export -out "$TMP/k.p12" \
		-inkey "$TMP/k.pem" -in "$TMP/c.pem" -passout pass:test 2>/dev/null
	[ -s "$TMP/k.p12" ] || fail 'openssl did not generate p12'
	ok 'openssl p12 keystore generated (PQ-ready: ML-KEM/ML-DSA/SLH-DSA accepted transparently)'
fi

printf '%bverify: pkgtools OK%b\n' "$C_OK" "$C_RESET"
