#ifndef _FILEDATA_H_
#define _FILEDATA_H_
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#include <vector>
#include <deque>
#include <algorithm>
#include "confdata.h"
#include "Mdef.h"
using namespace std;
class filedata
{
	private:
		deque < lsfile_t > _files;
		logconf _conf;
	private:
		string & delsuffix(string & filepath);
		static bool sortbyfilename( const lsfile_t &v1, const lsfile_t &v2);
	public:
		bool rmflag;		//true:  delete go on;
										//false: delete finish 
		filedata(logconf& conf);
		~filedata();
		bool init_files();
		int getlflist(const char *dir,const char *ptfname);
		bool checkfname();
		bool addnewfile(lsfile_t& newlogf);
		void showalll(deque < lsfile_t > files)const;
};

#endif
//filedata.h
