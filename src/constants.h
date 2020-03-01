/**
 * constants.h
 * All our global constants.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "windowshelper.h"

// Return codes.
#define UKI_OK    1
#define UKI_ERROR 0
#define UKI_ERROR_NOMANIFEST        -11
#define UKI_ERROR_NOVARIABLES       -12
#define UKI_ERROR_NOARTICLE         -13
#define UKI_ERROR_NOTEMPLATE        -14
#define UKI_ERROR_NOMAINTEMPLATE    -15
#define UKI_ERROR_INDEX_NOT_FOUND   -16
#define UKI_ERROR_VARIABLE_NOTFOUND -17
#define UKI_ERROR_BODYVAR_NOTFOUND  -18
#define UKI_ERROR_PARSING_ARTICLE   -21
#define UKI_ERROR_PARSING_VARIABLES -22
#define UKI_ERROR_PARSING_TEMPLATE  -23
#define UKI_ERROR_READING_TEMPLATE  -24
#define UKI_ERROR_DIRLIST_NOTFOUND    -31
#define UKI_ERROR_DIRLIST_FILEUNKNOWN -32
#define UKI_ERROR_CONVERSION_AW -41
#define UKI_ERROR_CONVERSION_WA -42
#define UKI_ERROR_REGEX_ASSET_IMAGE -51

// Paths.
#define UKI_MANIFEST_PATH "/MANIFEST.uki"
#define UKI_VARIABLE_PATH "/VARIABLES.uki"
#define UKI_ARTICLE_ROOT  "/pages/"
#define UKI_TEMPLATE_ROOT "/templates/"
#define UKI_ASSETS_ROOT   "/assets/"
#define UKI_ARTICLE_EXT   "htm"
#define UKI_TEMPLATE_EXT  "htm"

// Variable keys.
#define UKI_VAR_MAIN_TEMPLATE "main_template"

// Misc.
#ifdef WINDOWS
#define UKI_MAX_PATH          MAX_PATH
#define UKI_MAX_TEMPLATE_NAME MAX_PATH
#else
#include <limits.h>

#define UKI_MAX_PATH          PATH_MAX
#define UKI_MAX_TEMPLATE_NAME NAME_MAX
#endif

#endif /* _CONSTANTS_H_ */
