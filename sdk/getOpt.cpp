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
	typedef vector < vstr > vvstr;
public:
	OPt(int argc, char **argv):_argc(argc), _argv(argv)
	{
	}
	OPt(int argc, char **argv, string optstr):_argc(argc), _argv(argv), _optStr(optstr)
	{
	}
public:
	bool getOpt();
	vvstr getOptArray();
	void showOptArray();
	bool setoptStr(const char *optstr);
	bool setMultioptStr();
	string OPtState2Str(OPtState State);

private:
	OPtState isOpt(char optChar);
	OPtState isMultiOpt(string optstr);
	bool getArgv(const char* opt, int &i);
	bool dealMixopt(char* argv,vstr& argvs);

private:
	int _argc;
	char **_argv;
	string _optStr;
	vvstr OptArray;
	vstr _multioptStr;
};

// Set the Option string
//@ optstr : base option string <= a:bcd:e
bool OPt::setoptStr(const char *optstr)
{
	if (!optstr)
		return false;
	_optStr = optstr;
	printf("You set option string is %s\n\n", optstr);
	return true;
}

// Set the multi Option string
bool OPt::setMultioptStr()
{
	return false;
	return true;
}

// The option char is right or not
//@ optChar : option characters,To determine whether in _optStr
OPtState OPt::isOpt(char optChar)
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
		State = isOpt(argv[i]);
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
// Get the option's arguments
//@ opt : option string <= -a
//@ i   : option's position
bool OPt::getArgv(const char* opt, int &i)
{
	if(!opt)
		return false;
	vector < string > argvlist;
	OPtState State = NOO;
	State = isOpt(opt[1]);
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
				OptArray.push_back(argvlist);
				printf("\t%s Have argumets : ",opt);
				for (int j = 1; j < argvlist.size();j++)
					printf("%s ", argvlist[j].c_str());
				printf("\n");
			}
			else
			{
				printf("\t[%s] You need argument, but Not gived...\n",opt);
			}
			break;
		case NnA:
			OptArray.push_back(argvlist);
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
OPt::vvstr OPt::getOptArray()
{
	return OptArray;
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
					if (_multioptStr.empty())
						printf("You are not set Multi-optionn String!\n");
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
							getArgv(mixOpt[j].c_str(),i);
						}
					}
					else
						printf("mix option error!!\n");
				}
				else			// -a
				{
					getArgv(_argv[i], i);
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
	for (i = 0; i < OptArray.size(); i++)
	{
		for (j = 0; j < OptArray[i].size(); j++)
			printf("%s\t", OptArray[i][j].c_str());
		printf("\n");
	}
}

int main(int argc, char **argv)
{
	if (argc < 2)
		printf("have val have member\n");
	OPt Opts(argc, argv);
	Opts.setoptStr("ab:cd:e");
	Opts.getOpt();
	printf("\n====================\n");
	Opts.showOptArray();
	OPt::vvstr tmp = Opts.getOptArray();

	return 0;
}
