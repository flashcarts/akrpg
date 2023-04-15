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









#ifndef _MAINLIST_H_
#define _MAINLIST_H_

#include <nds.h>
#include "listview.h"
#include "startmenu.h"
#include "sigslot.h"
#include "keymessage.h"
#include "touchmessage.h"
#include "dsrom.h"
#include "zoomingicon.h"

// 显示游戏列表，文件列表等等
class cMainList : public akui::cListView
{
public:

    enum VIEW_MODE
    {
        VM_LIST = 0,
        VM_ICON
    };

    enum //COLUMN_LIST
    {
        INDEX_COLUMN = 0,
        ICON_COLUMN = 1,
        SHOWNAME_COLUMN = 2,
        REALNAME_COLUMN = 3,
        DISPNAME_COLUMN = 4,
        SAVETYPE_COLUMN = 5,
        FILESIZE_COLUMN = 6
		/*
        SAVETYPE_COLUMN = 4,
        FILESIZE_COLUMN = 5
		*/
    };

public:

    cMainList( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    ~cMainList();

public:

    int init();

    bool enterDir( const std::string & dirName );

    void backParentDir();

    void refresh();

    std::string getCurrentDir();

    bool getRomInfo( u32 rowIndex, DSRomInfo & info ) const;

    void setRomInfo( u32 rowIndex, const DSRomInfo & info );

    void setViewMode( VIEW_MODE mode );

    std::string getSelectedFullPath();

    std::string getSelectedShortPath();

    std::string getSelectedLFN();

    VIEW_MODE getViewMode() { return _viewMode; }

    void arrangeIcons();

    akui::Signal1< u32 > selectedRowHeadClicked;

    akui::Signal0 directoryChanged;

protected:

    void draw();

    void drawIcons();                        // 直接画家算法画 icons

    enum {
        POSITION = 0,
        CONTENT = 1
    };

    void updateActiveIcon( bool updateContent );  // 更新活动图标的坐标等等

protected:

    void onSelectedRowClicked( u32 index );

    void onSelectChanged( u32 index );

    void onScrolled( u32 index );

protected:

    VIEW_MODE _viewMode;

    std::string _currentDir;

    std::vector< std::string > _extnameFilter;

    std::vector< DSRomInfo > _romInfoList;

    cZoomingIcon _activeIcon;

    //cSprite _activeIcon;

    float _activeIconScale;
};


#endif//_MAINLIST_H_
