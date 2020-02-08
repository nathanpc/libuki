/**
 * fileutils.h
 * A file utility helper.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_

#include "windowshelper.h"

#ifndef WINDOWS
#include <stddef.h>
#include <stdbool.h>
#endif

bool file_exists(const char *fpath);
long file_contents_size(const char *fname);
size_t slurp_file(char **contents, const char *fname);

#endif /* _FILEUTILS_H_ */
