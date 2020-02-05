/**
 * template.c
 * A simple templating library to construct all that sweet content.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#include "template.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// Substitution types.
#define SUBSTITUTE_STRING   0
#define SUBSTITUTE_VARIABLE 1
#define SUBSTITUTE_TEMPLATE 2

// Position structure.
typedef struct {
	size_t begin;
	size_t end;
} spos_t;

// Private methods.
spos_t substpos(const char *haystack, const char *needle, const uint8_t type);
void replace_string(char **haystack, const char *needle, const char *substr,
					const uint8_t type);

/**
 * Substitutes template references found inside a template.
 *
 * @param template Template file contents.
 */
bool substitute_templates(char **template) {
	char fname[UKI_MAX_TEMPLATE_NAME];

	if (sscanf(*template, "%*[^[][%49[^]]]%*[^]]", fname) == 1) {
		replace_string(template, fname, "=HELLO WORLD THIS IS OUR AWESOME THING=", SUBSTITUTE_TEMPLATE);

		if (!substitute_templates(template))
			return true;
	} else {
		return false;
	}
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
