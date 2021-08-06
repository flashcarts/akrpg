#include "fatfile.h"

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>

#include "cache.h"
#include "file_allocation_table.h"
#include "bit_ops.h"
#include "filetime.h"
#include "fatfile.h"
#include "dbgtool.h"
#include "rpgprotocol.h"

int fwritex( const void * buffer, int _size, int _n, FILE * f )
{
	if( 0 == _n )
		return 0;
	dbg_printf("fwritex %d\n", _n );
	struct _reent r;
	int ret = _FAT_write_r( &r, (int)f->_file + 8, buffer, _size * _n );
	errno = r._errno;
	return ret;
}
