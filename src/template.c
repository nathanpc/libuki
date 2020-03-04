/**
 * template.c
 * A simple templating library to construct all that sweet content.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#include "constants.h"
#include "template.h"
#include "fileutils.h"
#include "strutils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef WINDOWS
#include <stdint.h>
#endif

// Substitution types.
#define SUBSTITUTE_STRING   0
#define SUBSTITUTE_VARIABLE 1
#define SUBSTITUTE_TEMPLATE 2

// sscanf matching.
#define TEMPLATE_FILE_MATCH "%*[^[][%49[^]]]%*[^]]"
#define TEMPLATE_VAR_MATCH  "%*[^%]%%%49[^%]%%%*[^%]"
#define TEMPLATE_BODY_MATCH "%_body_%"

// Global variables.
const char *wiki_root_path;
char template_path[UKI_MAX_PATH];

// Private methods.
void replace_string(char **haystack, const char *needle, const char *substr,
					const uint8_t type);
uki_error substitute_templates(char **template);
void populate_template_from_path(uki_template_t *template, const char *fpath);
void push_template(uki_template_container *container, uki_template_t template);

/**
 * Initializes the templating engine.
 *
 * @param container  Template container.
 * @param _wiki_root Path to the root of the uki wiki.
 */
void initialize_templating(uki_template_container *container,
						   const char *_wiki_root) {
	// Initialize paths.
	wiki_root_path = _wiki_root;
	pathcat(2, template_path, wiki_root_path, UKI_TEMPLATE_ROOT);

	// Initialize the template container.
	container->size = 0;
	container->list = malloc(sizeof(uki_template_t));
}

/**
 * Get a template by its index.
 *
 * @param  index     Template index.
 * @param  container Template container to search into.
 * @return           The template structure if it was found. NULL otherwise.
 */
uki_template_t find_template_i(const size_t index,
							   const uki_template_container container) {
	// Check if the index is out of bounds.
	if (index >= container.size) {
		uki_template_t nl;
		nl.name = NULL;
		nl.path = NULL;
		nl.parent = NULL;
		nl.deepness = 0;

		return nl;
	}

	return container.list[index];
}

/**
 * Pushes a template into the container.
 *
 * @param container Template container.
 * @param template  Template structure to be added.
 */
void push_template(uki_template_container *container, uki_template_t template) {
	container->list = realloc(container->list, sizeof(uki_template_t) *
							  (container->size + 1));
	container->list[container->size++] = template;
}

/**
 * Populates a template structure using a file path.
 * @remark This function ignores the template root path, but it must be present.
 *
 * @param template Template structure to be populated.
 * @param fpath    Complete file path. Template root part will be ignored.
 */
void populate_template_from_path(uki_template_t *template, const char *fpath) {
	const char *reldir = fpath;

	// Skip the template root directory.
	reldir += strlen(template_path);

	// Allocate memory and populate the structure.
	template->name = (char*)malloc(basename_noext(NULL, reldir) *
								   sizeof(char));
	template->path = (char*)malloc((strlen(reldir) + 1) * sizeof(char));
	basename_noext(template->name, reldir);
	strcpy(template->path, reldir);
	template->deepness = path_deepness(template->path);

	// Populate the parent path.
	if (template->deepness > 0) {
		template->parent = (char*)malloc(parent_dir_name(NULL, reldir) *
										 sizeof(char));
		parent_dir_name(template->parent, reldir);
	} else {
		template->parent = NULL;
	}
}

/**
 * Adds a template to the container by its path.
 *
 * @param  container Template container.
 * @param  fpath     Complete path to the template.
 * @return           The recently added template.
 */
uki_template_t add_template(uki_template_container *container,
							const char *fpath) {
	uki_template_t template;

	// Populate template and push it into the container.
	populate_template_from_path(&template, fpath);
	push_template(container, template);

	return template;
}

/**
 * Populates the templates container.
 *
 * @param  container Template container.
 * @return           UKI_OK if the operation was successful.
 */
uki_error populate_templates(uki_template_container *container) {
	dirlist_t dirlist;
	uki_error err;
	size_t i;

	// Go through the directory and sort the findings.
	dirlist.size = 0;
	if ((err = list_directory_files(&dirlist, template_path, true)) != UKI_OK)
		return err;
	sort_dirlist(&dirlist);

	// Push the files into the template container.
	for (i = 0; i < dirlist.size; i++) {
		if (file_ext_match(dirlist.list[i], UKI_TEMPLATE_EXT))
			add_template(container, dirlist.list[i]);
	}

	// Clean up and return.
	free_dirlist(dirlist);
	return UKI_OK;
}

/**
 * Cleans up the mess we left behind.
 *
 * @param container Template container to be emptied.
 */
void free_templates(uki_template_container container) {
	size_t i;
	for (i = 0; i < container.size; i++) {
		free(container.list[i].path);
		free(container.list[i].name);
		free(container.list[i].parent);
	}

	free(container.list);
	container.size = 0;
}

/**
 * Substitutes template references found inside a template.
 *
 * @param  template Template file contents.
 * @return          UKI_OK when all the substitutions were successful.
 */
