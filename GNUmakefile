# GNUmakefile — Application Lifecycle Management (ALM) wrapper
#
# GNU make prefers GNUmakefile over makefile, so this file takes
# precedence and wraps upstream Heirloom's `makefile` with lifecycle
# targets: bootstrap, configure, build, install, verify, uninstall,
# status, snapshot, lifecycle.
#
# Upstream Heirloom's `makefile` is unchanged and still handles the
# actual compile. Type `make help` to list all targets.

SHELL     = /bin/sh
PREFIX   ?= /opt/heirloom
PKG      ?= pkgtools
TITLE    ?= Packaging Tools
UPSTREAM ?= heirloom-pkgtools-070227

# ROOT is the DESTDIR-style staging root. Default empty = install
# directly into PREFIX. Set ROOT=/some/path to stage.
ROOT     ?=

# Path to the upstream makefile. `makefile` is lowercase in every
# Heirloom package.
UPSTREAM_MK = makefile

# Colour output if attached to a tty.
ifeq ($(shell tty >/dev/null 2>&1 && echo yes),yes)
  C_BOLD  := \033[1m
  C_OK    := \033[32m
  C_WARN  := \033[33m
  C_FAIL  := \033[31m
  C_RESET := \033[0m
else
  C_BOLD  :=
  C_OK    :=
  C_WARN  :=
  C_FAIL  :=
  C_RESET :=
endif

# ---------------- default + help ----------------

.DEFAULT_GOAL := help

.PHONY: help
help:
	@printf '$(C_BOLD)Heirloom $(TITLE) — Darwin port ALM$(C_RESET)\n'
	@printf '\n'
	@printf 'Prefix:   %s\n' '$(PREFIX)'
	@printf 'Package:  %s (upstream: $(UPSTREAM))\n' '$(PKG)'
	@printf 'Root:     %s (empty = install directly, else DESTDIR-style)\n' '$(ROOT)'
	@printf '\n'
	@printf '$(C_BOLD)Lifecycle targets$(C_RESET)\n'
	@printf '  %-16s %s\n' 'lifecycle'  'full: bootstrap + configure + build + install + verify'
	@printf '  %-16s %s\n' 'bootstrap'  'install brew + prerequisites (idempotent)'
	@printf '  %-16s %s\n' 'configure'  'verify environment + prep $(PREFIX)'
	@printf '  %-16s %s\n' 'build'      'compile via upstream makefile (alias: all)'
	@printf '  %-16s %s\n' 'install'    'install into $(ROOT)$(PREFIX) + write manifest'
	@printf '  %-16s %s\n' 'verify'     'smoke-test installed binaries'
	@printf '  %-16s %s\n' 'uninstall'  'remove installed files per manifest'
	@printf '  %-16s %s\n' 'status'     'report installation state'
	@printf '\n'
	@printf '$(C_BOLD)QA + housekeeping$(C_RESET)\n'
	@printf '  %-16s %s\n' 'test'       'run pre-commit fast + push tiers'
	@printf '  %-16s %s\n' 'test-manual' 'run pre-commit manual tier (roundtrip + lint sweep)'
	@printf '  %-16s %s\n' 'snapshot'   'git-tag current state'
	@printf '  %-16s %s\n' 'clean'      'remove build products'
	@printf '  %-16s %s\n' 'distclean'  'clean + remove generated Makefiles'
	@printf '\n'
	@printf '$(C_BOLD)Upstream passthrough$(C_RESET) (delegates to $(UPSTREAM_MK))\n'
	@printf '  make -f $(UPSTREAM_MK) <target>\n'
	@printf '\n'
	@printf '$(C_BOLD)Environment overrides$(C_RESET)\n'
	@printf '  PREFIX=/some/where  install prefix (default $(PREFIX))\n'
	@printf '  ROOT=/some/where    DESTDIR-style staging root (default empty)\n'

# ---------------- lifecycle ----------------

.PHONY: lifecycle
lifecycle: bootstrap configure build install verify
	@printf '$(C_OK)✓ lifecycle complete$(C_RESET)\n'

# ---------------- prerequisites ----------------

.PHONY: bootstrap
bootstrap:
	@printf '$(C_BOLD)bootstrap$(C_RESET) — installing prerequisites\n'
	@sh scripts/bootstrap.sh

# ---------------- configure ----------------

.PHONY: configure
configure:
	@printf '$(C_BOLD)configure$(C_RESET) — verifying environment + prefix\n'
	@sh scripts/configure.sh

# ---------------- build ----------------

.PHONY: build all
build all:
	@printf '$(C_BOLD)build$(C_RESET) — delegating to upstream $(UPSTREAM_MK)\n'
	@$(MAKE) -f $(UPSTREAM_MK)

# ---------------- install ----------------

.PHONY: install
install:
	@sh scripts/install-man.sh '$(ROOT)$(PREFIX)' 2>/dev/null || true
	@printf '$(C_BOLD)install$(C_RESET) — installing into $(ROOT)$(PREFIX)\n'
	@$(MAKE) -f $(UPSTREAM_MK) install ROOT=$(ROOT)
	@sh scripts/write-manifest.sh '$(PREFIX)' '$(PKG)'

# ---------------- verify ----------------

.PHONY: verify
verify:
	@printf '$(C_BOLD)verify$(C_RESET) — running post-install smoke tests\n'
	@sh scripts/verify.sh '$(PREFIX)'

# ---------------- uninstall ----------------

.PHONY: uninstall
uninstall:
	@printf '$(C_BOLD)uninstall$(C_RESET) — removing installed files per manifest\n'
	@sh scripts/uninstall.sh '$(PREFIX)' '$(PKG)'

# ---------------- status ----------------

.PHONY: status
status:
	@sh scripts/status.sh '$(PREFIX)' '$(PKG)'

# ---------------- QA ----------------

.PHONY: test
test:
	@printf '$(C_BOLD)test$(C_RESET) — pre-commit (fast + push tiers)\n'
	@if command -v pre-commit >/dev/null 2>&1; then \
		pre-commit run --all-files --hook-stage pre-commit && \
		pre-commit run --all-files --hook-stage pre-push ; \
	else \
		printf '$(C_WARN)pre-commit not installed; run: make bootstrap$(C_RESET)\n' ; \
		exit 1 ; \
	fi

.PHONY: test-manual
test-manual:
	@printf '$(C_BOLD)test-manual$(C_RESET) — pre-commit manual tier (release gate)\n'
	@if command -v pre-commit >/dev/null 2>&1; then \
		pre-commit run --all-files --hook-stage manual ; \
	else \
		printf '$(C_WARN)pre-commit not installed$(C_RESET)\n' ; \
		exit 1 ; \
	fi

# ---------------- snapshot / release ----------------

.PHONY: snapshot
snapshot:
	@sh scripts/snapshot.sh

# ---------------- clean ----------------

.PHONY: clean distclean
clean:
	@$(MAKE) -f $(UPSTREAM_MK) clean 2>/dev/null || \
		$(MAKE) -f $(UPSTREAM_MK) mrproper 2>/dev/null || \
		printf 'no clean target upstream; skipping\n'

distclean: clean
	-@find . -name 'Makefile' -not -name 'Makefile.mk' \
		-not -path './.git/*' -delete 2>/dev/null || true
	-@rm -rf .install-manifest.txt
