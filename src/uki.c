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
uki_variable_container variables;

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
	size_t rpath_len = strlen(wiki_path) + 1;
	size_t vpath_len = rpath_len + strlen(UKI_VARIABLE_PATH);
	char *wiki_var_path = (char*)malloc(vpath_len * sizeof(char));

	// Copy the wiki root path string.
	wiki_root = (char*)malloc(rpath_len * sizeof(char));
	strncpy(wiki_root, wiki_path, rpath_len);

	// Build the wiki variables file path and check for its existance.
	snprintf(wiki_var_path, vpath_len, "%s%s", wiki_path, UKI_VARIABLE_PATH);
	if (!file_exists(wiki_var_path)) {
		free(wiki_var_path);
		return UKI_ERROR_NOVARIABLES;
	}

	// Initialize and populate variable container.
	initialize_variables(&variables);
	if (!populate_variables(&variables, wiki_var_path)) {
		free(wiki_var_path);
		return UKI_ERROR_PARSING_VARIABLES;
	}

	free(wiki_var_path);
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
 * Gets a uki variable by its index.
 *
 * @param  index Variable index.
 * @param  var   The variable structure if it was found. NULL otherwise.
 * @return       TRUE if the variable was found.
 */
bool uki_variable(const uint8_t index, uki_variable_t *var) {
	// Check if the index is out of bounds.
	if (index >= variables.size) {
		var = NULL;
		return false;
	}

	*var = variables.list[index];
	return true;
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
		return "No manifest file found in the provided path.\n";
	case UKI_ERROR_NOVARIABLES:
		return "No variables file found in the provided path.\n";
	case UKI_ERROR_NOARTICLE:
		return "Article not found.\n";
	case UKI_ERROR_PARSING_ARTICLE:
		return "Error occured while parsing an article.\n";
	case UKI_ERROR_PARSING_VARIABLES:
		return "Error occured while parsing the variables file.\n";
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
	free_variables(variables);
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
