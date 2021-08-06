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









#ifndef _AKUI_POINT_H_
#define _AKUI_POINT_H_

#include <algorithm>

namespace akui
{
    template <typename T>
    class t_point
    {
    public:
        typedef T value_type;
        value_type x;
        value_type y;

        t_point():x(),y(){}
        t_point(value_type x,value_type y) :x(x),y(y) {}
        t_point(const t_point &p) { x=p.x,y=p.y; }
        t_point operator+(const t_point &p) const { return t_point(x+p.x,y+p.y); }
        t_point operator-(const t_point &p) const { return t_point(x-p.x,y-p.y); }
        bool operator==(const t_point &p) const { return (x==p.x) && (y==p.y); }
        bool operator!=(const t_point &p) const { return (x!=p.x) || (y!=p.y); }
        t_point& operator+=(const t_point &p) { x+=p.x,y+=p.y; return *this; }
        t_point& operator-=(const t_point &p) { x-=p.x,y-=p.y; return *this; }
        t_point& operator=(const t_point &p) { x=p.x,y=p.y; return *this; }
        //当前点是否在p的左侧
        bool is_left(const t_point &p) const { return x<p.x; }                            // 是否在点的左边
        //当前点是否在p的右侧
        bool is_right(const t_point &p) const { return x>p.x; }                            // 是否在点的右边
        //当前点是否在p的上方
        bool is_up(const t_point &p) const { return y<p.y; }                                // 是否在点的上边
        //当前点是否在p的下方
        bool is_down(const t_point &p) const { return y>p.y; }                            // 是否在点的下边
        t_point& operator () (value_type x_,value_type y_){x=x_;y=y_;return *this;}
    };

    //point
    typedef t_point<int> cPoint;
    typedef t_point<float> cPointf;
    //size
    typedef t_point<int> cSize;

}


#endif //_AKUI_POINT_H_

