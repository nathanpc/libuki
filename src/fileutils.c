/**
 * fileutils.c
 * A file utility helper.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#include "fileutils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifndef WINDOWS
#include <unistd.h>
#include <stdint.h>
#include <dirent.h>
#endif

/**
 * Concatenates paths together safely. It is assumed that only the last string
 * is a file. Which means a directory separator will be added between all
 * strings except the last one.
 *
 * @param  npaths     Number of paths to be concatenated.
 * @param  final_path Pre-allocated pointer to a string buffer.
 * @param  ...        Paths to be concatenated.
 * @return            Size of the final buffer.
 */
size_t pathcat(int npaths, char *final_path, ...) {
	va_list ap;
	int i;

	// Initialize final path.
	final_path[0] = '\0';

	// Loop through paths.
	va_start(ap, final_path);
	for (i = 0; i < npaths; i++) {
		// TODO: Improve the performance of strcat.
		strncat(final_path, va_arg(ap, char*), UKI_MAX_PATH - 1);

		if (i < (npaths - 1)) {
			strncat(final_path, "/", UKI_MAX_PATH - 1);
		}
	}

	// TODO: Clean up path to remove duplicate separators.

	return strlen(final_path);
}

/**
 * Concatenates an extension to a file path.
 *
 * @param  final_path Path to the file without extension.
 * @param  ext        File extension without the dot.
 * @return            Size of the final buffer.
 */
size_t extcat(char *final_path, const char *ext) {
	strncat(final_path, ".", UKI_MAX_PATH - 1);
	strncat(final_path, ext, UKI_MAX_PATH - 1);

	return strlen(final_path);
}

/**
 * Lists the directory contents and stores it in a directory listing structure.
 * @remark  If list is NULL this function will return the size of the list to be
 *          allocated.
 * @warning Always set the dirlist_t.size to 0 before calling this function.
 *
 * @param  list       Directory list container. Allocated by this function.
 * @param  path       Path to the directory you want to list.
 * @param  recursive  Should this listing be done recursively?
 * @return            UKI_OK if everything went OK or list size if list is NULL.
 */
ssize_t list_directory_files(dirlist_t *list, const char *path,
							 const bool recursive) {
	DIR *dh;
	struct dirent *dir;
	char subpath[UKI_MAX_PATH];
	ssize_t count = 0;
	ssize_t err;

	// Allocate space for our list if needed.
	if (list != NULL) {
		if (list->size == 0) {
			err = list_directory_files(NULL, path, recursive);
			if (err < 0)
				return err;

			// Allocate listing array.
			list->size = err;
			list->list = (char**)malloc(err * sizeof(char*));
		}
	}

	// Open directory for listing.
	dh = opendir(path);
	if (dh == NULL) {
		return UKI_ERROR_DIRLIST_NOTFOUND;
	}

	// Read directory contents recursively.
	while (dir = readdir(dh)) {
		// Ignore anything that starts with a dot.
		if (dir->d_name[0] == '.') {
			continue;
		}

		// Decide what to do.
		switch (dir->d_type) {
		case DT_DIR:
			// Is a directory, so only do something if we are recursive.
			if (recursive) {
				// Build path.
				pathcat(2, subpath, path, dir->d_name);

				// Get listing recursively.
				err = list_directory_files(list, subpath, recursive);
				if (err < 0)
					return err;

				// Add to the count.
				if (list == NULL)
					count += err;
				else
					count += err + 1;
			}
			break;
		case DT_REG:
			if (list != NULL) {
				// Build path to file.
				pathcat(2, subpath, path, dir->d_name);

				// Allocate string.
				list->list[count] = (char*)malloc((strlen(subpath) + 1) *
												  sizeof(char));
				strcpy(list->list[count], subpath);
			}

			count++;
			break;
		case DT_UNKNOWN:
			return UKI_ERROR_DIRLIST_FILEUNKNOWN;
		}
	}

	// Clean up.
	closedir(dh);

	// Return count if the list it's what we want.
	if (list == NULL) {
		return count;
	}

	return UKI_OK;
}

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
#ifdef WINDOWS
	DWORD dwAttrib;
#ifdef WINCE
	LPTSTR szPath;

	// Convert path string to Unicode.
	if (!StringAtoW(&szPath, fpath)) {
		printf("ERROR: String conversion from ASCII to Unicode failed: '%s'\r\n",
			fpath);
		MessageBox(NULL, L"File path string conversion from ASCII to Unicode failed",
			L"Path Conversion Failed", MB_ICONEXCLAMATION | MB_OK);

		return false;
	}

	// Get file attributes and free the path string.
	dwAttrib = GetFileAttributes(szPath);
	free(szPath);
#else
	dwAttrib = GetFileAttributes(fpath);
#endif

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
	return access(fpath, F_OK) != -1;
#endif
}

/**
 * Frees a directory listing structure.
 *
 * @param list Directory listing structure to be freed.
 */
void free_dirlist(dirlist_t list) {
	size_t i;
	for (i = 0; i < list.size; i++) {
		free(list.list[i]);
	}

	free(list.list);
}
