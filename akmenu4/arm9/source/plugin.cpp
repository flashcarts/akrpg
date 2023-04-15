
#include "plugin.h"
#include "inifile.h"
#include "systemfilenames.h"
#include "brightnesssetting.h"
#include "romloader.h"

#include <sys/stat.h>
#include "dbgtool.h" 

cPlugin::cPlugin()
{
	extNames = "";
}

cPlugin::~cPlugin() {
}

bool cPlugin::loadPlugin(const std::string &filename) {
	std::string extName, plugin, path;
	size_t lastDotPos = filename.find_last_of( '.' );

	dbg_printf("LoadPlugin: %s\n", filename.c_str());
	if( filename.npos != lastDotPos )
		extName = filename.substr( lastDotPos + 1 );
	else 
		return false;


	CIniFile ini;
    if ( !ini.LoadIniFile( SFN_PLUGIN_SETTINGS ) ) 
		return false;

	for( size_t i = 0; i < extName.size(); ++i )
		extName[i] = tolower( extName[i] );
	dbg_printf("extName: %s\n", extName.c_str());

	plugin = ini.GetString("Association",extName.c_str(),"");
	dbg_printf("Plugin: %s\n", plugin.c_str());
	if ( plugin == "" ) return false;

	path = SFN_PLUGIN_DIRECTORY;
	path = path + plugin;
	struct stat st;
	dbg_printf("Plugin: %s\n", path.c_str());
	if(-1==stat(path.c_str(),&st)) { return false; }

	CIniFile fi( SFN_LAST_SAVEINFO ) ;
	fi.SetString("Save Info","Plugin",filename);
	fi.SetString( "Save Info", "shortPathNDS", "");
	fi.SaveIniFile( SFN_LAST_SAVEINFO );
    loadRom( path );

	return true;
}

void cPlugin::loadSettings()
{
    CIniFile ini( SFN_PLUGIN_SETTINGS );
	extNames = ini.GetString("system", "Files", extNames);
}

