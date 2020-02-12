/**
 * windowshelper.c
 * Helps port this thing to Windows.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#include "windowshelper.h"
#ifdef WINDOWS
#include <string.h>

/**
 * Prints to the debug console. Just like printf.
 *
 * @param format String format as if it was printf.
 * @param ...    All your variables to be inserted into the format string.
 */
void PrintDebug(const char* format, ...) {
	va_list argptr;
	WCHAR szMsg[4084];
	char buffer[4084];

	// Build the string with sprintf.
	va_start(argptr, format);
	vsprintf(buffer, format, argptr);
	va_end(argptr);

	// Convert string to Unicode.
	if (!StringAtoW(szMsg, buffer)) {
		OutputDebugString(L"Error while converting string to unicode.\r\n");
		return;
	}

	// Print to debug console.
	OutputDebugString(szMsg);
	free(szMsg);
}

/**
 * Converts a regular ASCII string into a Unicode string.
 *
 * @param  szUnicode Pre-allocated Unicode string.
 * @param  szASCII   Original ASCII string.
 * @return           TRUE if the conversion was successful.
 */
BOOL StringAtoW(LPTSTR szUnicode, const char *szASCII) {
	size_t nLen = strlen(szASCII) + 1;

    if(MultiByteToWideChar(CP_ACP, 0, szASCII, -1, szUnicode, nLen) == 0) {
		return FALSE;
    }
    
    return TRUE;
}

/**
 * Converts a Unicode string into a regular ASCII string.
 *
 * @param  szASCII   Pre-allocated ASCII string.
 * @param  szUnicode Original Unicode string.
 * @return           TRUE if the conversion was successful.
 */
BOOL StringWtoA(char *szASCII, LPCTSTR szUnicode) {
	size_t nLen = wcslen(szUnicode) + 1;

    if(WideCharToMultiByte(CP_ACP, 0, szUnicode, -1, szASCII, nLen,
		NULL, NULL) == 0) {
		DWORD err = GetLastError();
		return FALSE;
    }

    return TRUE;
}

/**
 * Reads a single line from a file.
 *
 * @param  lineptr Pointer to the line string. Allocated by this function.
 * @param  n       String length.
 * @param  stream  File handler.
 * @return         Number of charaters read.
 */
size_t getline(char **lineptr, size_t *n, FILE *stream) {
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    int c;

    if (lineptr == NULL) {
        return -1;
    }
    if (stream == NULL) {
        return -1;
    }
    if (n == NULL) {
        return -1;
    }
    bufptr = *lineptr;
    size = *n;

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }
    if (bufptr == NULL) {
        bufptr = malloc(128);
        if (bufptr == NULL) {
            return -1;
        }
        size = 128;
    }
    p = bufptr;
    while(c != EOF) {
        if ((p - bufptr) > (size - 1)) {
            size = size + 128;
            bufptr = realloc(bufptr, size);
            if (bufptr == NULL) {
                return -1;
            }
        }
        *p++ = c;
        if (c == '\n') {
            break;
        }
        c = fgetc(stream);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}

#endif
