/**
 * template.h
 * A simple templating library to construct all that sweet content.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

#include <stdbool.h>

void initialize_templating(const char *_wiki_root);
int substitute_templates(char **template);
int render_template(char **rendered, const char *template_name);

#endif /* _TEMPLATE_H_ */
