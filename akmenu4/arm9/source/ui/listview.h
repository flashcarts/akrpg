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









#ifndef _AKUI_LISTVIEW_H_
#define _AKUI_LISTVIEW_H_

#include <string>
#include <vector>
#include "window.h"

namespace akui
{
class cListColumn
{
public:
    cListColumn() : index(0), width(0), offset(0), text("") {}

    u32            index;
    u16            width;
    u16            offset;
    std::string text;
};

class cListItem
{
public:
    cListItem() : index(0), column(0), imageId(0), param(0), text("") {}
    u32            index;
    u32            column;
    u32            imageId;
    u32            param;
    std::string    text;
};

class cListView : public cWindow
{
public:

    cListView( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    virtual ~cListView() {}

    typedef std::vector< cListItem > itemVector; // a column
public:

    void draw();

    bool insertColumn( size_t index, const std::string & text, u8 width );

    bool insertRow( size_t index, const std::vector< std::string > & text );

    void removeRow( size_t index ); // return id of the item next to the removed item

    void removeAllRows();

    u32 getRowCount() { return _rows.size(); }

    void setRowHeight( u16 height ) { _rowHeight = height; setListViewSize( _size.x, _size.y ); }

    void setListViewSize( u16 w, u16 h ) {
        setSize( cSize( w, h ) );
        _visibleRowCount = h / _rowHeight;
        //if( _visibleRowCount > _rows.size() ) _visibleRowCount = _rows.size();
    }

    void arangeColumnsSize();

    void selectRow( int id );

    u32 selectedRowId() { return _selectedRowId; }

    void selectNext() { selectRow( _selectedRowId + 1 ); }

    void selectPrev() { selectRow( _selectedRowId - 1 ); }

    u32 rowBelowPoint( const cPoint & pt );

    void scrollTo( int id );

    u32 visibleRowCount() { return _visibleRowCount; }

    cWindow& loadAppearance(const std::string& aFileName );

    bool process( const akui::cMessage & msg );

    Signal1< u32 > selectedRowClicked;

    Signal1< u32 > selectChanged;

    Signal1< u32 > scrolled;


protected:

    virtual void onSelectChanged( u32 index ) {}

    virtual void onSelectedRowClicked( u32 index ) {}

    virtual void onScrolled( u32 index ) {}

    bool processTouchMessage( const akui::cTouchMessage & msg );

protected:

    void setFirstVisibleId();

    void drawFrame();

    void drawSelectionBar();

    void drawText();

protected:

    std::string    _text;
    u16        _rowHeight;
    u16        _textColor;
    u16        _textColorHilight;
    u16        _selectionBarColor1;
    u16        _selectionBarColor2;

    u32        _selectedRowId;
    u32        _firstVisibleRowId;
    u32        _visibleRowCount;
    u16        _rowsPerpage;
    bool    _touchMovedAfterTouchDown;

    std::vector< cListColumn >        _columns;
    std::vector< itemVector >        _rows;
};

}
#endif//_LISTVIEW_H_
