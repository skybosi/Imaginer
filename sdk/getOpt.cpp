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
	OPt(int argc, char **argv):_argc(argc), _argv(argv)
	{
	}
	OPt(int argc, char **argv, string optstr):_argc(argc), _argv(argv), _optStr(optstr)
	{
	}
public:
	bool getOpt();
	bool getMultiArgv(const char* opt, int &i);
	void showOptArray();
	bool setoptStr(const char *optstr);
	bool setMultioptStr(const char *Multioptstr);
	string OPtState2Str(OPtState State);

	OPtState isMultiOpt(string optstr,int& pos);
private:
	OPtState isSingleOpt(char optChar);
	bool getSingleArgv(const char* opt, int &i);
	bool dealMixopt(char* argv,vstr& argvs);

private:
	int _argc;
	char **_argv;
	string _optStr;
	string _moptStr;
	vvstr _singleoptArray; //single option's arguments
	mvstr _multioptArray;//Mulit option's arguments
};

// Set the Option string
//@ optstr : base option string <= a:bcd:e
bool OPt::setoptStr(const char *optstr)
{
	if (!optstr)
		return false;
	_optStr = optstr;
	printf("You set Single-option string is %s\n\n", optstr);
	return true;
}

// Set the multi Option string
bool OPt::setMultioptStr(const char *Multioptstr)
{
	if(!Multioptstr)
		return false;
	_moptStr = Multioptstr;
	printf("You set Multi-option string is %s\n\n", Multioptstr);
	return true;
}

// The single-option char is right or not
//@ optChar : option characters,To determine whether in _optStr
OPtState OPt::isSingleOpt(char optChar)
{
	int i = 0;
	int optstrl = _optStr.size();
	while (i < optstrl)
	{
		if (_optStr[i] == optChar)
		{
			if (_optStr[i + 1] == ':')
				return MnA;
			else
				return NnA;
		}
		else
			i++;
	}
	return InA;
}
//The Multi-option string is right or not
//@optstr: option string,To determine whether in _moptStr
//@pos   : the postion at _moptStr
OPtState OPt::isMultiOpt(string optstr,int& pos)
{
	if(optstr.substr(0,2) != "--")
		return InA;
	optstr = optstr.substr(2);
	string options = optstr + "_";
	pos = _moptStr.find(options);
	if(pos != _moptStr.npos)
	{
		size_t oppos = pos + options.size();
		if( oppos != _moptStr.npos)
		{
			if(_moptStr[oppos+1] == ':' || _moptStr[oppos+2] == ':')
				return MnA;
			else;
				return NnA;
		}
	}
	return InA;
}
//analyze the mix options to simple option <= -ab > -a -b
//@argv  : mix options string
//@argvs : after deal with option
bool OPt::dealMixopt(char* argv,vstr& argvs)
{
	if(!argv)
		return false;
	int argvl = strlen(argv);
	int i = 1;
	int mnAnum = 0;
	OPtState State;
	string tmp;
	while (i < argvl)
	{
		tmp = "-";
		State = isSingleOpt(argv[i]);
		if(State != InA)
		{
			tmp += argv[i];
			if(State == MnA)
			{
				mnAnum++;
				if(mnAnum > 1)
					return false;
			}
			argvs.push_back(tmp);
			//printf("%s ",tmp.c_str());
		}
		i++;
	}
	return true;
}
// Get the single option's arguments
//@ opt : option string <= -a
//@ i   : option's position
bool OPt::getSingleArgv(const char* opt, int &i)
{
	if(!opt)
		return false;
	vector < string > argvlist;
	OPtState State = NOO;
	State = isSingleOpt(opt[1]);//skip ECc -
	argvlist.push_back(opt);
	while (State != InA && i + 1 < _argc && !SCMP(_argv[i + 1]))
	{
		if(State == MnA)
		{
			i++;
			argvlist.push_back(_argv[i]);
		}
		else //State == NnA
		{
			printf("\t[%s] Have spare part, will be ignored...\n",opt);
			break;
		}
	}
	switch(State)
	{
		case MnA:
			if (argvlist.size() > 1)
			{
				_singleoptArray.push_back(argvlist);
				printf("\t[%s] Have argumets : ",opt);
				for (int j = 1; j < argvlist.size();j++)
					printf("%s ", argvlist[j].c_str());
				printf("\n");
			}
			else
			{
				printf("\tSingle-Option [%s] need argument, but Not gived...\n",opt);
			}
			break;
		case NnA:
			_singleoptArray.push_back(argvlist);
			break;
		case InA:
			printf("\t[%s] Invalid option, will be ignored...\n", opt);
			break;
		default:
			break;
	}
	return true;
}

