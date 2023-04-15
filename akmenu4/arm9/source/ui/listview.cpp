/*---------------------------------------------------------------------------------


Copyright (C) 2007 Acekard, www.acekard.com

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









#include "ui.h"
#include "listview.h"
//#include "gdi.h"
//#include "files.h"
//#include "dbgtool.h"

namespace akui {

cListView::cListView( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
    :cWindow( parent, text )
{
    _size = cSize( w, h );
    _position = cPoint( x, y );
    _rowHeight = 12;
    _selectedRowId = 0;
    _firstVisibleRowId = 0;
    _visibleRowCount = 0;
    _rowsPerpage = 0;
    _textColor = uiSettings().listTextColor; //RGB15(24,24,24);
    _textColorHilight = uiSettings().listTextHighLightColor; //RGB15(31,31,31);
    _selectionBarColor1 = BIT(15)|uiSettings().listViewBarColor1; //RGB15(0,0,31);
    _selectionBarColor2 = BIT(15)|uiSettings().listViewBarColor2; //RGB15(0,0,31);
    _selectionBarOpacity = 100; // bliss
    _engine = GE_MAIN;
    _touchMovedAfterTouchDown = false;

}

void cListView::arangeColumnsSize()
{
    u16 offset = 0;
    for( size_t i = 0; i < _columns.size(); ++i )
    {
        _columns[i].offset = offset;
        offset += _columns[i].width;
    }
}

bool cListView::insertColumn( size_t index, const std::string & text, u8 width )
{
    if( index > _columns.size() )
        return false;

    cListColumn aColumn;
    aColumn.width = width;
    if( index > 0 )
        aColumn.offset = _columns[index-1].offset + _columns[index-1].width;
    else
        aColumn.offset = 0;

    _columns.insert( _columns.begin() + index, aColumn );
    return true;
}

bool cListView::insertRow( size_t index, const std::vector< std::string > & texts )
{
    size_t columnCount = _columns.size();
    if( 0 == columnCount || index > _rows.size() )
        return false;
    if( 0 == texts.size() )
        return false;

    _rows.insert( _rows.begin() + index, itemVector() );
    for( size_t col = 0; col < columnCount; ++col )
    {
        std::string itemText;
        if( col >= texts.size() )
            itemText = "Empty"; // Ä¬ÈÏ×Ö·û´®
        else
            itemText = texts[col];

        cListItem aItem;
        aItem.text = itemText;

        _rows[index].insert( _rows[index].begin() + col, aItem );
    }
    //if( _visibleRowCount > _rows.size() ) _visibleRowCount = _rows.size();

    return true;
}

void cListView::removeRow( size_t index )
{
    if( index >= _rows.size() ) return ;
    _rows.erase( _rows.begin() + index );
}

void cListView::removeAllRows()
{
    _rows.clear();
    _selectedRowId = 0;
    _firstVisibleRowId = 0;
    //_visibleRowCount = 0;
}

void cListView::draw()
{
    //dbg_printf( "cListView::draw %08x\n", this );
    //draw_frame();
    drawSelectionBar();
    drawText();
}

void cListView::drawFrame()
{
    gdi().setPenColor( RGB15(31,31,31), _engine );
    gdi().frameRect( _position.x, _position.y, _size.x, _size.y, _engine );
}

void cListView::drawSelectionBar()
{
    //if( _touchMovedAfterTouchDown )
    //    return;

    s16 x = _position.x - 2;
    s16 y = _position.y + (_selectedRowId - _firstVisibleRowId) * _rowHeight - 1;
    s16 w = _size.x + 4;
    s16 h = _rowHeight - 1;

    //gdi().setPenColor( _selectionBarColor );

    for( u8 i = 0; i < h; ++i ) {
		if ( _selectionBarOpacity < 100 )  { //bliss
			if( i & 1 )
				gdi().fillRectBlend( _selectionBarColor1,_selectionBarColor2,  x, y+i, w, 1,_selectionBarOpacity,_engine );
			else
				gdi().fillRectBlend( _selectionBarColor2,_selectionBarColor1,  x, y+i, w, 1,_selectionBarOpacity,_engine );
		}
		else {
			if( i & 1 )
				gdi().fillRect( _selectionBarColor1, _selectionBarColor2, x, y+i, w, 1, _engine );
			else
				gdi().fillRect( _selectionBarColor2, _selectionBarColor1, x, y+i, w, 1, _engine );
		}
    }
}

void cListView::drawText()
{
    size_t columnCount = _columns.size();
    size_t total = _visibleRowCount;
    if(  total > _rows.size() - _firstVisibleRowId )
        total = _rows.size() - _firstVisibleRowId;
/*
	char tmpfn[512]; // add by bliss
	int fnlen;
*/
    for( size_t i = 0; i < total; ++i )
    {
        for( size_t j = 0; j < columnCount; ++j )
        {
            s32 itemX = _position.x + _columns[j].offset;
            s32 itemY = _position.y + i * _rowHeight + ((_rowHeight - gs().fontHeight) >> 1);
            if( itemY + gs().fontHeight > (s32)(_position.y + _size.y) )
                break;
            if( _selectedRowId == _firstVisibleRowId+i/* && !_touchMovedAfterTouchDown */)
                gdi().setPenColor( _textColorHilight, _engine );
            else
                gdi().setPenColor( _textColor, _engine );
/*
			// added by bliss - hide extension name of .nds or .NDS
			if ( (gs().fileListType == 0) && gs().hideExtNDS && j == 2 ) { // SHOWNAME_COLUMN 
				strcpy(tmpfn, _rows[_firstVisibleRowId+i][j].text.c_str() );
				fnlen = strlen(tmpfn);

				if( fnlen > 4 && (strcasecmp(tmpfn+fnlen-4, ".nds") == 0 || strcasecmp(tmpfn+fnlen-4,".gba") == 0))
					tmpfn[fnlen-4] = 0;
				gdi().textOutRect( itemX, itemY, _columns[j].width, _rowHeight, tmpfn, _engine );
			}
			else
*/
            gdi().textOutRect( itemX, itemY,
                                _columns[j].width, _rowHeight,
                                _rows[_firstVisibleRowId+i][j].text.c_str(), _engine );
        }
    }
    //dbg_printf( "total %d _visible_row_count %d\n", total, _visible_row_count );
}

