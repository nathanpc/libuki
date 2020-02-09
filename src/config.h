/**
 * config.h
 * Helps parse the configuration files.
 *
 * @author: Nathan Campos <hi@nathancampos.me>
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "windowshelper.h"
#include <stdlib.h>
#ifndef WINDOWS
#include <stdint.h>
#include <stdbool.h>
#endif

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
//bool find_variable_i(const uint8_t index, const uki_variable_container container,
//					 uki_variable_t *var);
uki_variable_t find_variable_i(const uint8_t index, const uki_variable_container container);
int find_variable(const char *key, const uki_variable_container container);

#endif /* _CONFIG_H_ */
