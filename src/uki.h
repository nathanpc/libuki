/**
 * uki.h
 * A micro-wiki for personal stuff.
 *
 * @author: Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _UKI_H_
#define _UKI_H_

#include "windowshelper.h"
#include "constants.h"
#include "config.h"
#include "template.h"
#include "article.h"

// Define Windows DLL export and import macro.
#ifdef WINDOWS
#ifdef UKI_DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#else
#define DLL_API extern
#endif

// Error handling.
DLL_API const char* uki_error_msg(const int ecode);

// Initialization and destruction.
DLL_API uki_error uki_initialize(const char *wiki_path);
DLL_API void uki_clean();

// Lookup.
DLL_API size_t uki_configs_available();
DLL_API size_t uki_variables_available();
DLL_API size_t uki_articles_available();
DLL_API size_t uki_templates_available();
DLL_API uki_variable_t uki_config(const uint8_t index);
DLL_API uki_variable_t uki_variable(const uint8_t index);
DLL_API uki_article_t uki_article(const size_t index);
DLL_API uki_template_t uki_template(const size_t index);

// Asset management.
DLL_API uki_article_t uki_add_article(const char *article_path);
DLL_API uki_template_t uki_add_template(const char *template_path);

// Paths.
DLL_API uki_error uki_article_fpath(char *fpath, const uki_article_t article);
DLL_API uki_error uki_template_fpath(char *fpath, const uki_template_t template);
DLL_API uki_error uki_folder_articles(char *fpath);
DLL_API uki_error uki_folder_templates(char *fpath);

// Rendering.
DLL_API uki_error uki_render_template_from_text(char **content, const int deepness);
DLL_API uki_error uki_render_article_from_text(char **content, const int deepness);
DLL_API uki_error uki_render_article(char **rendered, const size_t index,
									 const bool preview);
DLL_API uki_error uki_render_template(char **rendered, const size_t index,
									  const bool preview);
DLL_API uki_error uki_render_page(char **rendered, const char *page);

#endif /* _UKI_H_ */
