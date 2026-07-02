/*
 * Placeholder translation unit for libpkgdb.a on Darwin.
 *
 * Heirloom pkgtools deliberately disables the bundled SQLite-based
 * libpkgdb (see the top-level pkgtools README: "features recently
 * introduced by Sun are partially disabled in this variant; this
 * applies to SQLite ..."). The Makefile.mk therefore sets OBJ = to
 * an empty list.
 *
 * Darwin's ar(1) refuses to create an archive with zero members
 * ("ar: no archive members specified"). GNU ar accepts it; BSD ar
 * does not. To keep the build coherent without re-enabling the
 * disabled SQLite path, provide this single no-op translation unit.
 * libpkgdb.a becomes a valid but semantically-empty archive.
 *
 * -- Heirloom Darwin port.
 */

/*
 * A single static (unused) symbol is enough for ar/ranlib to emit
 * a valid archive TOC. The __attribute__((used)) hint prevents the
 * compiler from stripping it before ar sees it.
 */
static const char heirloom_libpkgdb_darwin_placeholder[]
	__attribute__((used)) = "heirloom-pkgtools libpkgdb disabled on Darwin";
