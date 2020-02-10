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
#define UKI_ERROR_NOMANIFEST     -11
#define UKI_ERROR_NOVARIABLES    -12
#define UKI_ERROR_NOARTICLE      -13
#define UKI_ERROR_NOTEMPLATE     -14
#define UKI_ERROR_NOMAINTEMPLATE -15
#define UKI_ERROR_VARIABLE_NOTFOUND -16
#define UKI_ERROR_BODYVAR_NOTFOUND  -17
#define UKI_ERROR_PARSING_ARTICLE   -21
#define UKI_ERROR_PARSING_VARIABLES -22
#define UKI_ERROR_PARSING_TEMPLATE  -23
#define UKI_ERROR_READING_TEMPLATE  -24
#define UKI_ERROR_DIRLIST_NOTFOUND    -31
#define UKI_ERROR_DIRLIST_FILEUNKNOWN -32

// Paths.
#ifdef WINDOWS
#define UKI_MANIFEST_PATH "\\MANIFEST.uki"
#define UKI_VARIABLE_PATH "\\VARIABLES.uki"
#define UKI_ARTICLE_ROOT  "\\pages\\"
#define UKI_TEMPLATE_ROOT "\\templates\\"
#else
#define UKI_MANIFEST_PATH "/MANIFEST.uki"
#define UKI_VARIABLE_PATH "/VARIABLES.uki"
#define UKI_ARTICLE_ROOT  "/pages/"
#define UKI_TEMPLATE_ROOT "/templates/"
#endif
#define UKI_ARTICLE_EXT   "html"
#define UKI_TEMPLATE_EXT  "html"

// Variable keys.
#define UKI_VAR_MAIN_TEMPLATE "main_template"

// Misc.
#define UKI_MAX_PATH 512
#define UKI_MAX_TEMPLATE_NAME 50

#endif /* _CONSTANTS_H_ */
