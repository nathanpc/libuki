/**
 * uki.h
 * A micro-wiki for personal stuff.
 *
 * @author: Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _UKI_H_
#define _UKI_H_

// Return codes.
#define UKI_OK    1
#define UKI_ERROR 0
#define UKI_ERROR_NOMANIFEST -1
#define UKI_ERROR_NOARTICLE  -2
#define UKI_ERROR_PARSING_ARTICLE -3

// Paths.
#define UKI_MANIFEST_PATH "/MANIFEST.uki"
#define UKI_ARTICLE_ROOT  "/pages/"
#define UKI_ARTICLE_EXT   "html"

// Misc.
#define UKI_MAX_PATH 512

const char* uki_error_msg(const int ecode);
void uki_clean();
int uki_initialize(const char *wiki_path);
int uki_render_page(char **rendered, const char *page);

#endif /* _UKI_H_ */
