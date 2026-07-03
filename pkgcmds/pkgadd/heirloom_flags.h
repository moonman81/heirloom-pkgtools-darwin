/*
 * heirloom_flags.h — shared help/version/variant/dialect handling for
 * the Heirloom Darwin port.
 *
 * This is port scaffolding, NOT upstream Ritter code.
 * SPDX-License-Identifier: Zlib
 * Portions Copyright (c) 2026 moonman81 <i.am.moonman@gmail.com>
 *
 * PURPOSE
 * =======
 * Every C main() in the Heirloom port calls
 *
 *     heirloom_flags(argc, argv, "toolname", mask);
 *
 * as its first executable statement. The call
 *
 *   1. Honours HEIRLOOM_VARIANT / HEIRLOOM_DIALECT / --variant=<name>
 *      by re-execing the requested personality binary if the current
 *      process is not already the right one.
 *   2. Honours HEIRLOOM_PORT_VERSION_REQ (a required-port-version pin)
 *      by aborting if the running port version does not match.
 *   3. Handles -h / -H / --help / --usage (invokes man(1) on the tool
 *      inside the Heirloom man tree).
 *   4. Handles -v / -V / --version (prints a Heirloom Darwin port
 *      banner, plus the active variant and port revision).
 *   5. Handles --variants (lists the personality binaries available
 *      for the tool) and --describe-modality (prints the modality
 *      matrix — version × variant × dialect).
 *
 * MODALITY MODEL (version × variant × dialect)
 * ============================================
 * VERSION  : upstream Heirloom release + moonman81 port revision.
 *            Reported by --version. Pinnable via HEIRLOOM_PORT_VERSION_REQ.
 * VARIANT  : which SVR4 personality binary (SVID3 default, POSIX/SUS,
 *            POSIX-2001/SUS3, S42/SVID4-subset, UCB). Directory-based:
 *              $PREFIX/bin/<tool>              default (SVID3)
 *              $PREFIX/bin/posix/<tool>        SUS
 *              $PREFIX/bin/posix2001/<tool>    SUS3
 *              $PREFIX/bin/s42/<tool>          S42
 *              $PREFIX/ucb/<tool>              UCB
 *              $PREFIX/ccs/bin/<tool>          CCS (make, yacc, m4, sccs)
 * DIALECT  : behavioural style. Mostly a synonym for variant, but
 *            allows semantic names such as "bsd", "posix", "svr4",
 *            "sysv3". Mapped to variants via the dialect table below.
 *
 * SELECTION PRECEDENCE (highest wins):
 *   1. Command-line: --variant=<name> or --dialect=<name>
 *   2. Env: HEIRLOOM_VARIANT then HEIRLOOM_DIALECT
 *   3. Env: $SYSV3 (Ritter's classic SVID3 selector) — respected
 *   4. Compile-time default (whichever personality this binary was
 *      built as)
 *
 * The shim sets HEIRLOOM_ACTIVE_VARIANT before re-exec to prevent
 * infinite loops.
 */
#ifndef HEIRLOOM_FLAGS_H
#define HEIRLOOM_FLAGS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

/* Some Heirloom translation units redefine stdout / stderr as macros
 * (troff's tdef.h does `#define stderr xxstderr`) and some redefine
 * dprintf (grap's grap.h does `#define dprintf if(dbg)printf`). We
 * sidestep both by defining a private hf_emit(fd, fmt, ...) that goes
 * straight to a file descriptor via vsnprintf() + write(). */

static void hf_emit(int fd, const char *fmt, ...) {
	char buf[4096];
	va_list ap;
	int n;
	va_start(ap, fmt);
	n = vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	if (n > 0) {
		if ((size_t)n >= sizeof(buf)) n = (int)sizeof(buf) - 1;
		(void)write(fd, buf, (size_t)n);
	}
}

/* Bits for the mask argument. Set them when the lowercase form is
 * load-bearing in the tool's own getopt (e.g. -v = verbose in cp/tar
 * /cpio/grep, -h = hash in sh). The uppercase form (-H / -V) and the
 * long form (--help / --version) are always recognised regardless. */
