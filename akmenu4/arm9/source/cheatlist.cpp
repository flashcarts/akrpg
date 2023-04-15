/*---------------------------------------------------------------------------------


Copyright (C) 2008 Normmatt, www.normmatt.com, Smiths (www.emuholic.com)

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









#include <fat.h>
#include <string.h>
#include <unistd.h>
#include <cctype>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include "cheatlist.h"
#include "systemfilenames.h"
#include "msgbox.h"
#include "windowmanager.h"
#include "globalsettings.h"
#include "uisettings.h"
#include "language.h"
#include "unicode.h"
#include "settingwnd.h"
#include "files.h"
#include "mainlist.h"
#include "crc32.h"
#include "progresswnd.h"
#include "fontfactory.h"

#define MAXSLOTS 10
#define SKIPCOUNT 4

#define isON(s)				((s&(BIT(4)|BIT(7))) == (BIT(4)|BIT(7)))
#define isOFF(s)			((s&(BIT(4)|BIT(7))) == (BIT(4)))
#define isEXPANDED(s)		((s&(BIT(6)|BIT(7))) == (BIT(6)|BIT(7)))
#define isCOLLAPSED(s)		((s&(BIT(6)|BIT(7))) == (BIT(6)))
#define inFOLDER(s)			( s & BIT(1) )
#define isNOTE(s)			( s & (BIT(3)|BIT(5)))

using namespace akui;

cCheatListWnd::cCheatListWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cForm( x, y, w, h, parent, text ),
_buttonSave(0,0,46,18,this,"\x01 Save"),
_buttonCancel(0,0,46,18,this,"\x02 Cancel"),
_buttonDB(0,0,46,18,this,"DB Select"),
_buttonRebuild(0,0,46,18,this,"Rebuild")
{
    s16 buttonY = size().y - _buttonCancel.size().y - 4;

	// init buttons
    _buttonCancel.setStyle( cButton::press );
    _buttonCancel.setText( "\x02 " + LANG( "cheats", "Cancel" ) );
    _buttonCancel.setTextColor( uis().buttonTextColor );
    _buttonCancel.loadAppearance( SFN_BUTTON3 );
    _buttonCancel.clicked.connect( this, &cCheatListWnd::onCancel );
    addChildWindow( &_buttonCancel );

    _buttonSave.setStyle( cButton::press );
    _buttonSave.setText( "\x01 " + LANG( "cheats", "Save" ) );
    _buttonSave.setTextColor( uis().buttonTextColor );
    _buttonSave.loadAppearance( SFN_BUTTON3 );
    _buttonSave.clicked.connect( this, &cCheatListWnd::onSaveExit );
    addChildWindow( &_buttonSave );

    _buttonDB.setStyle( cButton::press );
    _buttonDB.setText(  LANG( "cheats", "DB Select" ) );
    _buttonDB.setTextColor( uis().buttonTextColor );
    _buttonDB.loadAppearance( SFN_BUTTON3 );
    _buttonDB.clicked.connect( this, &cCheatListWnd::onShowDBSelectWnd );
    addChildWindow( &_buttonDB);

    _buttonRebuild.setStyle( cButton::press );
    _buttonRebuild.setText( LANG( "cheats", "Rebuild" ) );
    _buttonRebuild.setTextColor( uis().buttonTextColor );
    _buttonRebuild.loadAppearance( SFN_BUTTON3 );
    _buttonRebuild.clicked.connect( this, &cCheatListWnd::onRebuildCheat);
    addChildWindow( &_buttonRebuild);

	// arrange button position
    s16 nextButtonX = size().x;
    s16 buttonPitch = _buttonCancel.size().x + 4;
    nextButtonX -= buttonPitch;
    _buttonCancel.setRelativePosition( cPoint(nextButtonX, buttonY) );

    buttonPitch = _buttonSave.size().x + 2;
    nextButtonX -= buttonPitch;
    _buttonSave.setRelativePosition( cPoint(nextButtonX, buttonY) );

	nextButtonX = 4;
	_buttonRebuild.setRelativePosition( cPoint(nextButtonX, buttonY) );

    nextButtonX += _buttonRebuild.size().x + 2;
	_buttonDB.setRelativePosition( cPoint(nextButtonX, buttonY) );

    loadAppearance( "" );
    arrangeChildren();
	
	_size = cSize( w, h );
    _position = cPoint( x, y );
    _rowHeight = 12;
    _selectedRowId = 0;
    _firstVisibleRowId = 0;
    _visibleRowCount = 10;
    _rowsPerpage = 0;
    _textColor = uiSettings().listTextColor; //RGB15(24,24,24);
    _textColorHilight = uiSettings().listTextHighLightColor; //RGB15(31,31,31);
	_selectionBarColor1 = BIT(15)|uiSettings().listViewBarColor1; //RGB15(0,0,31);
    _selectionBarColor2 = BIT(15)|uiSettings().listViewBarColor2; //RGB15(0,0,31);

    CIniFile ini( SFN_UI_SETTINGS);
	_verticalLineColor = BIT(15)| ini.GetInt("cheats", "verticalLineColor", _textColor); 
	_textNoteColor = BIT(15) | ini.GetInt("cheats","textNoteColor", _textColor);
	
	//Setup some initial values;
	_fnf = 0;
	curFile = 0;
	virtualCurFile = 0;
	virtualFile = 0;
	fileCounter = 0;
	viewFileCounter = 0;
	_touchMovedAfterTouchDown = false;
	
	//Load bitmaps for Tick and Cross
	if( !_tick.valid() ) {
        _tick = createBMP15FromFile( SFN_CHEATS_TICK );
    }
	if( !_cross.valid() ) {
        _cross = createBMP15FromFile( SFN_CHEATS_CROSS );
    }
	if( !_folder_plus.valid() ) { 
        _folder_plus = createBMP15FromFile( SFN_CHEATS_FOLDER_PLUS );
    }
	if( !_folder_minus.valid() ) { 
        _folder_minus = createBMP15FromFile( SFN_CHEATS_FOLDER_MINUS);
    }
	if( !_note.valid() ) { //- note? (Smiths)
        //_note = createBMP15FromFile( SFN_CHEATS_NOTE );
    }
    if( !_arrow_up.valid() ) { //- scroll arrows soon enough (Smiths)
        _arrow_up = createBMP15FromFile( SFN_CHEATS_ARROW_UP );
    }
    if( !_arrow_down.valid() ) { //- scroll arrows soon enough (Smiths)
        _arrow_down = createBMP15FromFile( SFN_CHEATS_ARROW_DOWN );
    }
	
}

cCheatListWnd::~cCheatListWnd()
{
	cRows.clear();
}

void cCheatListWnd::drawIcon( cBMP15 background, int x, int y, GRAPHICS_ENGINE engine )
{
	gdi().maskBlt( background.buffer(),
		x, y, background.width(), background.height(), engine );
}

void cCheatListWnd::insertRow( const char *text, u8 status, u32 oFile)
{
	cRow row;
	row.status = status;
	row.oNote = 0;
	row.oChild = 0;
	row.oFile = oFile;
	row.text = text;

	cRows.push_back(row);
	fileCounter += 1;
	if ( !(status & BIT(1)) ) viewFileCounter += 1; // inFolder
}
	
void cCheatListWnd::ReadList()
{
	

	//Load up Cheat List
	FILE *f;
    //int i=0;
	size_t len=0;
	char string[512];
   	
	fileCounter = 0;
	virtualCurFile = 0;
	viewFileCounter = 0;
	_fnf = 0;
	curFile = 0;
	virtualFile = 0;
	virtualOffset = 0;
	cRows.clear();
	u8 inFolder = 0;
	u16 tNote = 0, tFolder = 0;

	u32 width = size().x - 14;
	std::string breakedMsg;
    //std::string breakedMsg = font().breakLine( msg, 192 );

    struct stat st;
    if( (-1 == stat( _cheatName.c_str(), &st )) || st.st_size == 0 ) 
	{
		_fnf = 1;

		perror ("Error opening file");
		//std::string title = LANG("cheats", "error_title");
        std::string text = "     " + LANG("cheats", "error_text");	
		insertRow( text.c_str(),0,0);
	}
    else // parse cc
	{
		f = fopen(_cheatName.c_str(),"rb");
        while(!feof(f))
        {
			fgets (string , 512 , f);
            len = strlen(string);
			std::string cheatline = ""; cheatline = string;

			if(cheatline.substr(0,14) == ";@@EndOfFolder") {
				inFolder &= ~(BIT(1));
			}
			else if(cheatline.substr(0,14) == ";@@Folder Name")
			{
				cheatline = cheatline.substr(15);
				if ( gs().language != 3 && gs().language != 2 && gs().language != 1 && gs().language != 8 ) // skip for 2byte languages -- thanks to kzat3
				std::transform(cheatline.begin(), cheatline.end(), cheatline.begin(), (int(*)(int)) toupper); 


				sprintf(string, cheatline.c_str());
				len = strlen(string);
                string[0] = 0x20;

				cheatline = font().breakLine(cheatline, width);
				size_t idx = cheatline.find_first_of('\n');
				if ( idx < len - 1 ) { sprintf(&string[idx], "...") ; }

				tNote = cRows.size();
				tFolder = tNote;
				insertRow(string, BIT(6), ftell(f) - len ); // BIT(6) -- folder 
				inFolder = BIT(1);

			}
			else if(cheatline.substr(0,12) == ";@Cheat Note")
			{
				cheatline = cheatline.substr(13);

				size_t pos1 = 0;
				size_t pos2 = 1;

				cheatline = font().breakLine(cheatline, width - ((inFolder & BIT(1))? 8 : 0) );
				pos1 = cheatline.find('\n');
				while(cheatline.npos != pos1)  {
					sprintf(string, cheatline.substr(pos2 , pos1).c_str());

					insertRow(string, inFolder|BIT(3),0); // -- BIT(3) : note of the cheat.
					if ( inFolder & BIT(1) ) cRows[cRows.size()-1].oChild = ++cRows[tFolder].oChild;
					cRows[cRows.size()-1].oNote = ++cRows[tNote].oNote;

					pos2 = pos1 + 1;
					pos1 = cheatline.find( '\n', pos2 );
				}

			}
			else if(cheatline.substr(0,14) == ";@@Folder Note")
			{
				cheatline = cheatline.substr(15);

				size_t pos1 = 0;
				size_t pos2 = 1;

				cheatline = font().breakLine(cheatline, width);
				pos1 = cheatline.find('\n');
				while(cheatline.npos != pos1)  {
					sprintf(string, cheatline.substr(pos2 , pos1).c_str());

					insertRow(string, BIT(5), 0); // BIT(5) : note of the folder
					cRows[cRows.size()-1].oNote = ++cRows[tNote].oNote;
					cRows[cRows.size()-1].oChild = ++cRows[tFolder].oChild;

					pos2 = pos1 + 1;
					pos1 = cheatline.find( '\n', pos2 );
				}
			}
            

			else if(string[0]=='@')
            {
				cheatline = font().breakLine(cheatline, width - ((inFolder & BIT(1)) ? 8 : 0));
				size_t idx = cheatline.find_first_of('\n');
				if ( idx < len - 1 ) { sprintf(&string[idx], "...") ; }
                string[0] = 0x20;

				tNote = cRows.size();
				insertRow(string, inFolder|BIT(4)|BIT(7), ftell(f)-len); // BIT(7) - on/off, BIT(4) : cheat
				if ( inFolder & BIT(1) ) cRows[cRows.size()-1].oChild = ++cRows[tFolder].oChild;
            } else if(string[0]=='#')
            {
				cheatline = font().breakLine(cheatline, width - ((inFolder & BIT(1)) ? 8 : 0));
				size_t idx = cheatline.find_first_of('\n');
				if ( idx < len - 1 ) { sprintf(&string[idx], "...") ; }
                string[0] = 0x20;

				tNote = cRows.size();
				insertRow(string, inFolder|BIT(4), ftell(f)-len);
				if ( inFolder & BIT(1) ) cRows[cRows.size()-1].oChild = ++cRows[tFolder].oChild;
            }
         }
         fclose (f);
	}
}

void cCheatListWnd::draw()
{
    _renderDesc.draw( windowRectangle(), _engine );

    gdi().setPenColor( uiSettings().formTitleTextColor, selectedEngine() );
	
	s32 i=0;
	s32 cSize = MAXSLOTS;
	cBMP15 background;

	/*
	// to debug	
	char temp[256];
	sprintf(temp, "%d/%d/%d/%d/%d/%d   0x%X", curFile, virtualFile, fileCounter, viewFileCounter, virtualOffset,virtualCurFile, cRows[virtualFile+curFile].oFile);
    gdi().textOutRect( position().x + 58, position().y + 3, size().x - 8, 40, temp, selectedEngine() );
	*/
	
	if(viewFileCounter < MAXSLOTS ) cSize = viewFileCounter;
	
	if(_fnf) cSize = 1; else drawSelectionBar();

	s32 idx = 0;
	bool fs = false;
	
	const char *title[] = { "[O]", "[X]", " +", " -", "  " };
	const char *tt;
	for(i=curFile; i < curFile + cSize; i++) {
		// checking the end of file if folder is folded.
		if ( (u32)i >= cRows.size() ) break;

		if ( inFOLDER(cRows[i].status) && isCOLLAPSED(cRows[i-cRows[i].oChild].status) ) {	// skip rows in folder	
			u16 offset = cRows[i-cRows[i].oChild].oChild - cRows[i].oChild  ; 
			i += offset;
			cSize += offset +1;
			continue;
		}
	
		if ( isON(cRows[i].status) ) {
			background = _tick; 
			tt = title[0];
		}
		else if ( isOFF(cRows[i].status) ) {
			background = _cross;
			tt = title[1];
		}
		else if ( isCOLLAPSED( cRows[i].status ) ) {
			background = _folder_plus;
			tt = title[2];
		}
		else if ( isEXPANDED( cRows[i].status ) ) {
			background = _folder_minus; 
			tt = title[3];
			fs= background.valid() ? true : false; 
		}
		else if ( isNOTE( cRows[i].status ) ) {
			background = _note;
			tt = title[4];
		}
		else {
			//gdi().textOutRect( position().x + 25 , position().y + 22 + 3*12, size().x - 8 , 40, LANG("cheats", "error_title").c_str(), selectedEngine() );
			gdi().textOutRect( position().x + 25 , position().y + 22 , size().x - 8 , 40, cRows[i].text.c_str(), selectedEngine() );
			break;
		}

		u32 aX = inFOLDER(cRows[i].status) ? 9 : 0;
		if ( isNOTE(cRows[i].status) ) aX += 1;
	
		if ( cRows[i].status & (BIT(0)) ) {
			// draw vertical line
			gdi().setPenColor( _verticalLineColor, selectedEngine() );
			gdi().drawLine( position().x + 9, position().y + idx *12 + (!fs ? 18 : 22), 
					position().x + 9 , position().y + 18 + idx * 12 + _rowHeight, selectedEngine());
			fs = false;

			if(!isNOTE(cRows[i].status))
			gdi().drawLine( position().x + 9, position().y + 18 + idx *12 + _rowHeight, 
					position().x + 11 , position().y + 18 + idx * 12 + _rowHeight, selectedEngine());
		}
		
		if( virtualFile == idx)
			gdi().setPenColor( _textColorHilight, selectedEngine() );
        else
			//gdi().setPenColor( _textColor, selectedEngine() );
			gdi().setPenColor( ((isNOTE(cRows[i].status))? _textNoteColor : _textColor), selectedEngine() ); // kzat3
	
		if( background.valid() ) {
			drawIcon( background, position().x + 5 + aX, position().y + 23 + idx*12, selectedEngine()); // drawing icon
			gdi().textOutRect( position().x + 15 + aX, position().y + 22 + idx*12, size().x - 8 - aX, 40, cRows[i].text.c_str(), selectedEngine() ); // drawing text
			if ( cRows[i].status & BIT(6) && gs().language == 8 )
			gdi().textOutRect( position().x + 16 + aX, position().y + 22 + idx*12, size().x - 8 - aX, 40, cRows[i].text.c_str(), selectedEngine() ); // make a bold
		}
		else {
			gdi().textOutRect( position().x + 4 + aX, position().y + 22 + idx*12, size().x - 8 - aX, 40, tt, selectedEngine() ); // drawing icon
			gdi().textOutRect( position().x + 17 + aX, position().y + 22 + idx*12, size().x - 8 - aX, 40, cRows[i].text.c_str(), selectedEngine() ); // drawing text
			if ( cRows[i].status & BIT(6) && gs().language == 8 )
			gdi().textOutRect( position().x + 18 + aX, position().y + 22 + idx*12, size().x - 8 - aX, 40, cRows[i].text.c_str(), selectedEngine() ); // make a bold
		}

		//if( (cList[(i<<1)+1]==" -") || (cList[(i<<1)+1]==" +") )
		//gdi().textOutRect( position().x + 23, position().y + 22 + idx*12, size().x - 8, 40, cList[(i<<1)].c_str(), selectedEngine() );

		idx +=1;
	}
		
    if(viewFileCounter > (u32)virtualCurFile+MAXSLOTS) //More Cheats Indicator Down - Smiths
    {
        if (_arrow_down.valid()) drawIcon( _arrow_down, 230, 18, selectedEngine());
        else //draw crude arrow
        {
            gdi().drawLine( 230, 18, 230, 26, selectedEngine() );
            gdi().drawLine( 228, 22, 230, 26, selectedEngine() );
            gdi().drawLine( 232, 22, 230, 26, selectedEngine() );
        }
    }
    if(viewFileCounter > (u32)virtualCurFile-1) //More Cheats Indicator Up - Smiths
    {
        if (_arrow_up.valid()) drawIcon( _arrow_up, 240, 18, selectedEngine());
        else //draw crude arrow
        {
            gdi().drawLine( 240, 18, 240, 26, selectedEngine() );
            gdi().drawLine( 238, 22, 240, 18, selectedEngine() );
            gdi().drawLine( 242, 22, 240, 18, selectedEngine() );
        }

    }

    cForm::draw();

}

