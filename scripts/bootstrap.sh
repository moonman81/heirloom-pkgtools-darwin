#!/bin/sh
#
# bootstrap.sh — install prerequisites for building Heirloom on Darwin.
#
# Idempotent: safe to re-run. Reads prerequisites from three lists:
#   scripts/prereqs-brew.txt        Homebrew formulae
#   scripts/prereqs-brew-optional.txt   Optional formulae (best-effort)
#   scripts/prereqs-heirloom.txt    Companion Heirloom repos (git URLs)
#
# All lists are per-package overrides. Missing lists are treated as
# empty — safe defaults for lightweight packages.

set -eu

SCRIPT_DIR=$(dirname "$0")
REPO_ROOT=$(cd "$SCRIPT_DIR/.." && pwd)

# Colours
if tty >/dev/null 2>&1; then
	C_BOLD='\033[1m'; C_OK='\033[32m'; C_WARN='\033[33m'
	C_FAIL='\033[31m'; C_RESET='\033[0m'
else
	C_BOLD=''; C_OK=''; C_WARN=''; C_FAIL=''; C_RESET=''
fi

log()  { printf '%b\n' "$*" >&2; }
warn() { printf '%b%b%b\n' "$C_WARN" "$*" "$C_RESET" >&2; }
fail() { printf '%b%b%b\n' "$C_FAIL" "$*" "$C_RESET" >&2; exit 1; }

# ---- 1. Xcode CLT + brew ----

log "$C_BOLD== Xcode Command Line Tools ==$C_RESET"
if ! xcode-select -p >/dev/null 2>&1; then
	warn "Xcode CLT missing — will trigger installer"
	log 'Running: xcode-select --install (interactive)'
	xcode-select --install 2>&1 || true
	log 'After the CLT install finishes, re-run: make bootstrap'
	exit 1
fi
log "  ${C_OK}✓${C_RESET} $(xcode-select -p)"

log "$C_BOLD== Homebrew ==$C_RESET"
if ! command -v brew >/dev/null 2>&1; then
	warn 'Homebrew not installed'
	log 'Install manually per https://brew.sh, then re-run make bootstrap'
	exit 1
fi
log "  ${C_OK}✓${C_RESET} $(brew --version | head -1)"

# ---- 2. Required brew formulae ----

if [ -f "$REPO_ROOT/scripts/prereqs-brew.txt" ]; then
	log "$C_BOLD== Required Homebrew formulae ==$C_RESET"
	while IFS= read -r formula || [ -n "$formula" ]; do
		# Skip comments and blank lines
		case "$formula" in
			'#'*|'') continue ;;
		esac
		if brew list --formula "$formula" >/dev/null 2>&1; then
			log "  ${C_OK}✓${C_RESET} $formula (installed)"
		else
			log "  installing $formula ..."
			brew install "$formula" 2>&1 | tail -3
		fi
	done < "$REPO_ROOT/scripts/prereqs-brew.txt"
fi

# ---- 3. Optional brew formulae ----

if [ -f "$REPO_ROOT/scripts/prereqs-brew-optional.txt" ]; then
	log "$C_BOLD== Optional Homebrew formulae (best-effort) ==$C_RESET"
	while IFS= read -r formula || [ -n "$formula" ]; do
		case "$formula" in
			'#'*|'') continue ;;
		esac
		if brew list --formula "$formula" >/dev/null 2>&1; then
			log "  ${C_OK}✓${C_RESET} $formula (installed)"
		else
			log "  installing $formula (optional; failures OK) ..."
			brew install "$formula" 2>&1 | tail -2 || warn "  ! $formula skipped"
		fi
	done < "$REPO_ROOT/scripts/prereqs-brew-optional.txt"
fi

# ---- 4. pre-commit ----

log "$C_BOLD== pre-commit hook manager ==$C_RESET"
if command -v pre-commit >/dev/null 2>&1; then
	log "  ${C_OK}✓${C_RESET} $(pre-commit --version)"
else
	brew install pre-commit 2>&1 | tail -3
fi
# Install hooks into the local .git/hooks
if [ -d "$REPO_ROOT/.git" ]; then
	(cd "$REPO_ROOT" && pre-commit install --install-hooks 2>&1 | tail -2 || true)
	(cd "$REPO_ROOT" && pre-commit install --install-hooks --hook-type pre-push 2>&1 | tail -2 || true)
fi

# ---- 5. Companion Heirloom repos ----

if [ -f "$REPO_ROOT/scripts/prereqs-heirloom.txt" ]; then
	log "$C_BOLD== Companion Heirloom repos ==$C_RESET"
	log '  (these are peer packages that must be installed before this one)'
	while IFS= read -r line || [ -n "$line" ]; do
		case "$line" in
			'#'*|'') continue ;;
		esac
		# Format: pkg-name:binary-check-path
		pkg="${line%%:*}"
		checkpath="${line#*:}"
		if [ -x "$checkpath" ]; then
			log "  ${C_OK}✓${C_RESET} $pkg present ($checkpath)"
		else
			warn "  ✗ $pkg not installed at $checkpath"
			log '    Install by cloning + building the companion repo:'
			log "    git clone https://github.com/moonman81/heirloom-$pkg-darwin"
			log "    cd heirloom-$pkg-darwin && make lifecycle"
		fi
	done < "$REPO_ROOT/scripts/prereqs-heirloom.txt"
fi

# ---- 6. /opt/heirloom writability ----

log "$C_BOLD== /opt/heirloom writability ==$C_RESET"
if [ -d /opt/heirloom ] && [ -w /opt/heirloom ]; then
	log "  ${C_OK}✓${C_RESET} /opt/heirloom exists + writable"
elif [ ! -d /opt/heirloom ]; then
	warn '  /opt/heirloom does not exist'
	log '  Create with (requires sudo — one-off; subsequent installs never need sudo):'
	log '    sudo install -d -o "$(id -u)" -g "$(id -g)" -m 755 \'
	log '        /opt/heirloom /opt/heirloom/bin /opt/heirloom/bin/s42 \'
	log '        /opt/heirloom/bin/posix /opt/heirloom/bin/posix2001 \'
	log '        /opt/heirloom/ucb /opt/heirloom/ccs/bin \'
	log '        /opt/heirloom/lib /opt/heirloom/share/man/5man \'
	log '        /opt/heirloom/etc/default /opt/heirloom/var/adm /opt/heirloom/var/log'
	log '    sudo chown -R "$(id -u):$(id -g)" /opt/heirloom'
else
	warn "  /opt/heirloom exists but not writable by $(id -un)"
	log "  Fix with: sudo chown -R $(id -u):$(id -g) /opt/heirloom"
fi

log ''
log "$C_BOLD${C_OK}bootstrap complete$C_RESET"
