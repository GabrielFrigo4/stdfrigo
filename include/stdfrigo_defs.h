#ifndef STDFRIGO_DEFS_H
#define STDFRIGO_DEFS_H

/* ===============================================================
 * MICROSOFT WINDOWS BUILD TOOLS (MSVC)
 * =============================================================== */

#if defined(_MSC_VER)
#pragma comment(lib, "libstdfrigo.a")
#endif

/* ===============================================================
 * VERSIONAMENTO
 * =============================================================== */

#define STDFRIGO_VER_MAJOR 1
#define STDFRIGO_VER_MINOR 0
#define STDFRIGO_VER_PATCH 0

#define _STR(x) #x
#define _VAL(x) _STR(x)

#define STDFRIGO_VERSION _VAL(STDFRIGO_VER_MAJOR) "." _VAL(STDFRIGO_VER_MINOR) "." _VAL(STDFRIGO_VER_PATCH)

#undef _STR
#undef _VAL

#define STDFRIGO_VERSION_HEX ((STDFRIGO_VER_MAJOR << 16) | (STDFRIGO_VER_MINOR << 8) | (STDFRIGO_VER_PATCH))

#endif