void cCheatListWnd::drawSelectionBar()
{
	s16 x = _position.x + 1;
    s16 y = _position.y + _rowHeight*2 + virtualFile * _rowHeight - 2;
    s16 w = _size.x - 2;
    s16 h = _rowHeight;// - 1;

    for( u8 i = 0; i < h; ++i ) {
            gdi().fillRect( _selectionBarColor1, _selectionBarColor2, x, y+i, w, 1, _engine );
    }
}

bool cCheatListWnd::process( const akui::cMessage & msg )
{
    bool ret = false;

    ret = cForm::process( msg );

    if( !ret ) {
        if( msg.id() > cMessage::keyMessageStart && msg.id()
            < cMessage::keyMessageEnd )
        {
            ret = processKeyMessage( (cKeyMessage &)msg );
        }
        else if( msg.id() > cMessage::touchMessageStart
            && msg.id() < cMessage::touchMessageEnd )
        {
            ret = processTouchMessage( (cTouchMessage &)msg );
        }
    }
    return ret;
}

void cCheatListWnd::prevFile(u8 sc)
{
	while( 0 < sc--) {
		virtualCurFile -= 1;
		curFile -= 1;
		if ( curFile < 0 ) {
			curFile = 0;
			return;
		}

		if ( inFOLDER(cRows[curFile].status) && isCOLLAPSED( cRows[curFile - cRows[curFile].oChild ].status)) {
			u16 offset = cRows[curFile].oChild - cRows[curFile-cRows[curFile].oChild].oNote;
			curFile -= offset;
			virtualOffset += offset;
		}
	}
}

