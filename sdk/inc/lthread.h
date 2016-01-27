#ifndef _LTHREAD_H_
#define _LTHREAD_H_
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#include "Basethread.h"
#include "confdata.h"
#include "lmutex.h"
#include "Mdef.h"
#include "filedata.h"
using namespace std;
class lthread:public Basethread
{

	private:
		bool renameflag;
		bool breakflag;
		logconf _conf;
		lmutex* _logfmutex;
		filedata* _filedata;
	private:
		static lthread *_linstance;
		lthread(logconf& conf);
		lthread(const lthread &);
		lthread& operator = (const lthread &);
		class CGarbo   //它的唯一工作就是delete _linstance
		{
			public:
				~CGarbo()
				{
					if(lthread::_linstance)
					{
						(lthread::_linstance)->freedata();
						cout << "log thread will deading" << endl;
						delete lthread::_linstance;
						lthread::_linstance = NULL;
					}
				}
		};
		static CGarbo garbo;
	public:
		static lthread* getlthread(logconf& conf);
		friend class logT;
		//~lthread();
		int run();
		float getfsize(FSU& fsu);
		bool checkffull();
		bool checklogfnum(string logpath);
		bool addnewf(lsfile_t& newlogf);
		void freedata();
};

#endif
// lthread.h
