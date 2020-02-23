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
char article_path[UKI_MAX_PATH];

// Private methods.
void push_article(uki_article_container *container, uki_article_t article);
void populate_article_from_path(uki_article_t *article, const char *fpath);

/**
 * Initializes an article container.
 *
 * @param container  Article container.
 * @param _wiki_root Wiki root path.
 */
void initialize_articles(uki_article_container *container,
						 const char *_wiki_root) {
	// Initialize paths.
	wiki_root_path = _wiki_root;
	pathcat(2, article_path, wiki_root_path, UKI_ARTICLE_ROOT);

	container->size = 0;
	container->list = malloc(sizeof(uki_article_t));
}

/**
 * Gets an article by its index.
 *
 * @param  index     Article index.
 * @param  container Article container to search into.
 * @return           The article structure if it was found. NULL otherwise.
 */
uki_article_t find_article_i(const size_t index,
							 const uki_article_container container) {
	// Check if the index is out of bounds.
	if (index >= container.size) {
		uki_article_t nl;
		nl.name = NULL;
		nl.path = NULL;
		nl.parent = NULL;
		nl.deepness = 0;

		return nl;
	}

	return container.list[index];
}

/**
 * Pushes an article into the container.
 *
 * @param container Article container.
 * @param article   Article structure to be added.
 */
void push_article(uki_article_container *container, uki_article_t article) {
	container->list = realloc(container->list, sizeof(uki_article_t) *
							  (container->size + 1));
	container->list[container->size++] = article;
}

/**
 * Populates an article structure using a file path.
 * @remark This function ignores the article root path, but it must be present.
 *
 * @param article Article structure to be populated.
 * @param fpath   Complete file path. Article root part will be ignored.
 */
void populate_article_from_path(uki_article_t *article, const char *fpath) {
	const char *reldir = fpath;

	// Skip the article root directory.
	reldir += strlen(article_path);

	// Allocate memory and populate the structure.
	article->name = (char*)malloc(basename_noext(NULL, reldir) *
								  sizeof(char));
	article->path = (char*)malloc((strlen(reldir) + 1) * sizeof(char));
	basename_noext(article->name, reldir);
	strcpy(article->path, reldir);
	article->deepness = path_deepness(article->path);

	// Populate the parent path.
	if (article->deepness > 0) {
		article->parent = (char*)malloc(parent_dir_name(NULL, reldir) *
										sizeof(char));
		parent_dir_name(article->parent, reldir);
	} else {
		article->parent = NULL;
	}
}

/**
 * Adds an article to the container by its path.
 *
 * @param  container Article container.
 * @param  fpath     Complete path to the article.
 * @return           The recently added article.
 */
uki_article_t add_article(uki_article_container *container, const char *fpath) {
	uki_article_t article;

	// Populate article and push it into the container.
	populate_article_from_path(&article, fpath);
	push_article(container, article);

	return article;
}

/**
 * Populates the articles container.
 *
 * @param  container Articles container.
 * @return           UKI_OK if the operation was successful.
 */
int populate_articles(uki_article_container *container) {
	dirlist_t dirlist;
	int err;
	size_t i;

	// Go through the directory and sort the findings.
	dirlist.size = 0;
	if ((err = list_directory_files(&dirlist, article_path, true)) != UKI_OK)
		return err;
	sort_dirlist(&dirlist);

	// Push the files into the article container.
	for (i = 0; i < dirlist.size; i++) {
		if (file_ext_match(dirlist.list[i], UKI_ARTICLE_EXT))
			add_article(container, dirlist.list[i]);
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
		free(container.list[i].parent);
	}

	free(container.list);
	container.size = 0;
}
