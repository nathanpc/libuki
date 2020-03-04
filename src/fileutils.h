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
#ifdef UNIX
#include <sys/types.h>
#include <stddef.h>
#include <stdbool.h>
#endif

// Directory listing container.
typedef struct {
	size_t  size;
	char   **list;
} dirlist_t;

// Checking.
bool file_exists(const char *fpath);
bool file_ext_match(const char *fpath, const char *ext);

// Path manipulaton.
size_t cleanup_path(char *path);
size_t extcat(char *final_path, const char *ext);
size_t pathcat(int npaths, char *final_path, ...);
size_t basename_noext(char *fname, const char *path);
size_t parent_dir_name(char *pdir, const char *path);

// Directory listing.
void free_dirlist(dirlist_t list);
int path_deepness(const char *path);
void sort_dirlist(dirlist_t *list);
ssize_t list_directory_files(dirlist_t *list, const char *path,
							 const bool recursive);

// File content.
long file_contents_size(const char *fname);
size_t slurp_file(char **contents, const char *fname);
uki_error substitute_assets(char **html, const int deepness);

#endif /* _FILEUTILS_H_ */
