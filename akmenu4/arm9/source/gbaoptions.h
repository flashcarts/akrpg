/*
 * 2008/03/04
 * @by bliss
 *
 * Copyright (C) 2008 Bliss. All rights reserved.
 * This code following BSD LICENSE.
 *
 */

#ifndef __GBAOPTION_H__
#define __OBAOPTION_H__

#include <nds.h>
#include <vector>
#include <string>
#include "singleton.h"

#define OPT_GLOBAL	0
#define OPT_DISABLE	1
#define OPT_ENABLE1	2
#define OPT_ENABLE2	3

typedef struct _GBAOPTION_T
{
	// identifier
	char gameTitle[0xC];
	char gameCode[4];
	// options
	u8 gbaMode;
	u8 saveSlots;
	u8 patchSleep;
	u8 patchSram;
	u8 norMode;
	u8 reserved_1;
	u8 reserved_2;
	u8 reserved_3;

} GBAOPTION;

class cGbaOptionManager
{
	public:

		cGbaOptionManager();

		~cGbaOptionManager();

	public:

		bool importOptionList();

		bool exportOptionList();

		void updateOptionList(const GBAOPTION & aOption);

		void getRomOption( GBAOPTION & gameOption);

	protected:

		std::vector< GBAOPTION > _optionList;
};

typedef t_singleton< cGbaOptionManager > GbaOptionManager_s;
inline cGbaOptionManager & gbaOptionManager() { return GbaOptionManager_s::instance(); }

#endif
