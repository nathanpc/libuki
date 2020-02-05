/**
 * fileutils.c
 * A file utility helper.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#include "fileutils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

/**
 * Gets the size of a buffer to hold the whole contents of a file.
 *
 * @param  fname File path.
 * @return       Size of the content of specified file. -1 if an error occured.
 */
long file_contents_size(const char *fname) {
	FILE *fh;
	long numbytes;

	// Open file.
	fh = fopen(fname, "r");
	if (fh == NULL) {
		return -1L;
	}

	// Seek file to determine its size.
	fseek(fh, 0L, SEEK_END);
	numbytes = ftell(fh);

	// Close the file handler and return.
	fclose(fh);
	return numbytes;
}

/**
 * Reads a whole file and stores it into a string.
 *
 * @param  contents String where the file contents are to be stored (will be
 *                  allocated by this function).
 * @param  fname    File path.
 * @return          Size of the contents string. Sets contents to NULL in case
 *                  of error.
 */
size_t slurp_file(char **contents, const char *fname) {
	FILE *fh;
	long fsize;
	size_t nread;

	// Get file size and allocate memory for it.
	fsize = file_contents_size(fname);
	if (fsize >= 0L) {
		*contents = (char*)malloc((fsize + 1) * sizeof(char));
	} else {
		*contents = NULL;
		return 0;
	}

	// Handle empty files.
	if (fsize == 0L) {
		*contents[0] = '\0';
		return 0;
	}

	// Open file to read its contents.
	fh = fopen(fname, "r");
	if (fh == NULL) {
		*contents = NULL;
		return 0;
	}

	// Reads the whole file into the buffer and close it.
	nread = fread(*contents, sizeof(char), fsize, fh);
	fclose(fh);

	// Make sure our string is properly terminated and return.
	(*contents)[nread - 1] = '\0';
	return nread;
}

/**
 * Checks if a file exists.
 *
 * @param  fpath File path to be checked.
 * @return       TRUE if the file exists.
 */
bool file_exists(const char *fpath) {
	return access(fpath, F_OK) != -1;
}
