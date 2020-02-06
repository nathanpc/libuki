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

// Error handling.
extern const char* uki_error_msg(const int ecode);

// Initialization and destruction.
extern int uki_initialize(const char *wiki_path);
extern void uki_clean();

// Configuration and variables.
extern bool uki_config(const uint8_t index, uki_variable_t *var);
extern bool uki_variable(const uint8_t index, uki_variable_t *var);

// Rendering.
extern int uki_render_page(char **rendered, const char *page);

#endif /* _UKI_H_ */
