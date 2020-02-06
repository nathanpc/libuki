/**
 * uki
 * A command-line implementation of the uki personal wiki engine.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include <stdio.h>
#include <stdlib.h>
#include "uki.h"

// Private methods.
void usage();


/**
 * Application's main entry point.
 *
 * @param  argc Number of command-line arguments supplied.
 * @param  argv Array of command-line arguments.
 * @return      Return code.
 */
int main(const int argc, const char **argv) {
	int uki_error;

	// Check command-line arguments.
	if (argc == 2) {
		usage();
		return 1;
	}

	// Initialize the uki wiki.
	if ((uki_error = uki_initialize(argv[1])) != UKI_OK) {
		fprintf(stderr, uki_error_msg(uki_error));
		uki_clean();

		return 1;
	}

	// Print configurations.
	uki_variable_t *var;
	uint8_t iv = 0;
	printf("Configurations:\n");
	while (uki_config(iv, var)) {
		printf("   %s <- %s\n", var->key, var->value);
		iv++;
	}
	printf("\n");

	// Print variables.
	printf("Variables:\n");
	iv = 0;
	while (uki_variable(iv, var)) {
		printf("   %s <- %s\n", var->key, var->value);
		iv++;
	}
	printf("\n");

	// Render a page.
	char *content;
	if ((uki_error = uki_render_page(&content, argv[2])) != UKI_OK) {
		fprintf(stderr, uki_error_msg(uki_error));
		uki_clean();

		return 1;
	}

	// Print the page content.
	printf("Content:\n");
	printf("%s\n", content);

	// Clean up and return.
	uki_clean();
	return 0;
}

/**
 * Prints the program's usage.
 */
void usage() {
	printf("Usage: uki <wiki> <article>\n\n");
	printf("Arguments:\n");
	printf("    wiki\tPath to a wiki\n");
	printf("    article\tPath to a wiki article\n");
}
