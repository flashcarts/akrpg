#include <nds.h>
#include <string.h>

void dirFromFullPath( const char * path, char * dir )
{
    char * p = strrchr( path, '/' );
    memcpy( dir, path, p - path + 1 );
    dir[p-path+1]=0;
}

char * filenameFromFullPath( const char * path )
{
    char * p = strrchr( path, '/' );
    return p+1;
}


