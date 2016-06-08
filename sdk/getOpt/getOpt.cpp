#include "getOpt.h"
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
	if (!Multioptstr)
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
OPtState OPt::isMultiOpt(string optstr, int& pos)
{
	if (optstr.substr(0, 2) != "--")
		return InA;
	optstr = optstr.substr(2);
	string options = optstr + "_";
	pos = _moptStr.find(options);
	if (pos != (int)_moptStr.npos)
	{
		size_t oppos = pos + options.size();
		if (oppos != _moptStr.npos)
		{
			if (_moptStr[oppos + 1] == ':' || _moptStr[oppos + 2] == ':')
				return MnA;
			else;
			return NnA;
		}
	}
	return InA;
}
int OPt::isExist(string optstr)//test a optstr is exist or not
{
	size_t i = 0;
	if (optstr[1] == '-') //long option
	{
		for (; i < _multioptArray.size(); i++)
		{
			if (optstr == _multioptArray[i].longOpt)
			{
				return i;
			}
		}
	}
	else
	{
		for (; i < _singleoptArray.size(); i++)
		{
			if (optstr == _singleoptArray[i][0])
			{
				return i;
			}
		}
	}
	return -1;
}
//analyze the mix options to simple option <= -ab > -a -b
//@argv  : mix options string
//@argvs : after deal with option
bool OPt::dealMixopt(char* argv, vstr& argvs)
{
	if (!argv)
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
		if (State != InA)
		{
			tmp += argv[i];
			if (State == MnA)
			{
				mnAnum++;
				if (mnAnum > 1)
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
	if (!opt)
		return false;
	vector < string > argvlist;
	OPtState State = NOO;
	State = isSingleOpt(opt[1]);//skip ECc -
	argvlist.push_back(opt);
	int Epos = -1;
	while (State != InA && i + 1 < _argc && !SCMP(_argv[i + 1]))
	{
		if (State == MnA)
		{
			i++;
			argvlist.push_back(_argv[i]);
		}
		else //State == NnA
		{
			printf("\t[%s] Have spare part, will be ignored...\n", opt);
			break;
		}
	}
	switch (State)
	{
		case MnA:
			if (argvlist.size() > 1)
			{
				Epos = isExist(argvlist[0]);
				if (Epos != -1)//this option is exist ,will be over
				{
					_singleoptArray[Epos] = argvlist;
					printf("\t[%s] Have argumets : ", opt);
					for (size_t j = 1; j < argvlist.size(); j++)
						printf("%s ", argvlist[j].c_str());
					printf("\n");
				}
				else
				{
					_singleoptArray.push_back(argvlist);
					printf("\t[%s] Have argumets : ", opt);
					for (size_t j = 1; j < argvlist.size(); j++)
						printf("%s ", argvlist[j].c_str());
					printf("\n");
				}
			}
			else
			{
				printf("\tSingle-Option [%s] need argument, but Not gived...This option will be ignored\n", opt);
			}
			break;
		case NnA:
			Epos = isExist(argvlist[0]);
			if (Epos != -1)//this option is exist ,will be over
			{
				_singleoptArray[Epos] = argvlist;
				printf("\t[%s] Needn't argumets\n", opt);
			}
			else
			{
				_singleoptArray.push_back(argvlist);
				printf("\t[%s] Needn't argumets\n", opt);
			}
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
	if (!opt)
	{
		return false;
	}
	OPtState State = NOO;
	size_t pos = -1;
	State = isMultiOpt(opt, (int&)pos);
	if (State == InA)
	{
		printf("\tInvalid Multi-option, will be ignored\n");
		return false;
	}
	MultiOption mopt;
	mopt.longOpt = opt;
	pos = _moptStr.find_first_of("|", pos);
	char alias_char = '\0';
	bool save = true;
	switch (State)
	{
		case MnA:
			if (pos != _moptStr.npos)
			{
				alias_char = _moptStr[pos - 2];//MnA ,-2 get alais Char
			}
			else
			{
				printf("MnA only one Muilt option\n");
				alias_char = _moptStr[_moptStr.size() - 2];//MnA ,-2 get alais Char
			}
			//get multi-option arguments
			printf("\t[%s] Have argumets : ", opt);
			while (_argv[++i][0] != '-' && i + 1 < _argc)
			{
				printf("%s ", _argv[i]);
				mopt.margv.push_back(_argv[i]);
			}
			printf("\n");
			if (mopt.margv.size() < 1)
			{
				save = false;
				printf("\tMulti-Option [%s] need argument, but Not gived...,This option will be ignored\n", opt);
			}
			i--;
			break;
		case NnA:
			if (pos != _moptStr.npos)
			{
				alias_char = _moptStr[pos - 1];//NnA ,-1 get alais Char
			}
			else
			{
				printf("NnA only one Muilt option\n");
				alias_char = _moptStr[_moptStr.size() - 1];//NnA ,-1 get alais Char
			}
			break;
		default:
			break;
	}
	if (alias_char != '_')
	{
		if (isSingleOpt(alias_char) > NnA)
			mopt.aliasChar = alias_char;
		else
		{
			mopt.aliasChar = '\0';
			printf("\t[%s]:[%c] Multi-option alias is Fair,Because the char is use at single Option!\n", opt, alias_char);
		}
	}
	else
	{
		mopt.aliasChar = '\0';
		printf("\t[%s]:[ ] Multi-option alias is Fair,Because You not set for this Long option!\n", opt);
	}
	if (save)
	{
		int Epos = isExist(mopt.longOpt);
		if (Epos != -1)//this option is exist ,will be over
		{
			_multioptArray[Epos] = mopt;
		}
		else
		{
			_multioptArray.push_back(mopt);
		}
	}
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
					printf("\t[%s] is Multi-option\n", _argv[i]);
					if (_moptStr.empty())
						printf("You are not set Multi-option String!\n");
					else
						getMultiArgv(_argv[i], i);
				}
				else if (argvl > 2)	// -ab
				{
					vstr mixOpt;
					size_t j = 0;
					printf("Mix options :%s\n", _argv[i]);
					if (dealMixopt(_argv[i], mixOpt))
					{
						for (; j < mixOpt.size(); j++)
						{
							getSingleArgv(mixOpt[j].c_str(), i);
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
				printf("[%s] dafault help option!\n", _argv[i]);
			}
		}
		else
		{
			printf("[%s] Invalid argument, will be ignored...\n", _argv[i]);
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
	size_t i, j;
	for (i = 0; i < _singleoptArray.size(); i++)
	{
		printf("[%s] Single-option\n", _singleoptArray[i][0].c_str());
		for (j = 0; j < _singleoptArray[i].size(); j++)
			printf("\t%s\t", _singleoptArray[i][j].c_str());
		printf("\n");
	}
	for (i = 0; i < _multioptArray.size(); i++)
	{
		printf("[%s] Multi-option\n\tMulti-option's aliasChar: %c\n", _multioptArray[i].longOpt.c_str(), _multioptArray[i].aliasChar);
		if (!_multioptArray[i].margv.empty())
		{
			printf("\t[%s] Multi-option's argumet: ", _multioptArray[i].longOpt.c_str());
			for (j = 0; j < _multioptArray[i].margv.size(); j++)
				printf("%s\t", _multioptArray[i].margv[j].c_str());
		}
		printf("\n");
	}
}
/*
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
	 */
