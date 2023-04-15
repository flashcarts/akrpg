/*
 * 2008/03/04
 * @by bliss
 *
 * Copyright (C) 2008 Bliss. All rights reserved.
 * This code following BSD LICENSE.
 *
 */

#ifndef __OPTION_H__
#define __OPTION_H__

#include <nds.h>
#include <vector>
#include <string>
#include "singleton.h"

#define OPT_GLOBAL	0
#define OPT_DISABLE	1
#define OPT_ENABLE1	2
#define OPT_ENABLE2	3

typedef struct _OPTION_T
{
	u8 gameTitle[12];
	u8 gameCode[4];
	PACKED u16 gameCRC;
	// options
	u8 downloadPlayPatch;
	u8 resetInGame;
	u8 cheatingSystem;
	u8 saveSlots;
	u8 rumbleStrength;
	u8 reserved_1;
	u8 reserved_2;
	u8 reserved_3;

} OPTION;

class cOptionManager
{
	public:

		cOptionManager();

		~cOptionManager();

	public:

		bool importOptionList();

		bool exportOptionList();

		void updateOptionList(const OPTION & aOption);

		void getRomOption( OPTION & gameOption);

	protected:

		std::vector< OPTION > _optionList;
};

typedef t_singleton< cOptionManager > OptionManager_s;
inline cOptionManager & optionManager() { return OptionManager_s::instance(); }

#endif
