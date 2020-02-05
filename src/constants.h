/**
 * constants.h
 * All our global constants.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

// Return codes.
#define UKI_OK    1
#define UKI_ERROR 0
#define UKI_ERROR_NOMANIFEST  -11
#define UKI_ERROR_NOVARIABLES -12
#define UKI_ERROR_NOARTICLE   -13
#define UKI_ERROR_NOTEMPLATE  -14
#define UKI_ERROR_PARSING_ARTICLE   -21
#define UKI_ERROR_PARSING_VARIABLES -22
#define UKI_ERROR_PARSING_TEMPLATE  -23
#define UKI_ERROR_READING_TEMPLATE  -24

// Paths.
#define UKI_MANIFEST_PATH "/MANIFEST.uki"
#define UKI_VARIABLE_PATH "/VARIABLES.uki"
#define UKI_ARTICLE_ROOT  "/pages/"
#define UKI_TEMPLATE_ROOT "/templates/"
#define UKI_ARTICLE_EXT   "html"
#define UKI_TEMPLATE_EXT  "html"

// Misc.
#define UKI_MAX_PATH 512
#define UKI_MAX_TEMPLATE_NAME 50

#endif /* _CONSTANTS_H_ */
