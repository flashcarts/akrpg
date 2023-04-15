/*
 * 2008/05/18
 * @by bliss (bliss@hanirc.org)
 *
 * Copyright (C) 2008 Bliss. All rights reserved.
 * This code following BSD LICENSE.
 *
 */



#ifndef _SAVESLOT_WND_H_
#define _SAVESLOT_WND_H_

#include "form.h"
#include "formdesc.h"
//#include "spinbox.h"
#include "statictext.h"
#include "message.h"
#include <string>

class cSaveSlotWnd : public akui::cForm
{
public:
    cSaveSlotWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    ~cSaveSlotWnd();

private:
	struct cRow {
		u8 index;
		bool isdefault;
		bool isexist;
	};


public:

    void draw();
	
    bool process( const akui::cMessage & msg );

    cWindow& loadAppearance(const std::string& aFileName );

    void setFileInfo( const std::string & fullPath, const u8 saveSlot);

protected:

	void initSlots();

    bool processKeyMessage( const akui::cKeyMessage & msg );

    bool processTouchMessage( const akui::cTouchMessage & msg );
	
	u32 rowBelowPoint( const akui::cPoint & pt );

	void drawSelectionBar();
	
    void onOK();

    void onCancel();
	
	void onSetDefault();

	void onCopy();

	void onDelete();

    void onShow();
	
	void insertRow( u8 index );

	bool isExist( const u8 index );
	bool isExist( const std::string &savePath);

    akui::cFormDesc _renderDesc;

	std::string _longName;
	
	std::vector<cRow> cRows;


	u8			_oldSlot;	
	u16			_rowHeight;
    u16			_textColor;
    u16			_textColorHilight;
    u16			_selectionBarColor1;
    u16			_selectionBarColor2;

    u32			_selectedRowId;
    bool		_touchMovedAfterTouchDown;

    akui::cButton _buttonOK;
    akui::cButton _buttonSetSlot;
    akui::cButton _buttonDelete;
    akui::cButton _buttonCopy;
};




#endif//_SAVESLOT_WND_H_
