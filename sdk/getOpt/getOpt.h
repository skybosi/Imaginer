#ifndef __GETOPT_H__
#define __GETOPT_H__
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h>
using namespace std;
class OPt
{
private:
	static int type(const char* str)
	{
		/*
		   char   c;  //type: 0
		   char*  p;  //type: 1
		   int    i;  //type: 2
		   long   l;  //type: 3
		   float  f;  //type: 4
		   double d;  //type: 5
		   */
		if(strlen(str) == 1){
			if(str[0] >= '0' && str[0] <= '9')
				return 2;
			else
				return 0;
		}else if(str[0] >= '0' && str[0] <= '9')
		{
			if(strchr(str, '.') == NULL)
			{
				return 2;
			}
			return 4;
		}else
		{
			return 1;
		}
		return -1;
	}
public:
	struct argv
	{
		private:
			char* _opt;
			int   _type;
		public:
			argv():_opt(NULL), _type(-1){}
			argv(const char* arg):_opt((char*)arg) { _type = type(arg); }
			argv(const string& arg){argv(arg.c_str());}
			argv(const argv& rhs)
			{
				_type = rhs._type;
				_opt = (char*) malloc(strlen(rhs._opt) * sizeof(char));
				strcpy(_opt, rhs._opt);
			}
			inline operator char()   { return _opt[0];}
			inline operator int()    { return atoi(_opt);}
			inline operator long()   { return atol(_opt);}
			inline operator float()  { return atof(_opt);}
			inline operator double() { return atof(_opt);}
			inline operator char*()  { return _opt;}
			inline operator string() { return _opt;}
		public:
			argv& operator =(const argv& rhs)
			{
				if(this != &rhs)
				{
					_opt  = rhs._opt;
					_type = rhs._type;
				}
				return *this;
			}
			/*
			   argv& operator =(const string& rhs)
			   {
			   argv rt(rhs);
			   return rt;
			   }
			*/
			bool operator==(const argv& rhs)
			{
				return (_type == rhs._type) && (0 == strcmp(_opt, rhs._opt));
			}
			bool operator==(const char* rhs)
			{
				argv rt(rhs);
				return (*this == rt);
			}
			bool operator==(const string& rhs)
			{
				return (*this == rhs.c_str());
			}
			const char* toString(){return _opt;}
			char operator[] (size_t i)const
			{
				return ((i < strlen(_opt)) ? _opt[i] : '\0');
			}
			friend std::ostream& operator<< (std::ostream& os, const argv& rhs)
			{
				return os << "data: " << rhs._opt << " type:" << rhs._type;
			}
	};
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
        typedef vector < argv > vargv;
		struct MultiOption
		{
			string longOpt;
			char   aliasChar;
            vargv   margv;
		};
typedef vector < MultiOption > mvargv;//record Mulit option's arguments
typedef vector < vargv > vvargv; //record single option's arguments
typedef void (*FUNCTION) (const OPt& );
	public:
		OPt(int argc, char **argv, char* mans = NULL) :_argc(argc), _argv(argv),_man(mans) { }
		OPt(int argc, char **argv, string optstr, char* mans = NULL) :_argc(argc), _argv(argv), _optStr(optstr), _man(mans) { }
		virtual ~OPt() {}
	public:
		bool getOpt();
		void showOptArray();
		bool setoptStr(const char *optstr);
		bool setMultioptStr(const char *Multioptstr);
		vvargv getOptSarry() { return _singleoptArray; }
		mvargv getOptMarry() { return _multioptArray; }
		void handle(FUNCTION handler);
		void manual(char* mans) { _man = mans;}
		char* manual() { return _man;}
	public:
		inline char  option(int i)const { return _singleoptArray.at(i).at(0)[1];}
		inline vargv argvs(int i)const { return _singleoptArray.at(i);}
		inline size_t ssize()const {return _singleoptArray.size();}
		inline size_t msize()const {return _multioptArray.size();}
	private:
		OPtState isSingleOpt(char optChar);
		OPtState isMultiOpt(string optstr, int& pos);
		bool getSingleArgv(const char* opt, int &i);
		bool getMultiArgv(const char* opt, int &i);
		bool dealMixopt(char* argv, vargv& argvs);
		int isExist(string optstr);//test a optstr is exist or not,return the position,if not exist return -1
		string OPtState2Str(OPtState State);
	private:
		int    _argc;
		char** _argv;
		string _optStr;
		string _moptStr;
		vvargv  _singleoptArray; //single option's arguments
		mvargv  _multioptArray;//Mulit option's arguments
		char*  _man;
};

#endif 
