/**
 * fileutils.h
 * A file utility helper.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_

#include "windowshelper.h"
#include "constants.h"
#ifndef WINDOWS
#include <sys/types.h>
#include <stddef.h>
#include <stdbool.h>
#endif

// Directory listing container.
typedef struct {
	size_t  size;
	char   **list;
} dirlist_t;

size_t cleanup_path(char *path);
void free_dirlist(dirlist_t list);
bool file_exists(const char *fpath);
size_t extcat(char *final_path, const char *ext);
size_t pathcat(int npaths, char *final_path, ...);
ssize_t list_directory_files(dirlist_t *list, const char *path,
							 const bool recursive);
long file_contents_size(const char *fname);
size_t slurp_file(char **contents, const char *fname);

#endif /* _FILEUTILS_H_ */
