/**
 * uki.h
 * A micro-wiki for personal stuff.
 *
 * @author: Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _UKI_H_
#define _UKI_H_

#include "constants.h"
#include "config.h"

const char* uki_error_msg(const int ecode);
void uki_clean();
int uki_initialize(const char *wiki_path);
bool uki_config(const uint8_t index, uki_variable_t *var);
bool uki_variable(const uint8_t index, uki_variable_t *var);
int uki_render_page(char **rendered, const char *page);

#endif /* _UKI_H_ */
