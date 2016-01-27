#ifndef _PARSECONF_H_
#define _PARSECONF_H_
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <ctype.h>
#include <cstdlib>
#include <dirent.h>
#include <unistd.h>
#include <typeinfo>
#include <fstream>
#include "Mdef.h"
#include "confdata.h"
using namespace std;
string del_coment(string & soustr, string head, string tail = "", bool way = 1);
bool isblank(const char *str);	// 是否可见字符
string & trim(string & s);
class parseconf
{
	friend class logT;
	private:
		ofstream _default_config_file;
		string _path;				// configuration file's path
		ifstream _confile;			// configuration file's file stream
		strm _kws;					// key-value with map
		logconf _lconf;
		bool set(INT & key, string keyname);
		bool set(LONG & key, string keyname);
		bool set(DOBL & key, string keyname);
		bool set(FLOT & key, string keyname);
		bool set(string & key, string keyname);
		void resetlsize();
		bool create_config_file();
	public:
		parseconf(string path);
		parseconf();
		~parseconf()
		{
			cout << "confile will close..." << endl;
			_confile.close();
		}
		bool getfkwtab();
		bool parse_conf();
		void setconf();
};

#endif
// parseconf.h _PARSECONF_H_
