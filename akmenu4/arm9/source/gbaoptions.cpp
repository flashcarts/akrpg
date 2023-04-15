/*
 * 2008/03/04
 * @by bliss
 *
 * Copyright (C) 2008 Bliss. All rights reserved.
 * This code following BSD LICENSE.
 *
 */

#include "gbaoptions.h"
#include "systemfilenames.h"
#include "language.h"
#include "fatfile_ex.h"
#include "dsrom.h"
#include "inifile.h"
#include <string.h>

cGbaOptionManager::cGbaOptionManager()
{
}

cGbaOptionManager::~cGbaOptionManager()
{
}

bool cGbaOptionManager::importOptionList()
{

	struct stat st;
    if( (-1 == stat( SFN_GBAOPTION_LIST , &st )) ) {
		dbg_printf("file is not found: %s\n",SFN_GBAOPTION_LIST);
		return false;
	}

	size_t filesize = st.st_size;
	if ( filesize  == 0 || filesize % sizeof(GBAOPTION) != 0) {
        dbg_printf("%s size error %d\n", SFN_GBAOPTION_LIST, filesize);
		return false;
	}


	FILE *f = fopen(SFN_GBAOPTION_LIST, "rb");
	if ( !f ) 
	{
        dbg_printf("fopne error: %s \n", SFN_GBAOPTION_LIST);
		return false;
	}
    u32 saveCount = filesize / sizeof(GBAOPTION);
	_optionList.resize(saveCount);
	u32 readed = fread( &_optionList[0], 1, filesize, f );
    if( filesize != readed )
    {
        dbg_printf("%s read length error %d\n", SFN_GBAOPTION_LIST, readed );
        fclose( f );
        f = NULL;
        return false;
    }

    fclose( f );
    return true;
}

bool cGbaOptionManager::exportOptionList()
{
    FILE * f = fopen( SFN_GBAOPTION_LIST , "wb" );
    if( NULL == f )
    {
        dbg_printf("fopen %s fail\n", SFN_GBAOPTION_LIST );
        return false;
    }
    u32 filesize = _optionList.size() * sizeof(GBAOPTION);

    u32 written = fwrite( &_optionList[0], 1, filesize, f );
    if( filesize != written )
    {
        dbg_printf("%s write length error %d\n", SFN_GBAOPTION_LIST, written );
        fclose( f );
        f = NULL;
        return false;
    }
    fclose( f );
    f = NULL;

    return true;

}

void cGbaOptionManager::updateOptionList( const GBAOPTION & aOption )
{
    size_t i = 0;
    for( i = 0; i < _optionList.size(); ++i )
    {
        if( 0 == memcmp( &_optionList[i], &aOption , sizeof(GBAOPTION) - 8 ) ) {
			memcpy(&_optionList[i] ,&aOption ,sizeof(aOption));
            break;
        }
    }
    if( i == _optionList.size() )
        _optionList.push_back( aOption );

    exportOptionList();

}


void cGbaOptionManager::getRomOption( GBAOPTION & gameOption )
{
    size_t saveCount = _optionList.size();
    for( size_t i = 0; i < saveCount; ++i )
    {
        if( 0 == memcmp( &gameOption , &_optionList[i], sizeof(GBAOPTION) - 8 ) )
        {
			memcpy(&gameOption ,&_optionList[i],sizeof(gameOption));
        }
    }
}

