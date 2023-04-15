/*
 * 2008/05/18
 * @by bliss (bliss@hanirc.org)
 *
 * Copyright (C) 2008 Bliss. All rights reserved.
 * This code following BSD LICENSE.
 *
 */




#ifndef _ROMSETTING_WND_H_
#define _ROMSETTING_WND_H_

#include "form.h"
#include "formdesc.h"
#include "spinbox.h"
#include "statictext.h"
#include "message.h"
#include <string>
#include "settingwnd.h"

class cRomSettingWnd : public cSettingWnd
{

public:
    cRomSettingWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    ~cRomSettingWnd();

public:
	bool setFileInfo( const std::string & longPath, const char* romid,const u8 saveSlot );

    bool process( const akui::cMessage & msg );

	void disableCheatMenu() ;

	u8 getSaveSlot() { return _saveSlot; }

protected:
	void onCheatMenu();
	void onSaveSlot();
    bool processKeyMessage( const akui::cKeyMessage & msg );

    akui::cButton _buttonSaveSlot;
    akui::cButton _buttonCheatMenu;

	std::string _romId;
	std::string _longName;

	bool _useCheatMenu;
	u8 _saveSlot;
};

#endif//_ROMSETTING_WND_H_
