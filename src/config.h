/**
 * config.h
 * Helps parse the configuration files.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// Variable limits.
#define VARIABLE_KEY_MAX_CHAR   50
#define VARIABLE_VALUE_MAX_CHAR 100

// Variable structure.
typedef struct {
	char *key;
	char *value;
} uki_variable_t;

// Variable container.
typedef struct {
	uint8_t size;
	uki_variable_t *list;
} uki_variable_container;


void initialize_variables(uki_variable_container *container);
bool populate_variables(uki_variable_container *container, const char *fname);
void free_variables(uki_variable_container container);

#endif /* _CONFIG_H_ */
