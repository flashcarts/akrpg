#ifndef _FATFILE_EX_H_
#define _FATFILE_EX_H_

#include <stdio.h>
#include "fatfile.h"

#ifdef __cplusplus
extern "C" {
#endif

int freadex( void * buffer, int _size, int _n, FILE * f );
int fwriteex( const void * buffer, int _size, int _n, FILE * f );

#ifdef __cplusplus
}
#endif


#endif//_FATFILE_EX_H_