void cCheatListWnd::nextFile(u8 sc)
{
	while( 0 < sc--) {
		virtualCurFile += 1;
		curFile += 1;

		if ( inFOLDER(cRows[curFile].status) && isCOLLAPSED(cRows[curFile - cRows[curFile].oChild ].status)) {
			u32 offset = cRows[curFile-cRows[curFile].oChild].oChild - cRows[curFile].oChild + 1;
			curFile += offset;
			virtualOffset -= offset;
		}
	}
}

u8 cCheatListWnd::selectPrev(u8 sc)
{
	u8 count = 0;
	while( 0 < sc--) {
		u32 c;
		virtualFile--;

		if ( !curFile && (virtualFile < 0) ) {
			virtualFile = 0;
			virtualCurFile = 0;
			virtualOffset = 0;
			return count;
		}

		c = virtualFile+curFile+virtualOffset;
		if ( inFOLDER(cRows[c].status) 
				&& isCOLLAPSED( cRows[c - cRows[c].oChild].status ) ) {
			virtualOffset -= (cRows[c - cRows[c].oChild].oChild - cRows[c - cRows[c].oChild].oNote);
		}

		c = virtualFile+curFile+virtualOffset;
		if ( isNOTE(cRows[c].status)) {
			virtualFile -= cRows[c-cRows[c].oNote].oNote;
			count += cRows[c-cRows[c].oNote].oNote;
		}

		if ( virtualFile < 0 ) {
			prevFile(  -virtualFile );
			virtualFile = 0;
		}
		count += 1;
	}
	return count;
}

