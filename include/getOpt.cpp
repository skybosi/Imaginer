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
#define ECc "-" //Explaincharacters
#define SCMP(_V) (!strncmp(_V,ECc,1))

	public:
		typedef vector<string> vstr;
		typedef vector<vstr>   vvstr;
	public:
		OPt(int argc,char** argv):
			_argc(argc),_argv(argv){}
		OPt(int argc,char** argv,string optstr):
			_argc(argc),_argv(argv),_optStr(optstr){}
	public:
		bool getOpt();
		vvstr getOptArray();
		void showOptArray();
		bool setoptStr(const char* optstr);
		bool setMultioptStr();
		string OPtState2Str(OPtState State);
	private:
		bool delArgv();
		OPtState isOpt(char optChar);
		OPtState isMultiOpt(string optstr);
		bool getArgv(OPtState State,int& i);
	private:
		int _argc;
		char** _argv;
		string _optStr;
		vvstr OptArray;
		vstr  _multioptStr;
};
//Set the Option string
bool OPt::setoptStr(const char* optstr)
{
	if(!optstr)
		return false;
	_optStr = optstr;
	printf("You set option string is %s\n",optstr);
	return true;
}
//Set the multi Option string
bool OPt::setMultioptStr()
{
	return false;
	return true;
}
//The option char is right or not
OPtState OPt::isOpt(char optChar)
{
	int i = 0;
	int optstrl = _optStr.size();
	while(i < optstrl)
	{
		if(_optStr[i] == optChar)
		{
			if(_optStr[i+1] == ':')
				return MnA;
			else
				return NnA;
		}
		else
			i++;
	}
	return InA;
}
//Get the option's arguments
bool OPt::getArgv(OPtState State,int& i)
{	
	bool flag = false;
	vector<string> argvlist;
	argvlist.push_back(_argv[i]);
	//获取所有参数
	while (i + 1 < _argc && !SCMP(_argv[i + 1]))
	{
		flag =  true;
		i++;
		if(State == MnA)
		{
			argvlist.push_back(_argv[i]);
			printf("\ti:%d %s\n", i, _argv[i]);
		}
		else
			printf("\tHave a spare part, will be discarded ... i:%d %s\n", i, _argv[i]);
	}
	if(flag)
		OptArray.push_back(argvlist);
	else
	{
		i++;
		argvlist.pop_back();
		printf("Need argument,but not give!\n");
	}
	return flag;
}
OPt::vvstr OPt::getOptArray()
{
	return OptArray;
}
//del the OptArray top
bool OPt::delArgv()
{
	if(OptArray.empty())
		return false;
	OptArray.pop_back();
	return true;
}
//Get the all Option and it's arguments
bool OPt::getOpt()
{
	if(_optStr.empty())
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
	OPtState State = NOO;
	for (; i < _argc; i++)
	{
		if (SCMP(_argv[i]))
		{
			printf("i:%d %s ", i, _argv[i]);
			int argvl = strlen(_argv[i]);
			if (argvl > 1)
			{
				if (_argv[i][1] == '-')//--
				{
					if(_multioptStr.empty())
						printf("You are not set Multi-optionn String!\n");
				}
				else if (argvl > 2)//-ab
				{
					int j = 1;
					int mnANum = 0;
					while(j < argvl)
					{
						State = isOpt(_argv[i][j]);
						if(State == MnA)
						{
							mnANum++;
							if(mnANum > 1)
							{
								printf("mix option error!!\n");
								break;
							}
						}
						printf("OPtState:%s\n",OPtState2Str(State).c_str());
						j++;
					}
					if(mnANum <= 1)
						getArgv(State,i);
				}
				else//-a
				{
					State = isOpt(_argv[i][1]);
					printf("OPtState:%s\n",OPtState2Str(State).c_str());
					getArgv(State,i);
				}
			}
			else
			{
				printf("help!\n");
			}
		}
		else
		{
			printf("无效参数：i:%d %s\n", i, _argv[i]);
		}
	}
	return true;
}

std::string OPt::OPtState2Str(OPtState State)
{
	switch(State)
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
	int i ,j;
	for(i = 0; i < OptArray.size(); i++)
	{
		for(j = 0; j < OptArray[i].size(); j++)
			printf("%s\t",OptArray[i][j].c_str());
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
	printf("====================\n");
	Opts.showOptArray();
	OPt::vvstr tmp = Opts.getOptArray(); 
	return 0;
}
