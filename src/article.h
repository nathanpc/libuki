/**
 * article.h
 * An article parser and manager helper module.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _ARTICLE_H_
#define _ARTICLE_H_

#include "windowshelper.h"
#ifndef WINDOWS
#include <stdlib.h>
#include <stdint.h>
#endif

// Article structure.
typedef struct {
	char *path;
	char *name;
} uki_article_t;

// Article container.
typedef struct {
	size_t size;
	uki_article_t *list;
} uki_article_container;


void initialize_articles(uki_article_container *container,
						 const char *_wiki_root);
int populate_articles(uki_article_container *container);
void free_articles(uki_article_container container);

#endif /* _ARTICLE_H_ */
