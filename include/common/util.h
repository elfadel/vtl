
#ifndef __UTIL_H
#define __UTIL_H

#include <stdint.h>

/**
 * Allocate memory for string characters.
 * @param len allocation size
 * @return pointer on allocated memory or NULL on failure
 **/ 
char *
allocate_strmem (int len);

/**
 * Allocate memory for an array of unsigned chars.
 * @param len allocation size
 * @return pointer on allocated memory or NULL on failure
 **/ 
uint8_t *
allocate_ustrmem (int len);

/**
 * Allocate memory for an array of ints.
 * @param len allocation size
 * @return pointer on allocated memory or NULL on failure
 **/ 
int *
allocate_intmem (int len);

/**
 * Computing the internet checksum (RFC 1071).
 * 
 **/ 
uint16_t 
checksum (uint16_t *addr, int len);

void DumpHex(const void* data, size_t size); 


#endif /* __UTIL_H */