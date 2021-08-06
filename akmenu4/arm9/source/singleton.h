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









#ifndef _SINGLETON_H_
#define _SINGLETON_H_
#include <cstdlib>

template< class T >
class t_singleton
{

public:

    static T & instance()
    {
        if( NULL == _instance )
        {
            create_instance();
            //atexit( release_instance );
            // t_singleton 他自己是不会有实例的，无法利用析构函数来释放内存
            // 所以利用 atexit 来在整个程序退出时释放内存
        }
        return *_instance;
    }

private:

    static void create_instance()
    {
        if( NULL == _instance )
        {
            _instance = new T();
        }
    }

    static void release_instance()
    {
        if( NULL != _instance )
        {
            delete _instance;
            _instance = NULL;
        }
    }

private:

    static T * _instance;
};

template< class T >
T * t_singleton< T >::_instance = NULL;

#endif //_AGL_SINGLETON_H_