#define HF_VERBOSE_TAKEN  0x1
#define HF_H_TAKEN        0x2

#ifndef HEIRLOOM_PORT_VERSION
# define HEIRLOOM_PORT_VERSION "1.1.0-darwin-arm64"
#endif
#ifndef HEIRLOOM_PORT_DATE
# define HEIRLOOM_PORT_DATE    "2026-07-03"
#endif
#ifndef HEIRLOOM_PORT_REVISION
# define HEIRLOOM_PORT_REVISION "moonman81/heirloom-*-darwin"
#endif
#ifndef HEIRLOOM_PREFIX
# define HEIRLOOM_PREFIX       "/opt/heirloom"
#endif
#ifndef HEIRLOOM_MANPATH
# define HEIRLOOM_MANPATH      "/opt/heirloom/share/man/5man:/opt/heirloom/share/man"
#endif

/* Which variant this binary was compiled as (default SVID3 unless one
 * of the -DSUS / -DSU3 / -DS42 / -DUCB / -DCCS flags is set at
 * compile-time). */
#if defined(SU3)
# define HEIRLOOM_BUILT_VARIANT "posix2001"
#elif defined(SUS)
# define HEIRLOOM_BUILT_VARIANT "posix"
#elif defined(S42)
# define HEIRLOOM_BUILT_VARIANT "s42"
#elif defined(UCB)
# define HEIRLOOM_BUILT_VARIANT "ucb"
#elif defined(HEIRLOOM_CCS_BUILD)
# define HEIRLOOM_BUILT_VARIANT "ccs"
#else
# define HEIRLOOM_BUILT_VARIANT "default"
#endif

/* Dialect → variant mapping. Extend by adding rows. */
struct hf_dialect_row { const char *dialect; const char *variant; };
static const struct hf_dialect_row hf_dialects[] = {
	{ "svid3",     "default"   },
	{ "svr3",      "default"   },  /* Ritter's SYSV3 style */
	{ "svr4",      "default"   },
	{ "sysv",      "default"   },
	{ "sysv3",     "default"   },
	{ "posix",     "posix"     },
	{ "sus",       "posix"     },
	{ "sus2",      "posix"     },
	{ "posix2001", "posix2001" },
	{ "sus3",      "posix2001" },
	{ "s42",       "s42"       },
	{ "svid4",     "s42"       },
	{ "ucb",       "ucb"       },
	{ "bsd",       "ucb"       },  /* colloquial */
	{ "ccs",       "ccs"       },
	{ NULL,        NULL        },
};

static const char *hf_dialect_to_variant(const char *dialect) {
	int i;
	if (dialect == NULL) return NULL;
	for (i = 0; hf_dialects[i].dialect != NULL; i++) {
		if (strcmp(hf_dialects[i].dialect, dialect) == 0)
			return hf_dialects[i].variant;
	}
	return NULL;
}

static const char *hf_variant_dir(const char *variant) {
	if (variant == NULL || strcmp(variant, "default") == 0) return "bin";
	if (strcmp(variant, "posix") == 0)     return "bin/posix";
	if (strcmp(variant, "posix2001") == 0) return "bin/posix2001";
	if (strcmp(variant, "s42") == 0)       return "bin/s42";
	if (strcmp(variant, "ucb") == 0)       return "ucb";
	if (strcmp(variant, "ccs") == 0)       return "ccs/bin";
	return NULL;
}

static void hf_help(const char *tool) {
	setenv("MANPATH", HEIRLOOM_MANPATH ":/usr/share/man", 1);
	execlp("man", "man", tool, (char *)NULL);
	hf_emit(1,
	    "%s (Heirloom Darwin port %s, %s)\n"
	    "\n"
	    "For full documentation:  man %s\n"
	    "  Heirloom man tree:     %s\n"
	    "  HOWTO:                 %s/share/doc/heirloom-*/HOWTO.md\n"
	    "\n"
	    "This is a downstream Darwin port of the Heirloom Project.\n"
	    "Upstream: http://heirloom.sourceforge.net  (NOT authoritative here)\n",
	    tool, HEIRLOOM_PORT_VERSION, HEIRLOOM_PORT_DATE,
	    tool, HEIRLOOM_MANPATH, HEIRLOOM_PREFIX);
	exit(0);
}

