/**
 * uki.c
 * A micro-wiki for personal stuff.
 *
 * @author: Nathan Campos <nathan@innoveworkshop.com>
 */

#define UKI_DLL_EXPORTS
#include "uki.h"
#include "fileutils.h"
#include "template.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef WINDOWS
#include <stdbool.h>
#endif

// Private variables.
char *wiki_root;
uki_variable_container configs;
uki_variable_container variables;

// Private methods.
int populate_variable_container(const char *wiki_root, const char *var_fname,
								uki_variable_container *container);

#ifdef WINDOWS
/**
 * DLL main entry point.
 *
 * @param  hModule            Module handler.
 * @param  ul_reason_for_call Reason for calling this DLL.
 * @param  lpReserved         Reserved for the future.
 * @return                    TRUE if the DLL should load.
 */
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }

    return TRUE;
}
#endif

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

	// Populate the variable containers.
	if ((err = populate_variable_container(wiki_root, UKI_MANIFEST_PATH, &configs)) != UKI_OK)
		return err;
	if ((err = populate_variable_container(wiki_root, UKI_VARIABLE_PATH, &variables)) != UKI_OK)
		return err;

	// Initialize templating engine.
	initialize_templating(wiki_root);

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
	char article_path[UKI_MAX_PATH];
	int err;

	// Get main template.
	int idx = find_variable(UKI_VAR_MAIN_TEMPLATE, configs);
	if (idx < 0)
		return UKI_ERROR_NOMAINTEMPLATE;

	// Render template for placing article into.
	if ((err = render_template(rendered, configs.list[idx].value)) != UKI_OK)
		return err;

	// Build article path and render it.
#ifdef WINDOWS
	_snprintf(article_path, UKI_MAX_PATH, "%s%s%s.%s", wiki_root,
			  UKI_ARTICLE_ROOT, page, UKI_ARTICLE_EXT);
#else
	snprintf(article_path, UKI_MAX_PATH, "%s%s%s.%s", wiki_root,
			 UKI_ARTICLE_ROOT, page, UKI_ARTICLE_EXT);
#endif
	if ((err = render_article(rendered, article_path)) != UKI_OK)
		return err;

	return render_variables(rendered, variables);
}

/**
 * Gets a uki configuration by its index.
 *
 * @param  index Configuration index.
 * @return       The variable structure if it was found. NULL otherwise.
 */
uki_variable_t uki_config(const uint8_t index) {
	return find_variable_i(index, configs);
}

/**
 * Gets a uki variable by its index.
 *
 * @param  index Variable index.
 * @return       The variable structure if it was found. NULL otherwise.
 */
uki_variable_t uki_variable(const uint8_t index) {
	return find_variable_i(index, variables);
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
	case UKI_ERROR_NOTEMPLATE:
		return "Template file not found.\n";
	case UKI_ERROR_NOMAINTEMPLATE:
		return "No 'main_template' variable set in the manifest.\n";
	case UKI_ERROR_VARIABLE_NOTFOUND:
		return "Variable not found.\n";
	case UKI_ERROR_BODYVAR_NOTFOUND:
		return "Body variable not found in the main template.\n";
	case UKI_ERROR_PARSING_ARTICLE:
		return "Error occured while parsing an article.\n";
	case UKI_ERROR_PARSING_VARIABLES:
		return "Error occured while parsing the variables file.\n";
	case UKI_ERROR_PARSING_TEMPLATE:
		return "Error occured while parsing a template file.\n";
	case UKI_ERROR_READING_TEMPLATE:
		return "Error occured while reading a template file.\n";
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
 * Populates a variable/configuration container.
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
