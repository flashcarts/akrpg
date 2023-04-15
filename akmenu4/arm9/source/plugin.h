

#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#define SCROLL_FAST 4
#define SCROLL_MEDIUM 10
#define SCROLL_SLOW 16

#include <nds.h>
#include <string>
#include "singleton.h"

class cPlugin
{

public:

    cPlugin();
    ~cPlugin();

public:

    void loadSettings();
	bool loadPlugin( const std::string &filename);

public:
	std::string extNames;

private:
};


typedef t_singleton< cPlugin > plugin_s;
inline cPlugin & plugin() { return plugin_s::instance(); }

#endif//_PLUGIN_H_
