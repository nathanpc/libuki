/**
 * config.c
 * Helps parse the configuration files.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#include "config.h"
#include <string.h>
#include <stdio.h>

#define VAR_LINE_MATCH "%49[^=]=%99[0-9a-zA-Z '\".,;:!@#$%%^&*()_-+=][^\r\n]"

// Private methods.
bool parse_variable_line(const char *line, uki_variable_t *var);

/**
 * Initializes a variable container.
 *
 * @param container Variable container.
 */
void initialize_variables(uki_variable_container *container) {
	container->size = 0;
	container->list = malloc(sizeof(uki_variable_t));
}

/**
 * Populates the variables container.
 *
 * @param  container Variable container.
 * @param  fname     Variables file path.
 * @return           TRUE if the parsing was successful.
 */
bool populate_variables(uki_variable_container *container, const char *fname) {
	FILE *fh;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	// Open file for reading.
	fh = fopen(fname, "r");
	if (fh == NULL) {
		return false;
	}

	// Go through the file line by line.
	while ((read = getline(&line, &len, fh)) != -1) {
		uki_variable_t var;

		// Check if line is empty.
		if ((line[0] == '\n') || (line[0] == '\r')) {
			break;
		}

		// Parse variable line.
		if (!parse_variable_line(line, &var)) {
			return false;
		}

		// Push variable into container.
		container->list = realloc(container->list, sizeof(uki_variable_t) *
								  (container->size + 1));
		container->list[container->size++] = var;
	}

	// Clean up.
	fclose(fh);
	free(line);

	return true;
}

/**
 * Cleans up the mess we left behind.
 *
 * @param container Variable container to be emptied.
 */
void free_variables(uki_variable_container container) {
	for (uint8_t i = 0; i < container.size; i++) {
		free(container.list[i].key);
		free(container.list[i].value);
	}

	free(container.list);
	container.size = 0;
}

/**
 * Gets a variable by its index.
 *
 * @param  index     Variable index.
 * @param  container Variable container to search into.
 * @param  var       The variable structure if it was found. NULL otherwise.
 * @return           TRUE if the variable was found.
 */
bool find_variable_i(const uint8_t index, const uki_variable_container container,
					 uki_variable_t *var) {
	// Check if the index is out of bounds.
	if (index >= container.size) {
		var = NULL;
		return false;
	}

	*var = container.list[index];
	return true;
}

/**
 * Gets a variable index by its key.
 *
 * @param  key       Variable key.
 * @param  container Variable container to search into.
 * @return           Variable index in case it was found. A negative number
 *                   otherwise.
 */
int find_variable(const char *key, const uki_variable_container container) {
	for (uint8_t i = 0; i < container.size; i++) {
		if (strcmp(key, container.list[i].key) == 0) {
			return i;
		}
	}

	return -1;
}

/**
 * Parses a line and extracts a key and value for a variable.
 *
 * @param  line Line string to be parsed.
 * @param  var  Variable structure to be populated.
 * @return      TRUE if the parsing went well.
 */
bool parse_variable_line(const char *line, uki_variable_t *var) {
	char key[VARIABLE_KEY_MAX_CHAR];
	char value[VARIABLE_VALUE_MAX_CHAR];

	// Parse key and value.
	int nmatch = sscanf(line, VAR_LINE_MATCH, key, value);
	if (nmatch != 2) {
		return false;
	}

	// Allocate space for everyone.
	var->key = (char*)malloc((strlen(key) + 1) * sizeof(char));
	var->value = (char*)malloc((strlen(value) + 1) * sizeof(char));

	// Populate the variable structure.
	strcpy(var->key, key);
	strcpy(var->value, value);

	return true;
}
