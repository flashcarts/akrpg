/*
 * 2008/03/10
 * @by bliss
 *
 * Copyright (C) 2008 Bliss. All rights reserved.
 * This code following BSD LICENSE.
 *
 */

#ifndef __FILEICONS_H__
#define __FILEICONS_H__

#include <nds.h>
#include <vector>
#include <string>
#include "bmp15.h"
#include "singleton.h"

typedef struct _FILE_ICONS_T
{
	char extname[6];
	cBMP15 image;
} FILE_ICONS;

class cFileIcons
{
	public:

		cFileIcons();

		~cFileIcons();

	public:
		void loadIcons(void);
		bool isExist(const char *extName);
		int getIndex(void);
		const char *getExtName(int idx);
		void drawIcon(int idx, u8 x, u8 y, GRAPHICS_ENGINE engine);
		void drawIconMem(int idx, void *mem);

	protected:
		void _load(const char *directory );
		void _update(const char *extName, std::string &fullPath);
		void maskBlt( const void * src, u16 destW, u16 destH, void *mem);

		std::vector< FILE_ICONS > _file_icons;

		int _index;

};

typedef t_singleton< cFileIcons > FileIcons_s;
inline cFileIcons & fileicons() { return FileIcons_s::instance(); }


#endif
