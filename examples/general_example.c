/**
 * general_example.c
 * A simple example to test all the things involved in a Uki personal library
 * all at once.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uki.h>

// Private methods.
void usage();
void print_variables();
void print_templates_list();
void print_articles_list();
void print_articles_tree();
void print_page(const char *page_path);
void print_article(const size_t index);

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
	if (argc != 3) {
		usage();
		return 1;
	}

	// Initialize the uki wiki.
	if ((uki_error = uki_initialize(argv[1])) != UKI_OK) {
		fprintf(stderr, uki_error_msg(uki_error));
		uki_clean();

		return 1;
	}

	// Print our current state.
	print_variables();
	print_templates_list();
	print_articles_list();

	// Print trees.
	print_articles_tree();

	// Print a fully rendered page.
	print_page(argv[2]);

	// Print an article preview.
	print_article(4);

	// Clean up and return.
	uki_clean();
	return 0;
}

/**
 * Prints the configurations.
 */
void print_variables() {
	// Print configurations.
	printf("Configurations:\n");
	for (size_t i = 0; i < uki_configs_available(); i++) {
		uki_variable_t var = uki_config(i);
		printf("   %s <- %s\n", var.key, var.value);
	}
	printf("\n");

	// Print variables.
	printf("Variables:\n");
	for (size_t i = 0; i < uki_variables_available(); i++) {
		uki_variable_t var = uki_variable(i);
		printf("   %s <- %s\n", var.key, var.value);
	}
	printf("\n");
}

/**
 * Prints an articles tree.
 */
void print_articles_tree() {
	char lastparent[UKI_MAX_PATH];
	lastparent[0] = '\0';

	// TODO: Make this able to traverse directories of any deepness.

	printf("Directory Tree:\n");
	for (size_t i = 0; i < uki_articles_available(); i++) {
		uki_article_t article = uki_article(i);

		if (article.parent != NULL) {
			if (strcmp(lastparent, article.parent) != 0) {
				strcpy(lastparent, article.parent);
				printf("   %s\n", lastparent);
			}
		} else {
			if (lastparent[0] != '\0') {
				lastparent[0] = '\0';
			}
		}

		if (article.deepness > 0) {
			printf("      ");
		} else {
			printf("   ");
		}

		printf("%s\n", article.name);
	}

	printf("\n");
}

/**
 * Prints a list of the templates.
 */
void print_templates_list() {
	printf("Templates:\n");

	for (size_t i = 0; i < uki_templates_available(); i++) {
		uki_template_t template = uki_template(i);
		char fpath[UKI_MAX_PATH];

		printf("   %d %s  |  %s <- %s\n", template.deepness, template.parent,
			   template.path, template.name);

		uki_template_fpath(fpath, template);
		printf("      +--> %s\n", fpath);
	}

	printf("\n");
}

/**
 * Prints a list of the articles.
 */
void print_articles_list() {
	printf("Articles:\n");

	for (size_t i = 0; i < uki_articles_available(); i++) {
		uki_article_t article = uki_article(i);
		char fpath[UKI_MAX_PATH];

		printf("   %d %s  |  %s <- %s\n", article.deepness, article.parent,
			   article.path, article.name);

		uki_article_fpath(fpath, article);
		printf("      --> %s\n", fpath);
	}

	printf("\n");
}

/**
 * Prints a fully rendered page.
 *
 * @param page_path Relative page to the article path.
 */
void print_page(const char *page_path) {
	char *content;
	int err;

	printf("Page Content:\n");
	if ((err = uki_render_page(&content, page_path)) != UKI_OK) {
		fprintf(stderr, "ERROR: %s", uki_error_msg(err));
		uki_clean();
		exit(EXIT_FAILURE);
	}

	// Print the page content and free it.
	printf("%s\n", content);
	free(content);
}

/**
 * Prints a rendered article preview.
 *
 * @param index Article index.
 */
void print_article(const size_t index) {
	char *content;
	int err;

	printf("Article Preview:\n");
	if ((err = uki_render_article(&content, index, true)) != UKI_OK) {
		fprintf(stderr, uki_error_msg(err));
		uki_clean();
		exit(EXIT_FAILURE);
	}

	printf("%s\n", content);
	free(content);
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