u8 cCheatListWnd::selectNext(u8 sc)
{
	u8 count = 0;
	while(0 < sc--) {

		if ( viewFileCounter < MAXSLOTS ) {
			if ( (u32)virtualFile == (viewFileCounter - 1)) { // end of file.
				return count;
			}
		}

		u32 c;
		s32 offsetStart = virtualOffset;
		s32 virtualStart = virtualFile;
		s32 scroll = 0;

		virtualFile++;
		if ( virtualFile >= MAXSLOTS ) scroll += 1;

		c = virtualFile+curFile+virtualOffset;

		// skip notes
		s32 notes = 0;
		while ( c < cRows.size() && isNOTE( cRows[c].status ) ) {
			virtualFile +=1; 
			notes += 1;
			count += 1;
			if ( virtualFile >= MAXSLOTS ) { scroll += 1; }
			c = virtualFile+curFile+virtualOffset;
		}

		// skip hidden rows
		if ( c < cRows.size() 
				&& inFOLDER(cRows[c].status) 
				&& isCOLLAPSED( cRows[c-cRows[c].oChild].status ) ){
			virtualOffset += (cRows[c - cRows[c].oChild].oChild - cRows[c - cRows[c].oChild].oNote);
		}


		if ( virtualFile >= MAXSLOTS ) { 
			if ( (u32)(virtualCurFile + MAXSLOTS) >= viewFileCounter ) 
			{
				virtualOffset = offsetStart;
				virtualFile = MAXSLOTS - 1;
				if ( isNOTE( cRows[virtualFile+curFile+virtualOffset].status ) ) 
					virtualFile -= cRows[virtualFile+curFile+virtualOffset].oNote;
				return count;
			}

			c = virtualFile + curFile + virtualOffset; // reset cursor
			if ( c < cRows.size() ) {
				nextFile( scroll );
				virtualFile = MAXSLOTS -1;
				if ( isNOTE( cRows[virtualFile+curFile+virtualOffset].status ) ) 
					virtualFile -= cRows[virtualFile+curFile+virtualOffset].oNote;
			}
			
			else { // end of file
				if ( notes ) { // there are notes at the end of the file
					nextFile(  notes );
					virtualFile = virtualStart - notes ; // back to end of point

					// are there remain hidden files.
					c = virtualFile + curFile + virtualOffset; // reset cursor
					if ( ((u32)( c + notes ) < cRows.size() ) 
						&& inFOLDER(cRows[c + notes].status) 
							&& isCOLLAPSED(cRows[c + notes - cRows[c + notes].oChild].status))
						virtualOffset -= (cRows[c + notes - cRows[c + notes].oChild].oChild - cRows[c + notes - cRows[c + notes].oChild].oNote);
				}
				else {
					virtualOffset = offsetStart;
					virtualFile = virtualStart;
				}
				return count;
			}
		}
		else {
			// after folded
			if ( (u32)(virtualFile+ virtualCurFile ) >= viewFileCounter ) {
				virtualOffset = offsetStart;
				virtualFile = virtualStart;
				return count;
			}

			// notes 
			if ( (u32)(virtualFile+curFile + virtualOffset ) >= cRows.size() ) {
				virtualOffset = offsetStart;
				virtualFile = virtualStart;
				return count;
			}
		}
		count += 1;
	}

	return count;
}

bool cCheatListWnd::processKeyMessage( const cKeyMessage & msg )
{
    bool ret = false;
    if( msg.id() == cMessage::keyDown )
    {
        switch( msg.keyCode() )
        {
		case cKeyMessage::UI_KEY_UP:
			if(!_fnf)
			{
				selectPrev(1);
			}
            ret = true;
            break;
		case cKeyMessage::UI_KEY_DOWN:
			if(!_fnf)
			{
				selectNext(1);
			}
            ret = true;
            break;
		case cKeyMessage::UI_KEY_LEFT:
			if(!_fnf) {
				selectPrev(SKIPCOUNT);
			}
			ret = true;
            break;
		case cKeyMessage::UI_KEY_RIGHT:
			if(!_fnf) {
				selectNext(SKIPCOUNT);
			}
			ret = true;
            break;
        case cKeyMessage::UI_KEY_START:
			onRebuildCheat();
            ret = true;
            break;
		case cKeyMessage::UI_KEY_SELECT:
            onShowDBSelectWnd();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_X:
            onToggle();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_Y:
			onKeyYPressed();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_A:
            onSaveExit();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_B:
            onCancel();
            ret = true;
            break;
        default:
            {}
        };
    }

    return ret;
}

void cCheatListWnd::onKeyYPressed()
{
	u32 c = virtualFile+curFile+virtualOffset;
	while ( inFOLDER(cRows[c].status) && isEXPANDED(cRows[c - cRows[c].oChild].status) ) {
		selectPrev(1);
		c = virtualFile+curFile+virtualOffset;
	}

	if ( cRows[virtualFile+curFile+virtualOffset].status & BIT(6) ) onToggle();
}

void cCheatListWnd::onToggle()
{
	u32 c = virtualFile+curFile+virtualOffset;

	if ( isON( cRows[c].status ) ) 
		cRows[c].status &= ~BIT(7);
	else if ( isOFF(cRows[c].status) )
		cRows[c].status |= BIT(7);
	else if ( isEXPANDED(cRows[c].status) ) {
		cRows[c].status &= ~BIT(7);
		viewFileCounter -= (cRows[c].oChild - cRows[c].oNote);

        u32 i = 1;
        while( ( c + i < cRows.size() ) && (cRows[c+i].status&BIT(5)) ) 
            cRows[c + i++].status &=  ~BIT(0);
        while( ( c + i < cRows.size() ) && ( cRows[c+i].status & BIT(1)) ) 
            cRows[c + i++].status &=  ~BIT(0);

	}
	else if ( isCOLLAPSED(cRows[c].status) ) {
		cRows[c].status |= BIT(7);
		viewFileCounter += (cRows[c].oChild - cRows[c].oNote);

        u32 i = 1;
        while( ( c + i < cRows.size() ) && (cRows[c+i].status&BIT(5)) ) 
            cRows[c + i++].status |=  BIT(0);
        while( ( c + i < cRows.size() ) && ( cRows[c+i].status & BIT(1)) ) 
            cRows[c + i++].status |=  BIT(0);
        i -= 1;
        while (i && isNOTE(cRows[c+i].status) ) cRows[c + i--].status &=  ~BIT(0);

	}
}
bool cCheatListWnd::processTouchMessage( const akui::cTouchMessage & msg )
{
    bool ret = false;
    static int sumOfMoveY = 0;

    if( msg.id() == cMessage::touchMove ) {
        if( abs(msg.position().y) > 0 ) {
            sumOfMoveY += msg.position().y;
        }
        if( sumOfMoveY > gs().scrollSpeed ) {
            selectNext(1);
            sumOfMoveY = 0;
            _touchMovedAfterTouchDown = true;
        } else if( sumOfMoveY < -gs().scrollSpeed ) {
            selectPrev(1);
            sumOfMoveY = 0;
            _touchMovedAfterTouchDown = true;
        }
        ret = true;
    }

    if( msg.id() == cMessage::touchUp ) {
        sumOfMoveY = 0;
        if( !_touchMovedAfterTouchDown ) {
            u32 rbp = rowBelowPoint( cPoint(msg.position().x,msg.position().y) );
            if( ( rbp != (u32)-1 ) && (_selectedRowId == (u32) rbp )) 
				selectRow();
        }
        _touchMovedAfterTouchDown = false;
        ret = true;
    }

    if( msg.id() == cMessage::touchDown ) {
        _touchMovedAfterTouchDown = false;

        u32 rbp = rowBelowPoint( cPoint(msg.position().x,msg.position().y) );
        if( rbp != (u32)-1 ) {
			_selectedRowId = (u32) rbp;
        }
        ret = true;
    }

	return ret;
}

