/**
 * windowshelper.h
 * Helps port this thing to Windows.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _WINDOWSHELPER_H_
#define _WINDOWSHELPER_H_

// Check for UNIX.
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))

// Define us as a UNIX system.
#define UNIX

#endif /* UNIX */

// Check for Windows.
#if defined(_WIN32) || defined(WIN32) || defined(_WIN32_WCE)

// Enable Unicode.
#define UNICODE

// General headers.
#include <stdlib.h>
#include <stdio.h>

// The ubiquitous Windows header.
#include <windows.h>

// Windows constant definition.
#define WINDOWS
#if _WIN32_WCE
#define WINCE
#endif

// Standard type definitions.
typedef BYTE uint8_t;
typedef long ssize_t;
typedef BOOL bool;
#define true TRUE
#define false FALSE

// Defined INVALID_FILE_ATTRIBUTES for platforms that don't have it.
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES 0xFFFFFFFF
#endif

// String conversions.
BOOL StringAtoW(LPTSTR szUnicode, const char *szASCII);
BOOL StringWtoA(char *szASCII, LPCTSTR szUnicode);

// GNU extension compatibility.
size_t getline(char **lineptr, size_t *n, FILE *stream);

// Debugging.
void PrintDebug(const char* format, ...);

#endif /* WINDOWS */

#endif /* _WINDOWSHELPER_H_ */
