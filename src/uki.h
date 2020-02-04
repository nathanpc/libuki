/**
 * uki.h
 * A micro-wiki for personal stuff.
 *
 * @author: Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _UKI_H_
#define _UKI_H_

#include "config.h"

// Return codes.
#define UKI_OK    1
#define UKI_ERROR 0
#define UKI_ERROR_NOMANIFEST  -11
#define UKI_ERROR_NOVARIABLES -12
#define UKI_ERROR_NOARTICLE   -13
#define UKI_ERROR_PARSING_ARTICLE   -21
#define UKI_ERROR_PARSING_VARIABLES -22

// Paths.
#define UKI_MANIFEST_PATH "/MANIFEST.uki"
#define UKI_VARIABLE_PATH "/VARIABLES.uki"
#define UKI_ARTICLE_ROOT  "/pages/"
#define UKI_TEMPLATE_ROOT "/templates/"
#define UKI_ARTICLE_EXT   "html"
#define UKI_TEMPLATE_EXT  "html"

// Misc.
#define UKI_MAX_PATH 512

const char* uki_error_msg(const int ecode);
void uki_clean();
int uki_initialize(const char *wiki_path);
bool uki_variable(const uint8_t index, uki_variable_t *var);
int uki_render_page(char **rendered, const char *page);

#endif /* _UKI_H_ */