u32 cCheatListWnd::rowBelowPoint( const akui::cPoint & pt ) 
{
	s16 sx = _position.x + 1;
	s16 sy = _position.y + _rowHeight*2 - 2;
	s16 ex = sx + _size.x - 2;
	s16 ey = sy + _rowHeight * MAXSLOTS ;

    //if( windowRectangle().surrounds( pt ) ) {
	if ( pt.x > sx && pt.x < ex && pt.y > sy && pt.y < ey ) {
        u32 row = (pt.y - sy) / _rowHeight;
		return row;
	}
    return (u32)-1;
}

void cCheatListWnd::selectRow()
{
	int row = (int) _selectedRowId - virtualFile;
	if ( row > 0 ) // down
	{
		// click note??
		u32 c = _selectedRowId + curFile + virtualOffset;
		if ( c < cRows.size()
				&& isNOTE(cRows[c].status) ) {
			_selectedRowId -= cRows[c].oNote;
		}
		
		row = (int) _selectedRowId - virtualFile;
		if ( !row ) onToggle();
		else
		while(row>0) {
			u8 m = selectNext(1);
			if ( !m ) break;
			row -= m;
		}
	}
	else if ( row < 0 ) // up
	{
		while(row<0) {
			u8 m = selectPrev(1);
			if ( !m ) break;
			row += m;
		}
	}
	else // toggle
	{
		onToggle();
	}
}

void cCheatListWnd::onShowDBSelectWnd() 
{
    cSettingWnd settingWnd( 0,0, 252, 188, this, LANG("patches", "cheatdb select" ) );

	settingWnd.setNoConfirm();
    std::vector< std::string > _values;
    u32 cheatindex = 0;
    std::string extName;
    std::vector< std::string > cheatDBs;
    DIR_ITER * dir = diropen( SFN_CHEATS_DIRECTORY );
    if( NULL != dir ) {
        struct stat st;
        char filename[256];
        char longFilename[512];

        while (dirnextl(dir, filename, longFilename, &st) == 0) {
			if ( st.st_mode & S_IFDIR ) continue;
            std::string fn( filename );
            std::string lfn( longFilename );
            if( 0 == longFilename[0] )
                lfn = fn;
            size_t lastDotPos = lfn.find_last_of( '.' );
            if( lfn.npos != lastDotPos )
                extName = lfn.substr( lastDotPos );
            else
                extName = "";
            if( extName == ".XML" || extName == ".xml" || extName == ".DAT" || extName == ".dat" )
                _values.push_back( lfn );
        }
    } else {
        _values.push_back( gs().cheatsDB );
    }
    std::sort( _values.begin(), _values.end() );
    for( size_t i = 0; i < _values.size(); ++i ) {
        _values[i].insert(0, SFN_CHEATS_DIRECTORY);
        if( 0 == stricmp( _values[i].c_str(), gs().cheatsDB.c_str() ) ) { cheatindex = i; }
        _values[i].erase(0,_values[i].find_last_of('/')+1);
    }

    cheatDBs = _values;
	if ( cheatDBs.size() )
		settingWnd.addSettingItem( LANG("patches", "cheatdb select" ), _values, cheatindex );
	else {
		// return message box
	}

    u32 ret = settingWnd.doModal();

    if( ID_CANCEL == ret )
        return;

	if ( cheatDBs.size() ) {
		std::string selectedDB = _values[settingWnd.getItemSelection( 0 )];
		selectedDB.insert(0, SFN_CHEATS_DIRECTORY);
		gs().cheatsDB = selectedDB ; 
	}
}

void cCheatListWnd::onRebuildCheat()
{
	u32 ret = messageBox( NULL,LANG("cheats","confirm"),LANG("cheats", "confirm_rebuild"), MB_OK | MB_CANCEL );
	if ( ret == ID_OK ) {
		unlink(_cheatName.c_str());
		BuildCheatFile(gs().cheatsDB.c_str(), _romid, _shortName) ;
		ReadList();
	}
	doModal();
}

void cCheatListWnd::onSaveExit()
{
	FILE *f = fopen(_cheatName.c_str(),"rb");
	if ( f ) {
		fseek(f, 0, SEEK_END);
		long fSize = ftell(f);
		char *buffer = (char*)malloc(fSize+1);
		fseek(f, 0, SEEK_SET);
		fread (buffer,1,fSize,f);
		fclose(f);	
		
		for(u32 i=0; i<fileCounter; i++)
		{
			if (cRows[i].status & BIT(4)) {
				buffer[cRows[i].oFile] = isON(cRows[i].status) ? '@' : '#';
			}
			
		}
		
		f = fopen(_cheatName.c_str(),"wb");
		fwrite (buffer , 1 , fSize , f);
		free(buffer);
		fclose(f);
	}
	
	cForm::onOK();
}


cWindow& cCheatListWnd::loadAppearance(const std::string& aFileName )
{
    _renderDesc.loadData( SFN_FORM_TITLE_L, SFN_FORM_TITLE_R, SFN_FORM_TITLE_M );
    _renderDesc.setTitleText( _text );
    return *this;
}


// shortName is ascii code, it is used for open file
// longName is converted from unicode filename, it is used for show
bool cCheatListWnd::setFileInfo( const std::string & shortPath, const char* romid )
{	
    if( "" == shortPath ) {
        dbg_printf("short name %s\n", shortPath.c_str());
        return false;
    }

    struct stat st;
    if( -1 == stat( shortPath.c_str(), &st ) ) {
        return false;
    }

	_cheatName = shortPath;
	_cheatName = shortPath.substr(0,shortPath.find_last_of('.') +1);
	_cheatName += "cc";
	
	_shortName = shortPath;
	_romid = romid;
	return BuildCheatFile(gs().cheatsDB.c_str(), romid, shortPath); //--Realtime cheat creation from XML? OK! - Smiths
	
}

void cCheatListWnd::onOK()
{
    cForm::onOK();
}

void cCheatListWnd::onCancel()
{
    cForm::onCancel();
}

void cCheatListWnd::onShow()
{
    centerScreen();
}

