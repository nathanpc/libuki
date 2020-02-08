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
DLL_API int uki_initialize(const char *wiki_path);
DLL_API void uki_clean();

// Configuration and variables.
DLL_API uki_variable_t uki_config(const uint8_t index);
DLL_API uki_variable_t uki_variable(const uint8_t index);

// Rendering.
DLL_API int uki_render_page(char **rendered, const char *page);

#endif /* _UKI_H_ */
