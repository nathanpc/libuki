/**
 * article.h
 * An article parser and manager helper module.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _ARTICLE_H_
#define _ARTICLE_H_

#include "windowshelper.h"
#include "constants.h"
#ifdef UNIX
#include <stdlib.h>
#include <stdint.h>
#endif

// Article structure.
typedef struct {
	char *path;
	char *name;
	char *parent;
	int   deepness;
} uki_article_t;

// Article container.
typedef struct {
	size_t size;
	uki_article_t *list;
} uki_article_container;

// Memory management.
void initialize_articles(uki_article_container *container,
						 const char *_wiki_root);
uki_article_t add_article(uki_article_container *container, const char *fpath);
uki_error populate_articles(uki_article_container *container);
void free_articles(uki_article_container container);

// Lookup.
uki_article_t find_article_i(const size_t index,
							 const uki_article_container container);

#endif /* _ARTICLE_H_ */