// convert to local
static void _printName(FILE *CCfile, const char *str) {
	if ( _FAT_unicode_is_utf8((const u8*)str)) {
		UNICHAR *unicode_name = (UNICHAR *)malloc(strlen(str)* 2);
		char *local_name = (char *)malloc(strlen(str));
		_FAT_unicode_utf8_to_unicode((const u8*)str,unicode_name );
		_FAT_unicode_unicode_to_local((const UNICHAR *)unicode_name,(u8 *)local_name);
		fprintf(CCfile, "%s\n", local_name );
		free(local_name);
		free(unicode_name);
	}
	else {
		fprintf(CCfile, "%s\n",  str); 
	}
}
bool cCheatListWnd::_parseXML(FILE *XML, size_t xmlSize, FILE *CCfile, const char *romid, u32 crc32 ) {
    char theCRC[9];
	sprintf(theCRC,"%08X",(uint) crc32); // change %X to %08X to match games with a zero-started crc.  warmup 2008/03/05
	std::string CRC = theCRC;

	int done = 0; int hascheats = 0;
	fpos_t pos;
	char line [256] = "";
	std::string gameidXML = "<GAMEID>"; gameidXML.append(romid); gameidXML.append(" "); gameidXML.append(CRC); gameidXML.append("\0");
	std::string XMLin = "", XMLin2 = "";
	std::string cheatXML = "<cheat>", noteXML = "<note>";
	std::string folderXML = "<folder", folderXMLclose = "</folde"; //folder first is ';'

    progressWnd().setTipText(LANG("progress window","rebuilding cheat data" ));
    progressWnd().show();
    progressWnd().setPercent(0);

	//printf("%s \n", gameidXML.c_str());
	u32 pc = 0;
	while( !feof(XML) && done == 0)
	{
		if (XMLin.length() > 8) XMLin2 = XMLin.substr(2,XMLin.length()-7);
		fgets(line, 256, XML); XMLin.clear(); XMLin = line; 
		std::transform(XMLin.begin(), XMLin.end(), XMLin.begin(), (int(*)(int)) toupper);
		while (gameidXML.compare(XMLin.substr(2,21)) == 0) //it's the game's gameid line, moving on
		{
			//printf("FOUND %s \n", XMLin.c_str());
			//fprintf(CCfile, "; %s\n", XMLin2.substr(6,XMLin2.length()-10).c_str());
			fprintf(CCfile,";@@@@@ Game Title: ");
			_printName(CCfile,XMLin2.substr(6,XMLin2.length()-10).c_str());
			fprintf(CCfile, ";@@@@@ Game ID : %s\n", XMLin.substr(10,XMLin.length()-21).c_str());
			fprintf(CCfile,";----- Generated by ACEKARD R.P.G OSMENU\n\n\n");
			fgets(line, 256, XML); XMLin.clear(); XMLin = line; 
			hascheats = 1;
			done = 1;
		}
		if ( !( (pc++<<8) % 0x100000) ) 
			progressWnd().setPercent(ftell(XML) * 100 / xmlSize);
	}
	progressWnd().setPercent(100);
	progressWnd().hide();

	if ( !hascheats ) return false;

	// rebuild cheat file
	XMLin.clear(); done = 0;
	char folder_or_cheat[256];
	while( !feof(XML) && done == 0)
	{
		fgets(folder_or_cheat, sizeof(folder_or_cheat), XML); XMLin.clear(); XMLin = folder_or_cheat; 
		if (XMLin.length() > 1) 
		{
			XMLin = XMLin.substr(1,8); //cheat or folder start 2 in, but </game> is 1 in
		}
		if (folderXML.compare(XMLin.substr(1,8)) == 0) //see if there's a <folder>
		{
			fgetpos (XML,&pos); //remember where we are, gotta count items
			int folderend = 0; int items = 0; std::string allowed;
			while (folderend == 0) //do until end of </folder>
			{
				fgets(line, sizeof(line), XML); XMLin.clear(); XMLin = line; 
				if (XMLin.substr(3,11).compare("<allowedon>") == 0 ) //allowed on 0 or 1
				{
					if (XMLin.substr(14, 1).compare("0") == 0) allowed = "multi";
					if (XMLin.substr(14, 1).compare("1") == 0) allowed = "one";
				}
				if (cheatXML.compare(XMLin.substr(3,7)) == 0) 
				{
					items++;
				}
				if (XMLin.substr(2,9).compare("</folder>") == 0)	{ folderend = 1; }
			}
			fsetpos (XML, &pos); //back to <folder> start
			fgets(line, sizeof(line), XML); XMLin.clear(); XMLin = line; //next line = folder's <name> with 3 blank
			fprintf(CCfile, "; ---- folder (%s select) items = %i ----\n", allowed.c_str(), items);
			//fprintf(CCfile, "; %s\n", XMLin.substr(9,XMLin.length()-18).c_str()); //<folder><name> written
			//std::string folderName = XMLin.substr(9,XMLin.length()-18).c_str();;
			fprintf(CCfile,";@@Folder Name: ");
			_printName(CCfile, XMLin.substr(9,XMLin.length()-18).c_str()); //<folder><name> written -- fixed by kzat3
			folderend = 0;
			while (folderend == 0) //do until end of </folder>
			{
				fgets(line, sizeof(line), XML); XMLin.clear(); XMLin = line;
				if (noteXML.compare(XMLin.substr(3,6)) == 0) //a <FOLDER><NOTE>
				{
					XMLin = XMLin.substr(9, XMLin.length()-18); //that's the note
					//fprintf(CCfile, "; %s\n", XMLin.c_str()); //<cheat><name> written
					fprintf(CCfile, ";@@Folder Note: ");
					_printName(CCfile, XMLin.c_str()); //<cheat><name> written
					fgets(line, sizeof(line), XML); XMLin.clear(); XMLin = line;  //next line
				}
				if (cheatXML.compare(XMLin.substr(3,7)) == 0) //cheat in <folder>
				{
					fgets(line, sizeof(line), XML); XMLin.clear(); XMLin = line; //next line = cheat's <name> with 3 blank
					XMLin = XMLin.substr(10, XMLin.length()-19); //that's the name
					//printf("CHEAT: %s \n", XMLin.c_str());
					//fprintf(CCfile, "#%s\n", XMLin.c_str()); //<cheat><name> written
					//std::string cheatName = folderName + " - " + XMLin;
					fprintf(CCfile,"#");
					//_printName(CCfile,cheatName.c_str());
					_printName(CCfile, XMLin.c_str()); //<cheat><name> written
					fgets(line, sizeof(line), XML); XMLin.clear(); XMLin = line;  //next line = the codes
				
					if (noteXML.compare(XMLin.substr(4,6)) == 0) //a <CHEAT><NOTE>
					{
						XMLin = XMLin.substr(10, XMLin.length()-19); //that's the name -- fixed by kzat3
						//fprintf(CCfile, ";%s\n", XMLin.c_str()); //<cheat><note> written
						fprintf(CCfile,";@Cheat Note: ");
						_printName(CCfile,  XMLin.c_str()); //<cheat><note> written
						fgets(line, sizeof(line), XML); XMLin.clear(); XMLin = line;  //next line
					}
					fprintf(CCfile,";------------------\n");
					XMLin = XMLin.substr(11, XMLin.length()-21); //gets the AR <codes>, 1 more in than <cheat>
					std::string aCode;
					std::transform(XMLin.begin(), XMLin.end(), XMLin.begin(), (int(*)(int)) toupper);
					if (XMLin.length() > 16) //more than 1 code on line
					{
						uint i = 17, j = 0;
						aCode = XMLin;
						while (i < XMLin.length())
						{
							aCode.erase(i,1);
							i+=17;
						}
						i = 17;
						while (i < XMLin.length())
						{
							aCode.insert(i+j,"\n");
							i+=17; j+=1;
						}
					}
					else
					{
						aCode = XMLin;
					}
					//printf("CODES:\n%s\n", aCode.c_str());
					fprintf(CCfile, "%s\n", aCode.c_str()); //<folder><cheat><codes> written
				}
				if (XMLin.substr(2,9).compare("</folder>") == 0)	{ 
					folderend = 1; 
					fprintf(CCfile,";@@EndOfFolder\n\n\n");
				} //end of folder found
			}
		}
		else if (cheatXML.compare(XMLin.substr(1,8)) == 0) //cheat nonfolder
		{
			fgets(line, sizeof(line), XML); XMLin.clear(); XMLin = line;  //next line = cheat's <name> with 3 blank
			XMLin = XMLin.substr(9, XMLin.length()-18); //that's the name
			//printf("CHEAT: %s \n", XMLin.c_str());
			//fprintf(CCfile, "#%s\n", XMLin.c_str()); //<cheat><name> written
			fprintf(CCfile,"#");
			_printName(CCfile, XMLin.c_str()); //<cheat><name> written
			fgets(line, sizeof(line), XML); XMLin.clear(); XMLin = line;  //next line = the codes
			if (noteXML.compare(XMLin.substr(3,6)) == 0) //a NOTE, SKIP to next line (codes)
			{
				XMLin = XMLin.substr(9, XMLin.length()-18); //that's the name
				//fprintf(CCfile, ";%s\n", XMLin.c_str()); //<cheat><name> written
				fprintf(CCfile,";@Cheat Note: ");
				_printName(CCfile, XMLin.c_str()); //<cheat><name> written
				fgets(line, sizeof(line), XML); XMLin.clear(); XMLin = line;  //next line
			}
			fprintf(CCfile,";------------------\n");
			XMLin = XMLin.substr(10, XMLin.length()-20); //gets the AR <codes>, 1 more in than <cheat>
			std::string aCode;				
			std::transform(XMLin.begin(), XMLin.end(), XMLin.begin(), (int(*)(int)) toupper);
			if (XMLin.length() > 16) //more than 1 code on line
			{
				uint i = 17, j = 0;
				aCode = XMLin;
				while (i < XMLin.length())
				{
					aCode.erase(i,1);
					i+=17;
				}
				i = 17;
				while (i < XMLin.length())
				{
					aCode.insert(i+j,"\n");
					i+=17; j+=1;
				}
			}
			else
			{
				aCode = XMLin;
			}
			//printf("CODES:\n%s\n", aCode.c_str());
			fprintf(CCfile, "%s\n", aCode.c_str()); //<cheat><codes> written
		}
		else if (XMLin.substr(0,7).compare("</game>") == 0) done = 1; //</game> found, that's all for this game
	}

	return true;
}


