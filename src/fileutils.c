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
#ifdef UNIX
#include <unistd.h>
#include <stdint.h>
#include <dirent.h>
#endif

// Private methods.
int count_dir_deepness(const char *path);
#ifdef WINDOWS
int __cdecl sort_dirs_ascending (const void *a, const void *b);
#else
int sort_dirs_ascending (const void *a, const void *b);
#endif
ssize_t n_list_directory_files(size_t init_count, dirlist_t *list,
							   const char *path, const bool recursive);

/**
 * Gets just the filename without the extension from a path.
 * @remark If fname is NULL this function returns the size of the buffer
 *         to be allocated to fit the file name including the NULL terminator.
 *
 * @param fname Pre-allocated string where the file name will be placed.
 * @param path  File path to extract the file name from.
 */
size_t basename_noext(char *fname, const char *path) {
	const char *tmp = path;
	const char *lastpos = path;
	char *buf = fname;

	// Go through the path looking for the slashes.
	for (; *tmp != '\0'; tmp++) {
#ifdef WINDOWS
		if (*tmp == '\\')
#else
		if (*tmp == '/')
#endif
			lastpos = tmp;
	}

	// Check for initial slashes.
#ifdef WINDOWS
	if (*lastpos == '\\')
#else
	if (*lastpos == '/')
#endif
		lastpos++;

	// Copy the string only if we have it.
	if (fname != NULL) {
		// Copy the string until we hit the end or a dot.
		for (; *lastpos != '\0'; lastpos++) {
			*buf = *lastpos;
			if (*lastpos == '.')
				break;
			buf++;
		}

		*buf = '\0';
		lastpos = fname;
	}

	// Return the buffer length.
	return strlen(lastpos) + 1;
}

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
	va_end(ap);

	// Clean up path and return.
	return cleanup_path(final_path);
}

/**
 * Concatenates an extension to a file path.
 *
 * @param  final_path Path to the file without extension.
 * @param  ext        File extension without the dot.
 * @return            Size of the final buffer.
 */
size_t extcat(char *final_path, const char *ext) {
	// TODO: Improve performance of strcat.
	strncat(final_path, ".", UKI_MAX_PATH - 1);
	strncat(final_path, ext, UKI_MAX_PATH - 1);

	return strlen(final_path);
}

/**
 * Cleans up a path string and converts it to Windows separators if necessary.
 *
 * @param  path File path string.
 * @return      Final size of the cleaned up path.
 */
size_t cleanup_path(char *path) {
	char *pos;

	// Go through string searching for duplicate slashes.
	while ((pos = strstr(path, "//")) != NULL) {
		// Yay! Pointer shenanigans!
		pos++;
		*pos = '\0';
		pos++;

		// Append the rest of the string into itselt.
		strcat(path, pos);
	}

#ifdef WINDOWS
	// Fix strings that have a mix of Windows and UNIX slashes together.
	while ((pos = strstr(path, "\\/")) != NULL) {
		// Yay! Pointer shenanigans!
		pos++;
		*pos = '\0';
		pos++;

		// Append the rest of the string into itselt.
		strcat(path, pos);
	}

	// Convert UNIX path separators to Windows.
	pos = path;
	for (; *pos != '\0'; pos++) {
		if (*pos == '/')
			*pos = '\\';
	}
#endif

	return strlen(path);
}

/**
 * Sorts a directory listing alphabetically, giving priority to directories.
 *
 * @param list Directory listing structure.
 */
void sort_dirlist(dirlist_t *list) {
	qsort(list->list, list->size, sizeof(char*), sort_dirs_ascending);
}

/**
 * Lists the directory contents and stores it in a directory listing structure.
 * @warning Always set the dirlist_t.size to 0 before calling this function.
 *
 * @param  list      Directory list container. Allocated by this function.
 * @param  path      Path to the directory you want to list.
 * @param  recursive Should this listing be done recursively?
 * @return           UKI_OK if everything went OK.
 */
ssize_t list_directory_files(dirlist_t *list, const char *path,
							 const bool recursive) {
	ssize_t err;

	if ((err = n_list_directory_files(0, list, path, recursive)) < 0)
		return err;

	return UKI_OK;
}

/**
 * Lists the directory contents and stores it in a directory listing structure.
 * @remark  If list is NULL this function will return the size of the list to be
 *          allocated.
 * @warning Always set the dirlist_t.size to 0 before calling this function.
 *
 * @param  init_count Initial counter position.
 * @param  list       Directory list container. Allocated by this function.
 * @param  path       Path to the directory you want to list.
 * @param  recursive  Should this listing be done recursively?
 * @return            Current count if populating or list size if *list is NULL.
 */
