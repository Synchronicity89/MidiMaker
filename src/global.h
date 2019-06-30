/**
 * \file global.h
 * \brief Header file associated to 'global.c'.This file provides functions, and structures that can be used by any file of this project.
 * \author Brice CROIX
 * \date 17.06.2019
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdlib.h>
#include <stdio.h>

void* safe_malloc(size_t __size);

void* safe_realloc(void* ptr, size_t __size);

void* safe_calloc(size_t __nmenb, size_t __size);

#endif //GLOBAL_H