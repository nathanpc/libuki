/**
 * uki.c
 * A micro-wiki for personal stuff.
 *
 * @author: Nathan Campos <nathan@innoveworkshop.com>
 */

#include "uki.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

// Private variables.
char *wiki_root;

// Private methods.
bool file_exists(const char *fpath);
long file_contents_size(const char *fname);
size_t slurp_file(char **contents, const char *fname);

/**
 * Initializes the wiki.
 *
 * @param  wiki_path Path to the root of the uki wiki.
 * @return           UKI_OK if it's a valid uki wiki root.
 */
int uki_initialize(const char *wiki_path) {
	// Get path string length and allocate some memory.
	size_t path_len = strlen(wiki_path) + 1;
	char *wiki_manifest = (char*)malloc((path_len + strlen(UKI_MANIFEST_PATH)) *
										sizeof(char));

	// Copy the wiki root.
	wiki_root = (char*)malloc(path_len * sizeof(char));
	strncpy(wiki_root, wiki_path, path_len);
	snprintf(wiki_manifest, path_len + strlen(UKI_MANIFEST_PATH),
			 "%s%s", wiki_path, UKI_MANIFEST_PATH);

	// Check if a manifest exists.
	if (!file_exists(wiki_manifest)) {
		free(wiki_manifest);
		return UKI_ERROR_NOMANIFEST;
	}

	free(wiki_manifest);
	return UKI_OK;
}

/**
 * Render a wiki page.
 *
 * @param  rendered Rendered page text (will be allocated by this function).
 * @param  page     Relative path to the page (without the extension).
 * @return          UKI_OK if there were no errors.
 */
int uki_render_page(char **rendered, const char *page) {
	// Build article path.
	char article_path[UKI_MAX_PATH];
	snprintf(article_path, UKI_MAX_PATH, "%s%s%s.%s", wiki_root,
			 UKI_ARTICLE_ROOT, page, UKI_ARTICLE_EXT);

	// Check if there is an article there.
	if (!file_exists(article_path)) {
		return UKI_ERROR_NOARTICLE;
	}

	// Slurp file.
	slurp_file(rendered, article_path);
	if (*rendered == NULL) {
		return UKI_ERROR_PARSING_ARTICLE;
	}

	return UKI_OK;
}

/**
 * Gets a error message beased on a error code from uki.
 *
 * @param  ecode Error code.
 * @return       Error message.
 */
const char* uki_error_msg(const int ecode) {
	switch (ecode) {
	case UKI_ERROR_NOMANIFEST:
		return "No manifest found in the provided path.\n";
	case UKI_ERROR_NOARTICLE:
		return "Article not found.\n";
	case UKI_ERROR_PARSING_ARTICLE:
		return "Error occured while parsing an article.\n";
	case UKI_ERROR:
		return "General error.\n";
	}

	return "Unknown error.\n";
}

/**
 * Clean up our mess.
 */
void uki_clean() {
	free(wiki_root);
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
		*contents = (char*)malloc(1 + (fsize * sizeof(char)));
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
