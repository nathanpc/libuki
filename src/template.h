/**
 * template.h
 * A simple templating library to construct all that sweet content.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

#include "windowshelper.h"
#include "config.h"

#ifdef UNIX
#include <stdbool.h>
#endif

// Template structure.
typedef struct {
	char *path;
	char *name;
	char *parent;
	int   deepness;
} uki_template_t;

// Template container.
typedef struct {
	size_t size;
	uki_template_t *list;
} uki_template_container;

// Memory management.
void initialize_templating(uki_template_container *container,
						   const char *_wiki_root);
uki_template_t add_template(uki_template_container *container,
							const char *fpath);
uki_error populate_templates(uki_template_container *container);
void free_templates(uki_template_container container);

// Lookup.
uki_template_t find_template_i(const size_t index,
							   const uki_template_container container);

// Rendering.
uki_error render_template(char **rendered, const char *template_name);
uki_error render_article_in_template(char **filled_template, const char *path);
uki_error render_variables(char **filled_template,
						   const uki_variable_container variables);

#endif /* _TEMPLATE_H_ */
