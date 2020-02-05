/**
 * template.h
 * A simple templating library to construct all that sweet content.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

#include <stdbool.h>

#define UKI_MAX_TEMPLATE_NAME 50

bool substitute_templates(char **template);

#endif /* _TEMPLATE_H_ */
