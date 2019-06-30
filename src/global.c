/**
 * \file global.c
 * \brief This file provides functions, and structures that can be used by any file of this project
 * \author Brice CROIX
 * \date 17.06.2019
 */

#include "global.h"

/**
 * \fn void* safe_malloc(size_t size)
 * \author Brice Croix
 * \brief This function is just a malloc with a featured security
 * \param size is the malloc parameter
 * \return same as malloc()
 */
void* safe_malloc(size_t __size){
    void* out;
    out = malloc(__size);
    if(out == NULL){
        fprintf(stderr,"Fatal : failed to allocate %ld bytes\n",__size);
        exit(EXIT_FAILURE);
    }
    return out;
}

/**
 * \fn void* safe_realloc(void* ptr, size_t size)
 * \author Brice Croix
 * \brief This function is just a realloc with a featured security
 * \param ptr is the first realloc parameter
 * \param size is the second realloc parameter
 * \return same as realloc()
 */
void* safe_realloc(void* ptr, size_t __size){
    void* ptr2;
    ptr2 = ptr;

    ptr = realloc(ptr,__size);
    if(ptr == NULL){
        fprintf(stderr,"Fatal : failed to allocate %ld bytes\n",__size);
        free(ptr2);
        exit(EXIT_FAILURE);
    }
    return ptr;

}

/**
 * \fn void* safe_calloc(size_t __nmenb, size_t __size)
 * \author Brice Croix
 * \brief This function is just a calloc with a featured security
 * \param ptr is the first calloc parameter
 * \param size is the second calloc parameter
 * \return same as calloc()
 */
void* safe_calloc(size_t __nmenb, size_t __size){
    void* out;
    out = calloc(__nmenb, __size);
    if(out == NULL){
        fprintf(stderr,"Fatal : failed to allocate %ld bytes\n",__size);
        exit(EXIT_FAILURE);
    }
    return out;

}