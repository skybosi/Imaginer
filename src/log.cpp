#include "log_T.h"
#include <iostream>
using namespace std;

int main()
{
	logT a("./sdk/etc/logsys.conf");
	(a)(LDEBUG,ALLFMT(test,"aaaaaa"));
	(a)(LDEBUG,ALLFMT(test,"bbbbbb"));
	return 0;
}
