/*---------------------------------------------------------------------------------


Copyright (C) 2007 Acekard, www.acekard.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


---------------------------------------------------------------------------------*/









#ifndef _DBG_TOOL_H_
#define _DBG_TOOL_H_

#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//#define DEBUG

#define DEBUG_START();    {
#define DEBUG_END();    }

#ifdef DEBUG
static inline int dbg_printf( const char* format, ... )
{
    va_list args;
    va_start( args, format );
    int ret = vprintf( format, args );
    va_end(args);
    return ret;
}
#else
#define dbg_printf(...)
#endif//DEBUG


#ifdef DEBUG
static inline void _cwl(char *file,int line)
{
  char *seek=file;

  while(*seek!=0){
    if(*seek=='/') file=seek;
    seek++;
  }
  dbg_printf("%s(%d)\n",file,line);
}
#define cwl(); _cwl( __FILE__, __LINE__ );
#else
#define cwl()
#endif//DEBUG

#ifdef DEBUG
static inline void wait_press_b()
{
    dbg_printf("\npress B to continue.\n");
    scanKeys();
    u16 keys_up = 0;
    while( 0 == (keys_up & KEY_B) )
    {
        scanKeys();
        keys_up = keysUp();
    }
}
#else
#define wait_press_b()
#endif//DEBUG


#endif//_DBG_TOOL_H_