//Get the multi option array
bool OPt::getMultiArgv(const char* opt, int &i)
{
	if(!opt)
	{
		return false;
	}
	OPtState State = NOO;
	int pos = -1;
	State = isMultiOpt(opt,pos);
	if(State == InA)
	{
		printf("\tInvalid Multi-option, will be ignored\n");
		return false;
	}
	MultiOption mopt;
	mopt.longOpt = opt;
	pos = _moptStr.find_first_of("|",pos);
	char alias_char = '\0';
	switch(State)
	{
		case MnA:
			if(pos != _moptStr.npos)
			{
				alias_char = _moptStr[pos-2];//MnA ,-2 get alais Char
			}
			else
			{
				printf("MnA only one Muilt option\n");
				alias_char = _moptStr[_moptStr.size()-2];//MnA ,-2 get alais Char
			}
			//get arguments
			i++;
			printf("\t[%s] Have argumets : ",opt);
			while (isMultiOpt(_argv[i],pos) > NnA && i+1  < _argc )
			{
				printf("%s ",_argv[i]);
				mopt.margv.push_back(_argv[i]);
				i++;
			}
			printf("\n");
			if(mopt.margv.size() < 1)
			{
				printf("\tMulti-Option [%s] need argument, but Not gived...\n",opt);
			}
			i--;
			break;
		case NnA:
			if(pos != _moptStr.npos)
			{
				alias_char = _moptStr[pos-1];//NnA ,-1 get alais Char
			}
			else
			{
				printf("NnA only one Muilt option\n");
				alias_char = _moptStr[_moptStr.size()-1];//NnA ,-1 get alais Char
			}
			break;
		default:
			break;
	}
	if( alias_char != '_')
	{
		if(isSingleOpt(alias_char) > NnA)
			mopt.aliasChar = alias_char;
		else
		{
			mopt.aliasChar = '\0';
			printf("\t[%s]:[%c] Multi-option alias is Fair,Because the char is use at single Option!\n",opt,alias_char);
		}
	}
	else
	{
		mopt.aliasChar = '\0';
		printf("\t[%s]:[ ] Multi-option alias is Fair,Because You not set for this Long option!\n",opt);
	}
	_multioptArray.push_back(mopt);
	return true;
}

// Get the all Option and it's arguments
bool OPt::getOpt()
{
	if (_optStr.empty())
	{
		printf("please set base option string!\n");
		return false;
	}
	if (_argc < 2)
	{
		printf("please chose option!\n");
		return false;
	}
	int i = 1;
	for (; i < _argc; i++)
	{
		if (SCMP(_argv[i]))
		{
			int argvl = strlen(_argv[i]);
			if (argvl > 1)
			{
				if (_argv[i][1] == '-')	// --
				{
					printf("\t[%s] is Multi-option\n",_argv[i]);
					if (_moptStr.empty())
						printf("You are not set Multi-option String!\n");
					else
						getMultiArgv(_argv[i],i);
				}
				else if (argvl > 2)	// -ab
				{
					vstr mixOpt;
					int j = 0;
					printf("Mix options :%s\n", _argv[i]);
					if(dealMixopt(_argv[i],mixOpt))
					{
						for (; j < mixOpt.size(); j++)
						{
							getSingleArgv(mixOpt[j].c_str(),i);
						}
					}
					else
						printf("mix option error!!\n");
				}
				else			// -a
				{
					getSingleArgv(_argv[i], i);
				}
			}
			else
			{
				printf("[%s] dafault help option!\n",_argv[i]);
			}
		}
		else
		{
			printf("[%s] Invalid argument, will be ignored...\n",_argv[i]);
		}
	}
	return true;
}

std::string OPt::OPtState2Str(OPtState State)
{
	switch (State)
	{
	case MnA:
		return "MnA";
		break;
	case NnA:
		return "NnA";
		break;
	case InA:
		return "InA";
		break;
	default:
		return "NOO";
		break;
	}
}

void OPt::showOptArray()
{
	int i, j;
	for (i = 0; i < _singleoptArray.size(); i++)
	{
		printf("[%s] Single-option\n", _singleoptArray[i][0].c_str());
		for (j = 0; j < _singleoptArray[i].size(); j++)
			printf("\t%s\t", _singleoptArray[i][j].c_str());
		printf("\n");
	}
	for (i = 0; i < _multioptArray.size(); i++)
	{
		printf("[%s] Multi-option\n\tMulti-option's aliasChar: %c\n", _multioptArray[i].longOpt.c_str(),_multioptArray[i].aliasChar);
		if(!_multioptArray[i].margv.empty())
		{
			printf("\t[%s] Multi-option's argumet: ",_multioptArray[i].longOpt.c_str());
			for (j = 0; j < _multioptArray[i].margv.size(); j++)
				printf("%s\t", _multioptArray[i].margv[j].c_str());
		}
		printf("\n");
	}
}

int main(int argc, char **argv)
{
	if (argc < 2)
		printf("have val have member\n");
	OPt Opts(argc, argv);
	Opts.setoptStr("ab:cd:e");
	Opts.setMultioptStr("help_h|add_a:|");
	Opts.getOpt();
	printf("\n====================\n");
	Opts.showOptArray();
	return 0;
}
