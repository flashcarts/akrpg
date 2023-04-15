/*---------------------------------------------------------------------------------


Copyright (C) 2008 Normmatt, www.normmatt.com

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



/*---------------------------------------------------------------------------------
 
@ modified and added by bliss (bliss@hanirc.org)

	Parsing of DAT/encrtyped DAT
	Imporved UI - 
		CheatDB Selection, Rebuild Cheat File, Some of Buttons.
		Folder Expand and Imporved navigation.
		Screen touch navigation.

---------------------------------------------------------------------------------*/









#ifndef _CHEATLIST_H_
#define _CHEATLIST_H_

#include "form.h"
#include "formdesc.h"
#include "spinbox.h"
#include "statictext.h"
#include "message.h"
#include <string>
#include "dsrom.h"

class cCheatListWnd : public akui::cForm
{
public:
    cCheatListWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    ~cCheatListWnd();

private:
	struct DAT_INDEX {
		u32 gamecode;
		u32 crc32;
		unsigned long long offset;
	} ;

	struct cRow {
		u8 status;
		u16 oNote;
		u16 oChild;

		u32 oFile;
		std::string text;
	};


public:

    void draw();
	
	void ReadList();

    bool process( const akui::cMessage & msg );

    cWindow& loadAppearance(const std::string& aFileName );

    bool setFileInfo( const std::string & shortPath,  const char* romid);

	bool BuildCheatFile(const char* cheatfile, const char* romid, const std::string & outputfile);

	
protected:

    void setDiskInfo( const std::string & diskName );

    void showSaveType();
	
	void setShortcut();

    bool processKeyMessage( const akui::cKeyMessage & msg );
    bool processTouchMessage( const akui::cTouchMessage & msg );
	
	u32 rowBelowPoint( const akui::cPoint & pt );

	void drawSelectionBar();
	
	void onToggle();

	void onKeyYPressed();

    void onOK();
	
	void onCancel();

    void onShow();
	
	void onSaveExit();

	void onRebuildCheat();

	void onShowDBSelectWnd();

	void insertRow( const char *text, u8 status, u32 oFile );
	void nextFile(u8 sc);
	void prevFile(u8 sc);

	u8 selectNext(u8 sc);
	u8 selectPrev(u8 sc);
	void selectRow();


	void scrollTo( int id );
	
	void selectRow( int id );
	
	void drawIcon( cBMP15 background, int x, int y, GRAPHICS_ENGINE engine );

	bool _parseXML(FILE *XML, size_t xmlSize, FILE *CCfile, const char *romid, u32 crc32) ;
	bool _parseDAT(FILE *DAT, size_t datSize, FILE *CCfile, const char *romid, u32 crc32);
	//int nextString(char *, int); // kzat3

    akui::cFormDesc _renderDesc;

	std::string _shortName;
	
	std::string _cheatName;

	std::vector<cRow> cRows;

	//Selection
	
	int SelectedID;
	
	u16			_rowHeight;
    u16			_textColor;
    u16			_textColorHilight;
    u16			_selectionBarColor1;
    u16			_selectionBarColor2;
	u16			_verticalLineColor;
	u16			_textNoteColor;

    u32			_selectedRowId;
    u32			_firstVisibleRowId;
    u32			_visibleRowCount;
    u16			_rowsPerpage;
	
	u8			_fnf;

	s32			curFile;
	int 		virtualFile;
	s32			virtualCurFile;
	s32			virtualOffset;

	u32 		fileCounter;
	u32 		viewFileCounter;
	
    bool		_touchMovedAfterTouchDown;

	cBMP15   	_tick;
	cBMP15    	_cross;
	cBMP15   	_folder_plus;
	cBMP15   	_folder_minus;
	cBMP15    	_note;
	cBMP15    	_arrow_up;
	cBMP15    	_arrow_down;

	const char *_romid;

    akui::cButton _buttonSave;
    akui::cButton _buttonCancel;
    akui::cButton _buttonDB;
    akui::cButton _buttonRebuild;
};




#endif//_CHEATLIST_H_
