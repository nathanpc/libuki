/**
 * windowshelper.h
 * Helps port this thing to Windows.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _WINDOWSHELPER_H_
#define _WINDOWSHELPER_H_
#if defined(_WIN32) || defined(WIN32)

#include <stdlib.h>
#include <stdio.h>

// The ubiquitous Windows header.
#include <windows.h>

// Windows constant definition.
#define WINDOWS

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

// Portable getline.
size_t getline(char **lineptr, size_t *n, FILE *stream);

#endif
#endif /* _WINDOWSHELPER_H_ */