ssize_t n_list_directory_files(size_t init_count, dirlist_t *list,
							   const char *path, const bool recursive) {
	char subpath[UKI_MAX_PATH];
	ssize_t count = init_count;
	ssize_t err;

#ifdef WINDOWS
	HANDLE hFind;
	WIN32_FIND_DATA fndData;
	char fpath[UKI_MAX_PATH];
	WCHAR szPathW[UKI_MAX_PATH];
	char szFilename[UKI_MAX_PATH];
#else
	DIR *dh;
	struct dirent *dir;
#endif

	// Allocate space for our list if needed.
	if (list != NULL) {
		if (list->size == 0) {
			err = n_list_directory_files(0, NULL, path, recursive);
			if (err < 0)
				return err;

			// Allocate listing array.
			list->size = err;
			list->list = (char**)malloc(err * sizeof(char*));
		}
	}

#ifdef WINDOWS
	// Add the wildcard for the file search function and convert to Unicode.
	pathcat(2, fpath, path, "/*");
	if (!StringAtoW(szPathW, fpath))
		return UKI_ERROR_CONVERSION_AW;

	// Find the first file in the directory.
	hFind = FindFirstFile(szPathW, &fndData);
#else
	// Open directory for listing.
	dh = opendir(path);
	if (dh == NULL) {
		return UKI_ERROR_DIRLIST_NOTFOUND;
	}
#endif

	// Read directory contents recursively.
#ifdef WINDOWS
	while (hFind != INVALID_HANDLE_VALUE) {
		// Convert the filename to a normal string.
		if(!StringWtoA(szFilename, fndData.cFileName))
			return UKI_ERROR_CONVERSION_WA;
#else
	while ((dir = readdir(dh)) != NULL) {
#endif
		// Ignore anything that starts with a dot.
#ifdef WINDOWS
		if (szFilename[0] == '.') {
			goto nextfile;
		}
#else
		if (dir->d_name[0] == '.') {
			continue;
		}
#endif

		// Decide what to do.
#ifdef WINDOWS
		switch (fndData.dwFileAttributes) {
		case FILE_ATTRIBUTE_DIRECTORY:
#else
		switch (dir->d_type) {
		case DT_DIR:
#endif
			// Is a directory, so only do something if we are recursive.
			if (recursive) {
				// Build path.
#ifdef WINDOWS
				pathcat(2, subpath, path, szFilename);
#else
				pathcat(2, subpath, path, dir->d_name);
#endif

				// Get listing recursively.
				err = n_list_directory_files(count, list, subpath, recursive);
				if (err < 0)
					return err;

				// Count up.
				count += (err - count);
			}
			break;
#ifdef WINDOWS
		case FILE_ATTRIBUTE_NORMAL:
#ifdef WINCE
		case FILE_ATTRIBUTE_ARCHIVE:
#endif
#else
		case DT_REG:
#endif
			if (list != NULL) {
				// Build path to file.
#ifdef WINDOWS
				pathcat(2, subpath, path, szFilename);
#else
				pathcat(2, subpath, path, dir->d_name);
#endif

				// Allocate string.
				list->list[count] = (char*)malloc((strlen(subpath) + 1) *
												  sizeof(char));
				strcpy(list->list[count], subpath);
			}

			count++;
			break;
#ifdef UNIX
		case DT_UNKNOWN:
			return UKI_ERROR_DIRLIST_FILEUNKNOWN;
#endif
		}

#ifdef WINDOWS
nextfile:
		// Continue to the next file.
		if (FindNextFile(hFind, &fndData) == 0) {
			if (GetLastError() == ERROR_NO_MORE_FILES) {
				// If there are no more files, close the handle and return.
				FindClose(hFind);
				hFind = INVALID_HANDLE_VALUE;
			} else {
				return UKI_ERROR;
			}
		}
#endif
	}

#ifdef UNIX
	// Clean up.
	closedir(dh);
#endif

	return count;
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
	WCHAR szPath[UKI_MAX_PATH];

	// Convert path string to Unicode.
	if (!StringAtoW(szPath, fpath)) {
		printf("ERROR: String conversion from ASCII to Unicode failed: '%s'\r\n",
			fpath);
		MessageBox(NULL, L"File path string conversion from ASCII to Unicode failed",
			L"Path Conversion Failed", MB_ICONEXCLAMATION | MB_OK);

		return false;
	}

	// Get file attributes and return.
	dwAttrib = GetFileAttributes(szPath);
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

/**
 * Counts the relative deepness of a file path.
 *
 * @param  path File path string.
 * @return      Number of slashes found in the file path.
 */
int count_dir_deepness(const char *path) {
	int count;
	const char *tmp = path;

	for (count = 0; *tmp != '\0'; tmp++) {
		if (*tmp == '/')
			count++;
	}

	return count;
}

/**
 * A sorting function to sort directories to be used with qsort. Prioritizing
 * directories.
 *
 * @param  a First parameter to sort.
 * @param  b Next parameter to sort.
 * @return   qsort decision integer.
 */
#ifdef WINDOWS
int __cdecl sort_dirs_ascending (const void *a, const void *b) {
#else
int sort_dirs_ascending (const void *a, const void *b) {
#endif
    const char *pa = *(char *const *)a;
    const char *pb = *(char *const *)b;
	int da = count_dir_deepness(pa);
	int db = count_dir_deepness(pb);
	int dr = db - da;

	// Don't ask me about this. It just works.
    return dr ? dr : strcmp(pa, pb);
}
