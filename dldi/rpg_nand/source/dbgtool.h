#ifndef _DBG_TOOL_H_
#define _DBG_TOOL_H_

#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//#define DEBUG

#define DEBUG_START();	{
#define DEBUG_END();	}

#ifdef DEBUG
static inline int dbg_printf( const char* format, ... )
{
	va_list args;
	va_start( args, format );
	int ret = vprintf( format, args );
	va_end(args);
	return ret;
	return 0;
} 
#else
static inline void dbg_printf( const char * format, ...) {}
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
#define cwl();
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