/*
Copyright(C) 2007 yasu for r4decrypt
http://hp.vector.co.jp/authors/VA013928/
http://www.usay.jp/
http://www.yasu.nu/

2007/04/22 21:00 - First version

-- get from r4denc by yasu
-- it following GLP2
-- modified by bliss @ 2008/03/03
*/
#define BIT_AT(n, i) ((n >> i) & 1)
void r4decrypt(unsigned char *buf, size_t len, int n) {
	size_t r = 0;
	while ( r < len)
	{
		size_t i ;
		unsigned short key = n ^ 0x484A;
		for (i = 0 ; i < 512 && i < len - r ; i ++)
		{
			unsigned char _xor = 0;
			if (key & 0x4000) _xor |= 0x80;
			if (key & 0x1000) _xor |= 0x40;
			if (key & 0x0800) _xor |= 0x20;
			if (key & 0x0200) _xor |= 0x10;
			if (key & 0x0080) _xor |= 0x08;
			if (key & 0x0040) _xor |= 0x04;
			if (key & 0x0002) _xor |= 0x02;
			if (key & 0x0001) _xor |= 0x01;

			unsigned int k = ((buf[i] << 8) ^ key) << 16;
			unsigned int x = k;
			int j;
			for (j = 1; j < 32; j ++)
				x ^= k >> j;
			key = 0x0000;
			if (BIT_AT(x, 23)) key |= 0x8000;
			if (BIT_AT(k, 22)) key |= 0x4000;
			if (BIT_AT(k, 21)) key |= 0x2000;
			if (BIT_AT(k, 20)) key |= 0x1000;
			if (BIT_AT(k, 19)) key |= 0x0800;
			if (BIT_AT(k, 18)) key |= 0x0400;
			if (BIT_AT(k, 17) != BIT_AT(x, 31)) key |= 0x0200;
			if (BIT_AT(k, 16) != BIT_AT(x, 30)) key |= 0x0100;
			if (BIT_AT(k, 30) != BIT_AT(k, 29)) key |= 0x0080;
			if (BIT_AT(k, 29) != BIT_AT(k, 28)) key |= 0x0040;
			if (BIT_AT(k, 28) != BIT_AT(k, 27)) key |= 0x0020;
			if (BIT_AT(k, 27) != BIT_AT(k, 26)) key |= 0x0010;
			if (BIT_AT(k, 26) != BIT_AT(k, 25)) key |= 0x0008;
			if (BIT_AT(k, 25) != BIT_AT(k, 24)) key |= 0x0004;
			if (BIT_AT(k, 25) != BIT_AT(x, 26)) key |= 0x0002;
			if (BIT_AT(k, 24) != BIT_AT(x, 25)) key |= 0x0001;
			buf[i] ^= _xor;
		}

		buf+= 512;
		r  += 512;
		n  += 1;
	}
}

