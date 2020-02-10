/**
 * template.c
 * A simple templating library to construct all that sweet content.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#include "constants.h"
#include "template.h"
#include "fileutils.h"
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

// Position structure.
typedef struct {
	size_t begin;
	size_t end;
} spos_t;

// Private methods.
spos_t substpos(const char *haystack, const char *needle, const uint8_t type);
void replace_string(char **haystack, const char *needle, const char *substr,
					const uint8_t type);
int substitute_templates(char **template);

/**
 * Initializes the templating engine.
 *
 * @param _wiki_root Path to the root of the uki wiki.
 */
void initialize_templating(const char *_wiki_root) {
	wiki_root_path = _wiki_root;
}

/**
 * Substitutes template references found inside a template.
 *
 * @param  template Template file contents.
 * @return          UKI_OK when all the substitutions were successful.
 */
int substitute_templates(char **template) {
	char fname[UKI_MAX_TEMPLATE_NAME];
	char *ntemp;
	int err;
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
int render_template(char **rendered, const char *template_name) {
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
	int err;

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
int render_article(char **filled_template, const char *path) {
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
 * @param  type     Is this a template or variable substitution?
 */
void replace_string(char **haystack, const char *needle, const char *substr,
					const uint8_t type) {
	char *left;
	char *right;
	spos_t pos = substpos(*haystack, needle, type);

	// Allocate memory for our left and right side strings.
	left = (char*)malloc((pos.begin + 1) * sizeof(char));
	right = (char*)malloc((strlen(*haystack) - pos.end + 1) * sizeof(char));

	// Populate our left and right side strings.
	memcpy(left, *haystack, pos.begin);
	left[pos.begin] = '\0';
	strcpy(right, *haystack + pos.end);

	// Reallocate memory for our substituted string and copy its contents.
	*haystack = (char*)realloc(*haystack, (strlen(left) + strlen(right) +
										   strlen(substr) + 1) * sizeof(char));
	strcpy(*haystack, left);
	strcpy(*haystack + strlen(left), substr);
	strcpy(*haystack + strlen(left) + strlen(substr), right);

	// Clean up our mess.
	free(left);
	free(right);
}

/**
 * Gets a substitution candidate positions.
 *
 * @param  haystack Text containing the candidate substring.
 * @param  needle   What we want to find inside.
 * @param  type     Is this a template or variable substitution?
 * @return          Position of the string inside the haystack.
 */
spos_t substpos(const char *haystack, const char *needle, const uint8_t type) {
	spos_t pos;
	char cmpneedle[UKI_MAX_TEMPLATE_NAME + 3];
	char *occur;

	// Build comparison needle.
	if (type == SUBSTITUTE_TEMPLATE) {
		sprintf(cmpneedle, "[%s]", needle);
	} else if (type == SUBSTITUTE_VARIABLE) {
		sprintf(cmpneedle, "%%%s%%", needle);
	} else {
		strncpy(cmpneedle, needle, UKI_MAX_TEMPLATE_NAME);
	}

	// Get substring position.
	occur = strstr(haystack, cmpneedle);
	if (occur != NULL) {
		pos.begin = (size_t)(occur - haystack);
		pos.end = pos.begin + strlen(cmpneedle);
	} else {
		pos.begin = 0;
		pos.end = 0;
	}

	return pos;
}
