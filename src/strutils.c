/**
 * strutils.h
 * A collection of useful string manipulation utilities.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#include "strutils.h"
#include <string.h>

/**
 * Puts a string inside another string given a position.
 *
 * @param haystack String that will have its contents changed.
 * @param pos      Position of the text that will be replaced.
 * @param substr   Substitute string to be added to the base string.
 */
void strnreplace(char **haystack, const spos_t pos, const char *substr) {
	char *left;
	char *right;

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
 * Puts a string inside another string given a substring to find.
 *
 * @param haystack String that will have its contents changed.
 * @param needle   What we want to find inside.
 * @param substr   Substitute string to be added to the base string.
 */
void strsreplace(char **haystack, const char *needle, const char *substr) {
	spos_t pos;

	// Get the needle position inside the haystack.
	pos = strsubstpos(*haystack, needle);

	// Substitute based on the needle position.
	strnreplace(haystack, pos, substr);
}

/**
 * Gets a substitution candidate positions.
 *
 * @param  haystack Text containing the candidate substring.
 * @param  needle   What we want to find inside.
 * @return          Position of the string inside the haystack.
 */
spos_t strsubstpos(const char *haystack, const char *needle) {
	spos_t pos;
	char *occur;

	// Get substring position.
	occur = strstr(haystack, needle);
	if (occur != NULL) {
		pos.begin = (size_t)(occur - haystack);
		pos.end = pos.begin + strlen(needle);
	} else {
		pos.begin = 0;
		pos.end = 0;
	}

	return pos;
}
