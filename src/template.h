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

void initialize_templating(const char *_wiki_root);
int render_template(char **rendered, const char *template_name);
int render_article(char **filled_template, const char *path);
int render_variables(char **filled_template,
					const uki_variable_container variables);

#endif /* _TEMPLATE_H_ */