void cListView::selectRow( int id )
{
    if( 0 == _rows.size() ) return;

    if( id > (int)_rows.size() - 1 ) id = (int)_rows.size() - 1;
    if( id < 0 ) id = 0;

    //if( (int)_selectedRowId == id ) return;

    size_t lastVisibleRowId = _firstVisibleRowId + _visibleRowCount - 1;
    if( lastVisibleRowId > _rows.size() - 1 )
        lastVisibleRowId = _rows.size() - 1;
    if( id < (int)_firstVisibleRowId )
    {
        int offset = _selectedRowId - _firstVisibleRowId;
        scrollTo( id - offset );
    }
    if( id > (int)lastVisibleRowId )
    {
        int offset = _selectedRowId - _firstVisibleRowId;
        scrollTo( id - offset );
    }
    if( _selectedRowId != (u32)id ) {
        _selectedRowId = id;
        onSelectChanged( _selectedRowId );
        selectChanged( _selectedRowId );
    }

}

void cListView::scrollTo( int id )
{
    if( 0 == _rows.size() ) return;
    //if( _rows.size() < _visibleRowCount ) return;
    //dbg_printf("rows size %d, visibleRowCount %d\n", _rows.size(), _visibleRowCount );

    if( id > (int)_rows.size() - 1 )
        id = (int)_rows.size() - 1;
    if( id > (int)_rows.size() - (int)_visibleRowCount + 1)
        id = (int)_rows.size() - (int)_visibleRowCount + 1;
    if( id < 0 )
        id = 0;
    _firstVisibleRowId = id;
    onScrolled( id );
    scrolled( id );
    //dbg_printf("fvri %d, scroll_to %d\n", _first_visible_row_id, id );
}

cWindow& cListView::loadAppearance(const std::string& aFileName )
{
    return *this;
}

u32 cListView::rowBelowPoint( const cPoint & pt )
{
    if( windowRectangle().surrounds( pt ) ) {
        u32 row = _firstVisibleRowId + (pt.y - position().y) / _rowHeight;
        if( row > _rows.size() - 1)
            row = (u32)-1;
        return row;
    }
    return (u32)-1;
}

bool cListView::process( const cMessage & msg )
{
    bool ret = false;
    if(isVisible() && isEnabled())
    {
        //if( msg.id() > cMessage::keyMessageStart
        //    && msg.id() < cMessage::keyMessageEnd )
        //{
        //    ret = processKeyMessage( (cKeyMessage &)msg );
        //}
        if( msg.id() > cMessage::touchMessageStart
            && msg.id() < cMessage::touchMessageEnd )
        {
            ret = processTouchMessage( (cTouchMessage &)msg );
        }
    }

    return ret;
}

bool cListView::processTouchMessage( const cTouchMessage & msg )
{
    bool ret = false;

    static int sumOfMoveY = 0;

    if( msg.id() == cMessage::touchMove && isFocused() ) {
        if( abs(msg.position().y) > 0 ) {
            sumOfMoveY += msg.position().y;
        }
        if( sumOfMoveY > gs().scrollSpeed ) {
            selectNext();
            scrollTo( _firstVisibleRowId + 1 );
            sumOfMoveY = 0;
            _touchMovedAfterTouchDown = true;
        } else if( sumOfMoveY < -gs().scrollSpeed ) {
            selectPrev();
            scrollTo( _firstVisibleRowId - 1 );
            sumOfMoveY = 0;
            _touchMovedAfterTouchDown = true;
        }
        ret = true;
    }

    if( msg.id() == cMessage::touchUp ) {
        sumOfMoveY = 0;
        if( !_touchMovedAfterTouchDown ) {
            u32 rbp = rowBelowPoint( cPoint(msg.position().x,msg.position().y) );
            if( rbp != (u32)-1 ) {
                if( selectedRowId() == rbp ) {
                    onSelectedRowClicked( rbp );
                    selectedRowClicked( rbp );
                }
            }
        }
        _touchMovedAfterTouchDown = false;
        ret = true;
    }

    if( msg.id() == cMessage::touchDown ) {
        _touchMovedAfterTouchDown = false;
        u32 rbp = rowBelowPoint( cPoint(msg.position().x,msg.position().y) );
        if( rbp != (u32)-1 ) {
            selectRow( rbp );
        }
        ret = true;
    }

    return ret;
}


} // namespace akui
