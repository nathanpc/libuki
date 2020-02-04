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
uki_variable_container configs;
uki_variable_container variables;

// Private methods.
bool file_exists(const char *fpath);
long file_contents_size(const char *fname);
size_t slurp_file(char **contents, const char *fname);
int populate_variable_container(const char *wiki_root, const char *var_fname,
								uki_variable_container *container);

/**
 * Initializes the wiki.
 *
 * @param  wiki_path Path to the root of the uki wiki.
 * @return           UKI_OK if the initialization was completed successfully.
 */
int uki_initialize(const char *wiki_path) {
	int err;

	// Copy the wiki root path string.
	wiki_root = (char*)malloc((strlen(wiki_path) + 1) * sizeof(char));
	strcpy(wiki_root, wiki_path);

	if ((err = populate_variable_container(wiki_root, UKI_MANIFEST_PATH, &configs)) != UKI_OK)
		return err;
	if ((err = populate_variable_container(wiki_root, UKI_VARIABLE_PATH, &variables)) != UKI_OK)
		return err;

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
 * Gets a uki configuration by its index.
 *
 * @param  index Configuration index.
 * @param  var   The variable structure if it was found. NULL otherwise.
 * @return       TRUE if the configuration was found.
 */
bool uki_config(const uint8_t index, uki_variable_t *var) {
	// Check if the index is out of bounds.
	if (index >= configs.size) {
		var = NULL;
		return false;
	}

	*var = configs.list[index];
	return true;
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
	free_variables(configs);
	free_variables(variables);
}

/**
 * Populates a variabl/configuration container.
 *
 * @param  wiki_root Path to the root of the uki wiki.
 * @param  var_fname Path to the variabl/configuration file.
 * @param  container Variable container.
 * @return           UKI_OK if the operation was successful. Respective error
 *                   code otherwise.
 */
int populate_variable_container(const char *wiki_root, const char *var_fname,
								uki_variable_container *container) {
	// Get path string length and allocate some memory.
	size_t path_len = strlen(wiki_root) + 1 + strlen(var_fname);
	char *var_path = (char*)malloc(path_len * sizeof(char));

	// Build the wiki variables file path and check for its existance.
	sprintf(var_path, "%s%s", wiki_root, var_fname);
	if (!file_exists(var_path)) {
		free(var_path);
		return UKI_ERROR_NOVARIABLES;
	}

	// Initialize and populate variable container.
	initialize_variables(container);
	if (!populate_variables(container, var_path)) {
		free(var_path);
		return UKI_ERROR_PARSING_VARIABLES;
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
	return access(fpath, F_OK) != -1;
}
