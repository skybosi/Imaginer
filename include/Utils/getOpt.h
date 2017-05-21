#ifndef __GETOPT_H__
#define __GETOPT_H__
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include <stdlib.h>
using namespace std;
class OPt
{
enum OPtState
{
	MnA,    //Must need argument
	NnA,    //not need argument
	InA,    //invalid argument
	NOO     //Unknown Error
};
#define ECc "-"    // Explain characters
#define SCMP(_V) (!strncmp(_V,ECc,1))
	public:
		typedef vector < string > vstr;
		struct MultiOption
		{
			string longOpt;
			char   aliasChar;
			vstr   margv;
		};
		typedef vector < MultiOption > mvstr;//record Mulit option's arguments
		typedef vector < vstr > vvstr; //record single option's arguments
typedef void (*FUNCTION) (vvstr sopt, mvstr mopt);
	public:
		OPt(int argc, char **argv, char* mans = NULL) :_argc(argc), _argv(argv),_man(mans) { }
		OPt(int argc, char **argv, string optstr, char* mans = NULL) :_argc(argc), _argv(argv), _optStr(optstr), _man(mans) { }
		virtual ~OPt() {}
	public:
		bool getOpt();
		void showOptArray();
		bool setoptStr(const char *optstr);
		bool setMultioptStr(const char *Multioptstr);
		vvstr getOptSarry() { return _singleoptArray; }
		mvstr getOptMarry() { return _multioptArray; }
		void handle(FUNCTION handler);
		void manual(char* mans) { _man = mans;}
		char* manual() { return _man;}
	public:
		static char* tostr(string str) {return (char*)str.c_str();}
		static int   toint(string str) {return atoi((char*)str.c_str());}
		static long  tolong(string str) {return atol((char*)str.c_str());}
		static float tofd(string str) {return atof((char*)str.c_str());}
		static char  tochar(string str) {return str[0];}
		static char  tobool(string str) {if(str=="0")return false;return true;}
	private:
		OPtState isSingleOpt(char optChar);
		OPtState isMultiOpt(string optstr, int& pos);
		bool getSingleArgv(const char* opt, int &i);
		bool getMultiArgv(const char* opt, int &i);
		bool dealMixopt(char* argv, vstr& argvs);
		int isExist(string optstr);//test a optstr is exist or not,return the position,if not exist return -1
		string OPtState2Str(OPtState State);

	private:
		int    _argc;
		char** _argv;
		string _optStr;
		string _moptStr;
		vvstr  _singleoptArray; //single option's arguments
		mvstr  _multioptArray;//Mulit option's arguments
		char*  _man;
};

#endif 