uki_error substitute_templates(char **template) {
	char fname[UKI_MAX_TEMPLATE_NAME];
	char *ntemp;
	uki_error err;
	int nmatches;

	// Try to find a template tag.
	nmatches = sscanf(*template, TEMPLATE_FILE_MATCH, fname);
	if (nmatches == 1) {
		// Get template file contents.
		if ((err = render_template(&ntemp, fname)) != UKI_OK) {
			return err;
		}

		// If found, replace it.
		replace_string(template, fname, ntemp, SUBSTITUTE_TEMPLATE);
		free(ntemp);

		// Continue until we don't have any one left.
		if (substitute_templates(template) == UKI_OK)
			return UKI_OK;
#ifdef WINDOWS
	} else if ((nmatches == EOF) || (nmatches == 0)) {
#else
	} else if (nmatches == EOF) {
#endif
		// Finished substituting the templates.
		return UKI_OK;
	}

	// Looks like we finally don't have any tags left.
	return UKI_ERROR_PARSING_TEMPLATE;
}

/**
 * Renders a page template and returns it.
 *
 * @param  rendered      The final rendered page. Allocated by this function.
 * @param  template_name The template file to be rendered.
 * @return               UKI_OK if the rendering went smoothly.
 */
uki_error render_template(char **rendered, const char *template_name) {
	// Build template path.
	char template_path[UKI_MAX_PATH];
	pathcat(3, template_path, wiki_root_path, UKI_TEMPLATE_ROOT, template_name);
	extcat(template_path, UKI_TEMPLATE_EXT);

	// Check if there is a template there.
	if (!file_exists(template_path))
		return UKI_ERROR_NOTEMPLATE;

	// Slurp file.
	slurp_file(rendered, template_path);
	if (*rendered == NULL)
		return UKI_ERROR_READING_TEMPLATE;

	// Render the template and return.
	return substitute_templates(rendered);
}

/**
 * Substitutes variable references found inside a template.
 *
 * @param  filled_template Template contents already all populated.
 * @param  variables       Variables container.
 * @return                 UKI_OK when all the substitutions were successful.
 */
int render_variables(char **filled_template,
					 const uki_variable_container variables) {
	char vname[VARIABLE_KEY_MAX_CHAR];
	int ivar;
	int nmatches;
	uki_error err;

	// Try to find a variable tag.
	nmatches = sscanf(*filled_template, TEMPLATE_VAR_MATCH, vname);
	if (nmatches == 1) {
		// Get variable contents.
		if ((ivar = find_variable(vname, variables)) < 0) {
			//fprintf(stderr, "WARNING: Couldn't find variable '%s'.\n", vname);
			return UKI_ERROR_VARIABLE_NOTFOUND;
		}

		// If found, replace it.
		replace_string(filled_template, vname,
					   variables.list[ivar].value, SUBSTITUTE_VARIABLE);

		// Continue until we don't have any one left.
		err = render_variables(filled_template, variables);
		if ((err == UKI_OK) || (err ==  UKI_ERROR_VARIABLE_NOTFOUND))
			return err;
#ifdef WINDOWS
	} else if ((nmatches == EOF) || (nmatches == 0)) {
#else
	} else if (nmatches == EOF) {
#endif
		// Finished substituting the templates.
		return UKI_OK;
	}

	// Looks like we finally don't have any tags left.
	return UKI_ERROR_PARSING_TEMPLATE;
}

/**
 * Substitutes article page found inside a template.
 *
 * @param  filled_template Template contents already all populated.
 * @param  path            Article page absolute path.
 * @return                 UKI_OK when the substitution was successful.
 */
uki_error render_article_in_template(char **filled_template, const char *path) {
	char *article;

	// Check if there is an article there.
	if (!file_exists(path))
		return UKI_ERROR_NOARTICLE;

	// Check if there is a body variable available in the template.
	if (strstr(*filled_template, TEMPLATE_BODY_MATCH) == NULL) {
		return UKI_ERROR_BODYVAR_NOTFOUND;
	}

	// Slurp file.
	slurp_file(&article, path);
	if (article == NULL)
		return UKI_ERROR_PARSING_ARTICLE;

	// Replace the body variable and return.
	replace_string(filled_template, TEMPLATE_BODY_MATCH, article,
				   SUBSTITUTE_STRING);
	return UKI_OK;
}

/**
 * Puts a string inside another string given a substring to find.
 *
 * @param haystack String that will have its contents changed.
 * @param needle   What we want to find inside.
 * @param substr   Substitute string to be added to the base string.
 * @param type     Is this a template or variable substitution?
 */
void replace_string(char **haystack, const char *needle, const char *substr,
					const uint8_t type) {
	char cmpneedle[UKI_MAX_TEMPLATE_NAME + 3];

	// Build comparison needle.
	if (type == SUBSTITUTE_TEMPLATE) {
		sprintf(cmpneedle, "[%s]", needle);
	} else if (type == SUBSTITUTE_VARIABLE) {
		sprintf(cmpneedle, "%%%s%%", needle);
	} else {
		strncpy(cmpneedle, needle, UKI_MAX_TEMPLATE_NAME);
	}

	// Replace the string.
	strsreplace(haystack, cmpneedle, substr);
}
