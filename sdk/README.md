getOpt 详细介绍

##一，目的
    本类的目的是有效的解析命令行参数，使得对输入的参数可以很好地在某个工程内调用相关函数，同时设置相关函数的入参

##二，说明

###前提：
    对于命令行输入有三类：
    1，短选项(以"-"开头)
    2，长选项(以"--"开头)：这种选项可以有对应的简化短选项（只要这个短选项未被其他短选项使用）
    3，参数:作为有参数的选项的参数，无参数选项自动过滤多余的参数

对于选项，都可有是否存在参数的设置！

以下为类的相关申明(.h)
```c
class OPt
{
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
	OPt(int argc, char **argv):_argc(argc), _argv(argv){}
	OPt(int argc, char **argv, string optstr,string moptstr):_argc(argc), _argv(argv), _optStr(optstr),_moptStr(moptstr){}
public:
	bool getOpt();
	void showOptArray();
	bool setoptStr(const char *optstr);
	bool setMultioptStr(const char *Multioptstr);
	string OPtState2Str(OPtState State);
private:
	OPtState isSingleOpt(char optChar);
	OPtState isMultiOpt(string optstr,int& pos);
	bool getSingleArgv(const char* opt, int &i);
	bool getMultiArgv(const char* opt, int &i);
	bool dealMixopt(char* argv,vstr& argvs);
private:
	int _argc;
	char **_argv;
	string _optStr;
	string _moptStr;
	vvstr _singleoptArray; //single option's arguments
	mvstr _multioptArray;//Mulit option's arguments
};
```
- 你可以通过 构造函数 来设置 作为选项的字符(短选项)，字符串(长选项)
```c
OPt(int argc, char **argv, string optstr,string moptstr);
```
- 你也可以通过
```c
bool setoptStr(const char *optstr);//设置短选项
bool setMultioptStr(const char *Multioptstr);//设置长选项
```
- 对于选项的字符或字符串的设置格式：

    - 短选项：
    optstring中的指定的内容的意义
        0."-"开头，表示为短选项
        1.单个字符，表示选项内容，（如上ab:c:de各为一个选项）
        2.单个字符后接一个冒号：表示该选项后必须跟一个参数，参数紧跟在选项后或者以空格隔开。（如上例中的b:c:）
    - 长选项：
    multi-optstring中的指定的内容的意义
        0."--"开头，表明为长选项
        1.字符或字符串，建议用有意义的字符串 （如 help_h）
        2."_"分隔长选项和它对应的别名简化字符 (help_h中的_h)
        3.别名简化字符后接冒号：表示该选项后必须跟一个参数，参数紧跟在选项后或者以空格隔开
        4."|"分隔多个长选项,除非到字符末尾，如（help_h|add_a:）
    - 不支持(::) 可选参数选项,未来或许更新
- 对于长选项 即
```c
struct MultiOption
{
	string longOpt; //长选项的名字
	char   aliasChar;//长选项的简化别名字符（如果没被短选项使用，否则设为'\0'）
	vstr   margv;    //对于有参数的长选项，保留的参数
};
```
- 对于选项机器参数的存储
```c
    typedef vector < string > vstr;
    typedef vector < MultiOption > mvstr;//record Mulit option's arguments
    typedef vector < vstr > vvstr; //record single option's arguments
    vvstr _singleoptArray; //single option's arguments
	mvstr _multioptArray;//Mulit option's arguments
```
    - 短选项
        每一个选项存储在一个 ```vstr``` 里,该```vector``` 的首个值为选项名，剩余的（如果有）为之对应的参数
    - 长选项
        每一个选项存储在在一个 ```mvstr``` 的一个值```MultiOption```里,该```struct MultiOption``` 的
        - ```longOpt```   存储选项名
        - ```aliasChar``` 存储简化别名，
        - ```margv```     存储其对应参数（如果有）

### 几个注意
    1. 关于选项的混合
        - 短选项才可能混合，且只有 无参数间 或 无参数与一个有参数 混合
        - 长选项不可以混合
    2. 对于命令行的解析，尽可能获取所有段中正确的合理的选项和参数，且只会保留有效的选项及参数
		    - 如果选项有效，无需参数，->合理
		    - 如果选项有效，需要参数，且有参数（至于是否足够，无需考虑）->合理
		    - 如果选项有效，需要参数，但没有提供，->不合理
		    - 合理的才会被保留，否则自动忽略
    3. 对于同一选项多次出现，则以最后且合理的一次为准
