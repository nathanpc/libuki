/**
 * article.c
 * An article parser and manager helper module.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#include "article.h"
#include "constants.h"
#include "fileutils.h"
#include <string.h>
#include <stdio.h>

// Global variables.
const char *wiki_root_path;

/**
 * Initializes an article container.
 *
 * @param container  Article container.
 * @param _wiki_root Wiki root path.
 */
void initialize_articles(uki_article_container *container,
						 const char *_wiki_root) {
	wiki_root_path = _wiki_root;

	container->size = 0;
	container->list = malloc(sizeof(uki_article_t));
}

/**
 * Populates the articles container.
 *
 * @param  container articles container.
 * @return           UKI_OK if the operation was successful.
 */
int populate_articles(uki_article_container *container) {
	char article_path[UKI_MAX_PATH];
	dirlist_t dirlist;
	int err;
	size_t i;

	// Build article path root.
	pathcat(2, article_path, wiki_root_path, UKI_ARTICLE_ROOT);

	// Go through the directory and sort the findings.
	dirlist.size = 0;
	if ((err = list_directory_files(&dirlist, article_path, true)) != UKI_OK)
		return err;
	sort_dirlist(&dirlist);

	// Push the files into the article container.
	for (i = 0; i < dirlist.size; i++) {
		uki_article_t article;
		char *reldir = dirlist.list[i];

		// Skip the article root directory.
		reldir += strlen(article_path);

		// Allocate memory for the name and path, and populate them.
		article.name = (char*)malloc(basename_noext(NULL, reldir) *
									 sizeof(char));
		article.path = (char*)malloc((strlen(reldir) + 1) * sizeof(char));
		strcpy(article.path, reldir);
		basename_noext(article.name, reldir);

		// Push article into the container.
		container->list = realloc(container->list, sizeof(uki_article_t) *
								  (container->size + 1));
		container->list[container->size++] = article;
	}

	// Clean up and return.
	free_dirlist(dirlist);
	return UKI_OK;
}

/**
 * Cleans up the mess we left behind.
 *
 * @param container Article container to be emptied.
 */
void free_articles(uki_article_container container) {
	size_t i;
	for (i = 0; i < container.size; i++) {
		free(container.list[i].path);
		free(container.list[i].name);
	}

	free(container.list);
	container.size = 0;
}
