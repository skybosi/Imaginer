#ifndef __GETOPT_H__
#define __GETOPT_H__
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <string.h>
using namespace std;
enum OPtState
{
	MnA,    //Must need argument
	NnA,    //not need argument
	InA,    //invalid argument
	NOO     //Unknown Error
};
class OPt
{
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
	public:
		OPt(int argc, char **argv) :_argc(argc), _argv(argv) { }
		OPt(int argc, char **argv, string optstr) :_argc(argc), _argv(argv), _optStr(optstr) { }
		virtual ~OPt() {}
	public:
		bool getOpt();
		void showOptArray();
		bool setoptStr(const char *optstr);
		bool setMultioptStr(const char *Multioptstr);
		vvstr getOptSarry() { return _singleoptArray; }
		mvstr getOptMarry() { return _multioptArray; }
		virtual void setFunction() = 0;
		virtual void manual() = 0;

	private:
		OPtState isSingleOpt(char optChar);
		OPtState isMultiOpt(string optstr, int& pos);
		bool getSingleArgv(const char* opt, int &i);
		bool getMultiArgv(const char* opt, int &i);
		bool dealMixopt(char* argv, vstr& argvs);
		int isExist(string optstr);//test a optstr is exist or not,return the position,if not exist return -1
		string OPtState2Str(OPtState State);

	private:
		int _argc;
		char **_argv;
		string _optStr;
		string _moptStr;
		vvstr _singleoptArray; //single option's arguments
		mvstr _multioptArray;//Mulit option's arguments
};

#endif 
