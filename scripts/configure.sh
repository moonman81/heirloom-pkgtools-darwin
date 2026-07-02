#!/bin/sh
#
# configure.sh — validate environment before build. Does not modify
# mk.config (the port's mk.config is committed with Darwin defaults).

set -eu

if tty >/dev/null 2>&1; then
	C_OK='\033[32m'; C_WARN='\033[33m'; C_FAIL='\033[31m'; C_RESET='\033[0m'
else
	C_OK=''; C_WARN=''; C_FAIL=''; C_RESET=''
fi

log()  { printf '%b\n' "$*" >&2; }
warn() { printf '%b%b%b\n' "$C_WARN" "$*" "$C_RESET" >&2; }
fail() { printf '%b%b%b\n' "$C_FAIL" "$*" "$C_RESET" >&2; exit 1; }

fails=0

# ---- host + toolchain ----

log "== Host =="
log "  $(sw_vers -productName) $(sw_vers -productVersion) $(uname -m)"
log ''

log "== C toolchain =="
if command -v cc >/dev/null 2>&1; then
	log "  ${C_OK}✓${C_RESET} cc: $(cc --version 2>&1 | head -1)"
else
	warn '  ✗ cc not found'
	fails=$((fails+1))
fi
if command -v make >/dev/null 2>&1; then
	log "  ${C_OK}✓${C_RESET} make: $(make --version 2>&1 | head -1 || echo 'BSD make')"
else
	warn '  ✗ make not found'
	fails=$((fails+1))
fi
log ''

# ---- yacc / lex (needed by some Heirloom packages) ----

log "== yacc / lex =="
if command -v yacc >/dev/null 2>&1; then
	log "  ${C_OK}✓${C_RESET} yacc: $(command -v yacc)"
fi
if command -v lex >/dev/null 2>&1; then
	log "  ${C_OK}✓${C_RESET} lex: $(command -v lex)"
fi
log ''

# ---- /opt/heirloom writability ----

log "== /opt/heirloom =="
if [ -w /opt/heirloom ]; then
	log "  ${C_OK}✓${C_RESET} writable by $(id -un)"
elif [ -d /opt/heirloom ]; then
	warn "  ✗ /opt/heirloom exists but not writable by $(id -un)"
	log "  fix: sudo chown -R $(id -u):$(id -g) /opt/heirloom"
	fails=$((fails+1))
else
	warn '  ✗ /opt/heirloom does not exist'
	log '  fix: sh scripts/bootstrap.sh  (see instructions there)'
	fails=$((fails+1))
fi
log ''

# ---- mk.config sanity ----

if [ -f mk.config ]; then
	log "== mk.config =="
	prefix=$(grep -E '^(PREFIX|DEFBIN)[[:space:]]*=' mk.config | head -1)
	log "  ${prefix:-not-set}"
fi
if [ -f build/mk.config ]; then
	log "== build/mk.config =="
	prefix=$(grep -E '^(PREFIX|DEFBIN)[[:space:]]*=' build/mk.config | head -1)
	log "  ${prefix:-not-set}"
fi

# ---- companion Heirloom check ----

if [ -f scripts/prereqs-heirloom.txt ]; then
	log ''
	log "== Companion Heirloom packages =="
	while IFS= read -r line || [ -n "$line" ]; do
		case "$line" in
			'#'*|'') continue ;;
		esac
		pkg="${line%%:*}"
		checkpath="${line#*:}"
		if [ -x "$checkpath" ]; then
			log "  ${C_OK}✓${C_RESET} $pkg ($checkpath)"
		else
			warn "  ✗ $pkg not at $checkpath — install the companion repo first"
			fails=$((fails+1))
		fi
	done < scripts/prereqs-heirloom.txt
fi

log ''
if [ "$fails" -gt 0 ]; then
	fail "$fails prerequisite failure(s) — see above; re-run 'make bootstrap' then 'make configure'"
fi
log "${C_OK}configure OK — ready to build${C_RESET}"
