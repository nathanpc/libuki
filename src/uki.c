/**
 * uki.c
 * A micro-wiki for personal stuff.
 *
 * @author: Nathan Campos <nathan@innoveworkshop.com>
 */

#define UKI_DLL_EXPORTS
#include "uki.h"
#include "fileutils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef UNIX
#include <stdbool.h>
#endif

// Private variables.
char *wiki_root;
bool uki_initialized = false;
uki_variable_container configs;
uki_variable_container variables;
uki_article_container articles;
uki_template_container templates;

// Private methods.
uki_error populate_variable_container(const char *wiki_root,
									  const char *var_fname,
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
uki_error uki_initialize(const char *wiki_path) {
	uki_error err;
	uki_initialized = true;

	// Copy the wiki root path string.
	wiki_root = (char*)malloc((strlen(wiki_path) + 1) * sizeof(char));
	strcpy(wiki_root, wiki_path);

	// Populate the variable containers.
	if ((err = populate_variable_container(wiki_root, UKI_MANIFEST_PATH,
										   &configs)) != UKI_OK)
		return err;
	if ((err = populate_variable_container(wiki_root, UKI_VARIABLE_PATH,
										   &variables)) != UKI_OK)
		return err;

	// Initialize templating engine and populate the templates container.
	initialize_templating(&templates, wiki_root);
	if ((err = populate_templates(&templates)) != UKI_OK)
		return err;

	// Initialize and populate the articles container.
	initialize_articles(&articles, wiki_root);
	if ((err = populate_articles(&articles)) != UKI_OK)
		return err;

	return UKI_OK;
}

/**
 * Renders an article from its text contents.
 *
 * @param  content  Content to be rendered. (Reallocated by this function)
 * @param  deepness Article deepness inside the articles folder.
 * @return          UKI_OK if the operation was successful.
 */
uki_error uki_render_article_from_text(char **content, const int deepness) {
	return substitute_assets(content, deepness);
}

/**
 * Renders a template from its text contents.
 *
 * @param  content  Content to be rendered. (Reallocated by this function)
 * @param  deepness Template deepness inside the templates folder.
 * @return          UKI_OK if the operation was successful.
 */
uki_error uki_render_template_from_text(char **content, const int deepness) {
	return substitute_assets(content, deepness);
}

/**
 * Renders an article by its index.
 *
 * @param  rendered Rendered page contents (Allocated by this fuction).
 * @param  index    Article index.
 * @param  preview  Is this for preview? (Will change the contents of the page)
 * @return          UKI_OK if the operation was successful.
 */
uki_error uki_render_article(char **rendered, const size_t index,
							 const bool preview) {
	uki_article_t article;
	char fpath[UKI_MAX_PATH];
	uki_error err;

	// Get the article.
	article = uki_article(index);
	if (article.name == NULL)
		return UKI_ERROR_INDEX_NOT_FOUND;

	// Get the file path.
	if ((err = uki_article_fpath(fpath, article)) != UKI_OK)
		return err;

	// Check if there is an article there.
	if (!file_exists(fpath))
		return UKI_ERROR_NOARTICLE;

	// Slurp file.
	slurp_file(rendered, fpath);
	if (rendered == NULL)
		return UKI_ERROR_PARSING_ARTICLE;

	// Substitute asset paths if we are in preview mode.
	if (preview)
		return uki_render_article_from_text(rendered, article.deepness);

	return UKI_OK;
}

/**
 * Render a wiki page.
 *
 * @param  rendered Rendered page text (will be allocated by this function).
 * @param  page     Relative path to the page (without the extension).
 * @return          UKI_OK if there were no errors.
 */
uki_error uki_render_page(char **rendered, const char *page) {
	char article_path[UKI_MAX_PATH];
	uki_error err;

	// Get main template.
	int idx = find_variable(UKI_VAR_MAIN_TEMPLATE, configs);
	if (idx < 0)
		return UKI_ERROR_NOMAINTEMPLATE;

	// Render template for placing article into.
	if ((err = render_template(rendered, configs.list[idx].value)) != UKI_OK)
		return err;

	// Build article path and render it.
	pathcat(3, article_path, wiki_root, UKI_ARTICLE_ROOT, page);
	extcat(article_path, UKI_ARTICLE_EXT);
	if ((err = render_article_in_template(rendered, article_path)) != UKI_OK)
		return err;

	return render_variables(rendered, variables);
}

/**
 * Gets the number of available configurations.
 *
 * @return Number of available configurations.
 */
size_t uki_configs_available() {
	return configs.size;
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
 * Gets the number of available variables.
 *
 * @return Number of available variables.
 */
size_t uki_variables_available() {
	return variables.size;
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
 * Gets the number of available articles.
 *
 * @return Number of available articles.
 */
size_t uki_articles_available() {
	return articles.size;
}

/**
 * Gets a uki article structure by its index.
 *
 * @param  index Article index.
 * @param        The article structure if it was found. NULL otherwise.
 */
uki_article_t uki_article(const size_t index) {
	return find_article_i(index, articles);
}

/**
 * Add a new article.
 *
 * @param  article_path Complete path to the article file.
 * @return              Recently added article.
 */
uki_article_t uki_add_article(const char *article_path) {
	return add_article(&articles, article_path);
}

/**
 * Gets the number of available templates.
 *
 * @return Number of available templates.
 */
size_t uki_templates_available() {
	return templates.size;
}

/**
 * Gets a uki template structure by its index.
 *
 * @param  index Template index.
 * @param        The template structure if it was found. NULL otherwise.
 */
uki_template_t uki_template(const size_t index) {
	return find_template_i(index, templates);
}

/**
 * Add a new template.
 *
 * @param  template_path Complete path to the template file.
 * @return               Recently added template.
 */
uki_template_t uki_add_template(const char *template_path) {
	return add_template(&templates, template_path);
}

/**
 * Creates a file path to an article.
 *
 * @param  fpath   Pre-allocated string buffer to store the article file path.
 * @param  article Article structure.
 * @return         UKI_OK if the operation was successful.
 */
uki_error uki_article_fpath(char *fpath, const uki_article_t article) {
	// Build article path.
	pathcat(3, fpath, wiki_root, UKI_ARTICLE_ROOT, article.path);
	return UKI_OK;
}

/**
 * Creates a file path to a template.
 *
 * @param  fpath    Pre-allocated string buffer to store the template file path.
 * @param  template Template structure.
 * @return          UKI_OK if the operation was successful.
 */
uki_error uki_template_fpath(char *fpath, const uki_template_t template) {
	// Build template path.
	pathcat(3, fpath, wiki_root, UKI_TEMPLATE_ROOT, template.path);
	return UKI_OK;
}

/**
 * Gets the path to the articles folder.
 *
 * @param  fpath Pre-allocated string buffer to store the articles folder path.
 * @return       UKI_OK if the operation was successful.
 */
uki_error uki_folder_articles(char *fpath) {
	// Build article path.
	pathcat(2, fpath, wiki_root, UKI_ARTICLE_ROOT);
	return UKI_OK;
}

/**
 * Gets the path to the templates folder.
 *
 * @param  fpath Pre-allocated string buffer to store the templates folder path.
 * @return       UKI_OK if the operation was successful.
 */
uki_error uki_folder_templates(char *fpath) {
	// Build template path.
	pathcat(2, fpath, wiki_root, UKI_TEMPLATE_ROOT);
	return UKI_OK;
}

/**
 * Gets a error message beased on a error code from uki.
 *
 * @param  ecode Error code.
 * @return       Error message.
 */
const char* uki_error_msg(const uki_error ecode) {
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
	case UKI_ERROR_INDEX_NOT_FOUND:
		return "Index to template/article not found.\n";
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
	case UKI_ERROR_DIRLIST_NOTFOUND:
		return "Couldn't open directory for listing.\n";
	case UKI_ERROR_DIRLIST_FILEUNKNOWN:
		return "Your filesystem doesn't support dirent->d_type. Sorry.\n";
	case UKI_ERROR_CONVERSION_AW:
		return "String conversion from ASCII to Unicode failed\n";
	case UKI_ERROR_CONVERSION_WA:
		return "String conversion from Unicode to ASCII failed\n";
	case UKI_ERROR_REGEX_ASSET_IMAGE:
		return "There was a regex failure while substituting image assets.\n";
	case UKI_ERROR:
		return "General error.\n";
	}

	return "Unknown error.\n";
}

/**
 * Clean up our mess.
 */
void uki_clean() {
	if (uki_initialized) {
		free(wiki_root);
		free_variables(configs);
		free_variables(variables);
		free_articles(articles);
	}
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
uki_error populate_variable_container(const char *wiki_root, const char *var_fname,
								uki_variable_container *container) {
	// Get path string length and allocate some memory.
	size_t path_len = strlen(wiki_root) + 1 + strlen(var_fname);
	char *var_path = (char*)malloc(path_len * sizeof(char));

	// Build the wiki variables file path and check for its existance.
	pathcat(2, var_path, wiki_root, var_fname);
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
