/*
 * heirloom_flags.h - shared -h/--help/--usage/-v/--version flag handling
 * for the Heirloom Darwin port.
 *
 * This is port scaffolding, NOT upstream Ritter code.
 * SPDX-License-Identifier: Zlib
 * Portions Copyright (c) 2026 moonman81 <i.am.moonman@gmail.com>
 */
#ifndef HEIRLOOM_FLAGS_H
#define HEIRLOOM_FLAGS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HF_VERBOSE_TAKEN  0x1
#define HF_H_TAKEN        0x2

#ifndef HEIRLOOM_PORT_VERSION
# define HEIRLOOM_PORT_VERSION "1.0.0-darwin-arm64"
#endif
#ifndef HEIRLOOM_PORT_DATE
# define HEIRLOOM_PORT_DATE "2026-07-03"
#endif
#ifndef HEIRLOOM_MANPATH
# define HEIRLOOM_MANPATH "/opt/heirloom/share/man/5man:/opt/heirloom/share/man"
#endif

static void heirloom_flags_help(const char *tool) {
	setenv("MANPATH", HEIRLOOM_MANPATH ":/usr/share/man", 1);
	execlp("man", "man", tool, (char *)NULL);
	fprintf(stdout,
	    "%s (Heirloom Darwin port %s, %s)\n"
	    "\n"
	    "For full documentation run:  man %s\n"
	    "  or set MANPATH=%s\n\n"
	    "This is a downstream Darwin port of the Heirloom Project.\n"
	    "Upstream: http://heirloom.sourceforge.net (NOT authoritative here)\n",
	    tool, HEIRLOOM_PORT_VERSION, HEIRLOOM_PORT_DATE, tool, HEIRLOOM_MANPATH);
	exit(0);
}

static void heirloom_flags_version(const char *tool) {
	fprintf(stdout,
	    "%s (Heirloom Darwin port) %s\n"
	    "  build date:      %s\n"
	    "  upstream:        Heirloom Project <http://heirloom.sourceforge.net>\n"
	    "  port maintainer: moonman81 <i.am.moonman@gmail.com>\n"
	    "  licence:         per-file patchwork (CDDL/Caldera/Lucent/GPL/LGPL/zlib)\n"
	    "  warranty:        none, no fitness guarantee, port-status only\n",
	    tool, HEIRLOOM_PORT_VERSION, HEIRLOOM_PORT_DATE);
	exit(0);
}

static void heirloom_flags(int argc, char **argv, const char *tool, int mask) {
	int i;
	if (argc < 2) return;
	for (i = 1; i < argc; i++) {
		if (argv[i] == NULL) break;
		if (argv[i][0] != '-') return;
		if (strcmp(argv[i], "--") == 0) return;
		if (!(mask & HF_H_TAKEN) && strcmp(argv[i], "-h") == 0)
			heirloom_flags_help(tool);
		if (strcmp(argv[i], "-H") == 0 ||
		    strcmp(argv[i], "--help") == 0 ||
		    strcmp(argv[i], "--usage") == 0)
			heirloom_flags_help(tool);
		if (!(mask & HF_VERBOSE_TAKEN) && strcmp(argv[i], "-v") == 0)
			heirloom_flags_version(tool);
		if (strcmp(argv[i], "-V") == 0 ||
		    strcmp(argv[i], "--version") == 0)
			heirloom_flags_version(tool);
	}
}

#endif
