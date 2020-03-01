/**
 * strutils.h
 * A collection of useful string manipulation utilities.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _STRUTILS_H_
#define _STRUTILS_H_

#include <stdlib.h>

// Subsitution position structure.
typedef struct {
	size_t begin;
	size_t end;
} spos_t;

// Find and Replace.
void strnreplace(char **haystack, const spos_t pos, const char *substr);
void strsreplace(char **haystack, const char *needle, const char *substr);
spos_t strsubstpos(const char *haystack, const char *needle);

#endif /* _STRUTILS_H_ */