static void hf_version(const char *tool) {
	const char *active = getenv("HEIRLOOM_ACTIVE_VARIANT");
	if (active == NULL || active[0] == 0) active = HEIRLOOM_BUILT_VARIANT;
	hf_emit(1,
	    "%s (Heirloom Darwin port) %s\n"
	    "  build date:      %s\n"
	    "  built variant:   %s\n"
	    "  active variant:  %s\n"
	    "  port revision:   %s\n"
	    "  upstream:        Heirloom Project <http://heirloom.sourceforge.net>\n"
	    "  port maintainer: moonman81 <i.am.moonman@gmail.com>\n"
	    "  licence:         per-file patchwork (CDDL/Caldera/Lucent/GPL/LGPL/zlib)\n"
	    "  warranty:        none, no fitness guarantee, port-status only\n",
	    tool, HEIRLOOM_PORT_VERSION,
	    HEIRLOOM_PORT_DATE,
	    HEIRLOOM_BUILT_VARIANT,
	    active,
	    HEIRLOOM_PORT_REVISION);
	exit(0);
}

static void hf_list_variants(const char *tool) {
	static const char *bins[] = {
	    "bin", "bin/posix", "bin/posix2001", "bin/s42", "ucb", "ccs/bin", NULL
	};
	static const char *labels[] = {
	    "default (SVID3)", "POSIX/SUS", "POSIX-2001/SUS3",
	    "S42/SVID4-subset", "UCB/BSD", "CCS", NULL
	};
	int i;
	hf_emit(1, "Variants of %s available under %s:\n", tool, HEIRLOOM_PREFIX);
	for (i = 0; bins[i] != NULL; i++) {
		char path[1024];
		snprintf(path, sizeof(path), "%s/%s/%s", HEIRLOOM_PREFIX, bins[i], tool);
		if (access(path, X_OK) == 0)
			hf_emit(1, "  %-16s %s   [%s]\n", labels[i], path, "OK");
		else
			hf_emit(1, "  %-16s %s   [not installed]\n", labels[i], path);
	}
	exit(0);
}

static void hf_describe_modality(const char *tool) {
	hf_emit(1,
	    "MODALITY MATRIX for %s (Heirloom Darwin port)\n"
	    "\n"
	    "VERSION:  %s  (built %s, revision %s)\n"
	    "          Pin with HEIRLOOM_PORT_VERSION_REQ=<version-string>.\n"
	    "\n"
	    "VARIANT:  currently built as %s.\n"
	    "          Override with --variant=<name>, HEIRLOOM_VARIANT=<name>,\n"
	    "          or HEIRLOOM_DIALECT=<name>.\n"
	    "          Run with --variants to see which variants are installed.\n"
	    "\n"
	    "DIALECT:  human-friendly synonym for variant.\n"
	    "          Recognised: svid3 svr3 svr4 sysv sysv3 posix sus sus2\n"
	    "                     posix2001 sus3 s42 svid4 ucb bsd ccs.\n"
	    "\n"
	    "SELECTION PRECEDENCE (highest wins):\n"
	    "  1. --variant / --dialect flag\n"
	    "  2. HEIRLOOM_VARIANT env\n"
	    "  3. HEIRLOOM_DIALECT env\n"
	    "  4. SYSV3 env (Ritter's classic SVID3 selector)\n"
	    "  5. compile-time default (%s)\n"
	    "\n"
	    "See:      %s/share/doc/heirloom-*/HOWTO.md\n"
	    "          man heirloom-modality\n",
	    tool,
	    HEIRLOOM_PORT_VERSION, HEIRLOOM_PORT_DATE, HEIRLOOM_PORT_REVISION,
	    HEIRLOOM_BUILT_VARIANT,
	    HEIRLOOM_BUILT_VARIANT,
	    HEIRLOOM_PREFIX);
	exit(0);
}

/* Re-exec into the personality dir for the requested variant. Marks
 * HEIRLOOM_ACTIVE_VARIANT to prevent infinite loops. */