bool cCheatListWnd::_parseDAT(FILE *DAT, size_t datSize, FILE *CCfile, const char *romid, u32 crc32) {
	// checking DAT File header
	size_t encSize = 0;
    char *header = "R4 CheatCode";
    char rHeader[13]= {};
    fread( rHeader,1,12,DAT);

    if ( strncmp(header, rHeader,12) ) {
		r4decrypt((unsigned char *)rHeader,12,0);
		if ( strncmp(header, rHeader,12) ) {
			return false;
		}
		encSize = 1;
	}

	// Search Index
	unsigned char encbuf[512];
    DAT_INDEX idx,nidx;

	if ( encSize ) {
		fseek( DAT, 0 , SEEK_SET);
		fread( encbuf ,1, 512, DAT);
		r4decrypt(encbuf, 512 , 0);
		encSize = 0x100;
	}
	else {
		fseek( DAT, 0x100, SEEK_SET);
		fread( &nidx, 1,sizeof(nidx), DAT);
	}


    size_t dataSize = 0;
	bool done = false;
	u32 gamecode;
	memcpy(&gamecode, romid,sizeof(gamecode));
	u16 n = 0;
	size_t nr = 0;
    while( !done ) {
		if ( encSize ) { // encrypted
			nr = encSize % 512;
			memcpy( &idx, encbuf+nr, sizeof(idx));
			encSize += sizeof(idx);
			if ( (encSize >> 9) > n ) {
				n += 1;
				fread(encbuf, 1, 512, DAT);
				r4decrypt(encbuf, 512 , n);
			}
			nr = encSize % 512;
			memcpy( &nidx, encbuf+nr, sizeof(nidx));
		}
		else { // normal
			memcpy(&idx,&nidx,sizeof(idx));
			fread( &nidx,1,sizeof(nidx), DAT);
		}

        if (  gamecode == idx.gamecode && crc32 == idx.crc32 ) { // matching
			dbg_printf("D: %x %x %x\n",(int) idx.gamecode,(int) idx.crc32,(int)idx.offset);
            dataSize = ( nidx.offset ) ? nidx.offset - idx.offset : 0;
			if ( encSize ) { // arrange for encrypted
				encSize = idx.offset;
				nr = encSize % 512;
				dataSize += nr;
			}
			fseek( DAT,  idx.offset - nr  , SEEK_SET); // seek to data
			if ( !nidx.offset && dataSize == nr ) dataSize += ftell(DAT);
			done = true;
        }
		if ( !nidx.offset ) done = true;
    }

	if ( !dataSize) return false;

	// rebuild cheat file
	char *buffer = (char *)malloc(dataSize);
	fread( buffer,1,dataSize,DAT);
	if ( encSize ) r4decrypt((unsigned char *)buffer, dataSize, encSize >> 9 );

	char *gameTitle = (buffer + nr);
	fprintf(CCfile,";@@@@@ Game Title: %s\n",gameTitle);
	fprintf(CCfile,";@@@@@ Game Code: %s\n",romid);
	fprintf(CCfile,";@@@@@ CRC32: 0x%X\n",(uint)crc32);

	dbg_printf(";@@@@@ Game Title: %s\n",gameTitle);
	dbg_printf(";@@@@@ Game Code: %s\n",romid);
	dbg_printf(";@@@@@ CRC32: 0x%X\n",(uint)crc32);

	u32 *ccode = (u32 *)(((u32 )gameTitle+ strlen(gameTitle) + 4) & 0xfffffffc);
	u32 cheatCount = *ccode;

	cheatCount &= ~(0xf0000000);

	ccode += 9;

	fprintf(CCfile,";@@@@@ Number of Cheats: %d\n",(int)cheatCount);
	fprintf(CCfile,";----- Generated by ACEKARD R.P.G OSMENU\n\n\n");
	dbg_printf(";@@@@@ Number of Cheats: %d\n",(int)cheatCount);


	u32 cc = 0;
	size_t i,j;
	while( cc < cheatCount) {
		u32 folderCount = 1;
		bool inFolder = false;
		char *folderName = NULL;
		char *folderNote = NULL;
		if ((*ccode & 0xf0000000) == 0x10000000) {
			folderCount = *ccode & 0x00ffffff;
			folderName = (char *)((u32)ccode +4);
			folderNote = (char *)((u32)folderName+ strlen(folderName) + 1);
			fprintf(CCfile,";@@Folder Name: %s\n",folderName);
			if ( folderNote && *folderNote ) 
				fprintf(CCfile,";@@Folder Note: %s\n\n",folderNote);
			cc++;
			ccode = (u32 *)(((u32)folderName + strlen(folderName) + 1 + strlen(folderNote) + 1 + 3) & 0xfffffffc);
			inFolder = true;
			// process
		}

		for ( i = 0; i < folderCount; i++) {
			char *cheatName = (char *)((u32)ccode + 4);
			char *cheatNote = (char *)((u32)cheatName + strlen(cheatName) +1);
			u32 *cheatData = (u32 *)(((u32)cheatNote + strlen(cheatNote) + 1 + 3) & 0xfffffffc);
			u32 cheatDataLen = *cheatData++;

			if ( cheatDataLen ) {
				/*
				if ( folderName && *folderName ) {
					fprintf(CCfile,"#%s: ",folderName);
					fprintf(CCfile,"%s\n",cheatName);
				}
				else 
				*/
					fprintf(CCfile,"#%s\n",cheatName);
				if ( cheatNote && *cheatNote )
					fprintf(CCfile,";@Cheat Note: %s\n",cheatNote);

				fprintf(CCfile,";@Data Length: %d\n",(int)cheatDataLen);
				fprintf(CCfile,";------------------\n");
				for( j = 0; j < cheatDataLen ; j++ ) {
					fprintf(CCfile,"%08X", (uint)*(cheatData +j));
					( (j+1) % 2 ) ? fprintf(CCfile," ") : fprintf(CCfile,"\n");
				}
				if ( j % 2 ) fprintf(CCfile,"\n");
				fprintf(CCfile,"\n");
			}

			cc++;
			ccode = (u32 *)((u32)ccode+ ((*ccode + 1)*4));
		}
		if ( inFolder ) 
			fprintf(CCfile,";@@EndOfFolder\n\n\n");
	}

	dbg_printf("done...\n");
	free(buffer);
	return true;
}

bool cCheatListWnd::BuildCheatFile(const char* cheatfile, const char* romid, const std::string & outputfile) //Realtime Make cheats from ML -Smiths
{
	struct stat st;
	bool isExistCheatFile = (stat(_cheatName.c_str(),&st) != -1 ) ? true : false;

	if (stat(cheatfile,&st) == -1 ) 
	{ 
		//printf("ERROR CHEATS.XML NOT FOUND\n"); 
		if ( isExistCheatFile ) return true;
		messageBox( NULL,LANG("cheats","error_title"),LANG("cheats", "no cheatdb"), MB_OK );
		return false;
	}
	size_t cheatDBSize= st.st_size;

	if ( !cheatDBSize) {
		if ( isExistCheatFile ) return true;
		messageBox( NULL,LANG("cheats","error_title"),LANG("cheats", "invalid cheatdb"), MB_OK );
		return false;
	}


	if (isExistCheatFile ) // CC exists, make again?
	{
		return true;
		/*
		u32 ret = messageBox( NULL,LANG("cheats","confirm"),LANG("cheats", "ccexists"), MB_YES | MB_NO );
		if( ID_NO == ret ) {
			return true;
		}
		*/
	}

	//get CRC32 for precise detection
	FILE * NDSROM = fopen (outputfile.c_str(), "rb");
	unsigned char buf[511];
	unsigned long crc32 = 0;
	fread(buf, 1, 512, NDSROM);
	crc32 = CheckSum32(buf, 512); 
	crc32 = ~crc32; //bitwise inverse, thanks chishm!
	fclose(NDSROM);

	char *extName = strrchr(cheatfile, '.');


	FILE * cheatDB= fopen(cheatfile,"rb");
	FILE * CCfile = fopen(_cheatName.c_str(), "wb");

	bool ret = 
		strcasecmp(".xml",extName) ?
			_parseDAT(cheatDB,cheatDBSize,CCfile,romid,crc32) : 
			_parseXML(cheatDB,cheatDBSize,CCfile,romid,crc32);

	fclose(cheatDB);
	fclose(CCfile);

	if ( !ret ) {
		unlink(_cheatName.c_str());
		messageBox( NULL,LANG("cheats","error_title"),LANG("cheats", "notfound"), MB_OK );
		return false;
	}

	return true;
}