static void hf_reexec_variant(const char *tool, const char *variant, char **argv) {
	const char *dir;
	char path[1024];
	if (variant == NULL || strcmp(variant, HEIRLOOM_BUILT_VARIANT) == 0)
		return;  /* already right */
	if (getenv("HEIRLOOM_ACTIVE_VARIANT") != NULL)
		return;  /* prevent loops */
	dir = hf_variant_dir(variant);
	if (dir == NULL) {
		hf_emit(2, "%s: unknown variant '%s'\n", tool, variant);
		exit(2);
	}
	snprintf(path, sizeof(path), "%s/%s/%s", HEIRLOOM_PREFIX, dir, tool);
	if (access(path, X_OK) != 0)
		return;  /* variant binary not installed; keep default */
	setenv("HEIRLOOM_ACTIVE_VARIANT", variant, 1);
	execv(path, argv);
	/* If execv returns, it failed; fall through and keep running as
	 * the built variant. */
}

static void hf_version_pin_check(const char *tool) {
	const char *pin = getenv("HEIRLOOM_PORT_VERSION_REQ");
	if (pin == NULL || pin[0] == 0) return;
	if (strcmp(pin, HEIRLOOM_PORT_VERSION) != 0) {
		hf_emit(2,
		    "%s: HEIRLOOM_PORT_VERSION_REQ=%s but running %s\n",
		    tool, pin, HEIRLOOM_PORT_VERSION);
		exit(78);  /* EX_CONFIG */
	}
}

static void heirloom_flags(int argc, char **argv, const char *tool, int mask) {
	int i;
	const char *want_variant = NULL;
	const char *env;

	/* Version pinning first — it may abort. */
	hf_version_pin_check(tool);

	/* Command-line: --variant=X / --dialect=X */
	for (i = 1; i < argc; i++) {
		if (argv[i] == NULL || argv[i][0] != '-') break;
		if (strncmp(argv[i], "--variant=", 10) == 0) {
			want_variant = argv[i] + 10;
			break;
		}
		if (strncmp(argv[i], "--dialect=", 10) == 0) {
			want_variant = hf_dialect_to_variant(argv[i] + 10);
			if (want_variant == NULL) {
				hf_emit(2, "%s: unknown dialect '%s'\n",
				    tool, argv[i] + 10);
				exit(2);
			}
			break;
		}
	}
	if (want_variant == NULL) {
		env = getenv("HEIRLOOM_VARIANT");
		if (env != NULL && env[0] != 0) want_variant = env;
	}
	if (want_variant == NULL) {
		env = getenv("HEIRLOOM_DIALECT");
		if (env != NULL && env[0] != 0)
			want_variant = hf_dialect_to_variant(env);
	}
	if (want_variant == NULL) {
		env = getenv("SYSV3");
		if (env != NULL) want_variant = "default";
	}
	if (want_variant != NULL)
		hf_reexec_variant(tool, want_variant, argv);

	/* Help / version / listing flags. */
	if (argc < 2) return;
	for (i = 1; i < argc; i++) {
		if (argv[i] == NULL) break;
		if (argv[i][0] != '-') return;
		if (strcmp(argv[i], "--") == 0) return;

		/* Long forms — always recognised. */
		if (strcmp(argv[i], "--help") == 0 ||
		    strcmp(argv[i], "--usage") == 0)   hf_help(tool);
		if (strcmp(argv[i], "--version") == 0) hf_version(tool);
		if (strcmp(argv[i], "--variants") == 0)      hf_list_variants(tool);
		if (strcmp(argv[i], "--describe-modality") == 0) hf_describe_modality(tool);

		/* Uppercase caps — always recognised. */
		if (strcmp(argv[i], "-H") == 0) hf_help(tool);
		if (strcmp(argv[i], "-V") == 0) hf_version(tool);

		/* Lowercase — skipped if the tool claims the semantics. */
		if (!(mask & HF_H_TAKEN) && strcmp(argv[i], "-h") == 0)
			hf_help(tool);
		if (!(mask & HF_VERBOSE_TAKEN) && strcmp(argv[i], "-v") == 0)
			hf_version(tool);
	}
}

#endif /* HEIRLOOM_FLAGS_H */
