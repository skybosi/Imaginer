//#include <unistd.h>
#include "ibmp.h"
#include "dataPcer.h"
#include "dpcCore.h"
#include "dpcComm.h"
#include "dpcCons.h"
#include "BmpPer.h"
#include "iConsper.h"
using namespace Imaginer::DPC;

int main(int argc,char* argv[])
{
    if(argc < 3)
    {
        printf("%s usage: pic.bmp outpath\n",argv[0]);
        return 0;
    }
    /*
    std::vector<std::string> funlist;
    funlist.push_back("x + y");
    funlist.push_back("sin(x)*cos(x)");
    funlist.push_back("tan(x)*x");
    funlist.push_back("log(x*x)+tan(x)*x + sin(x)+cos(x)");
    funlist.push_back("sin(x)*x");
    funlist.push_back("cos(x)*x");
    int t = 0;
    //while(t++ < 100)
    //{
        BmpPer bm(500,500);
        dpcCons dpcons(bm.getData(),bm.getWidth(),bm.getHeight(),funlist,-10+t,10+t,400);
        dpcons.dealManager("l");
        bm.write(argv[2]);
    //  std::cout << "sleep ..." << std::endl;
    //  sleep(1);
    //}
    BmpPer* bmp = new BmpPer(argv[1]);
    if(bmp->read())
    {
        dpcCons dpcons(bmp->getData(),bmp->getWidth(),bmp->getHeight(),"sin",-5,5,200);
        //dpcCons dpcons(bmp,"sin",-5,5,200);
        dpcons.dealManager("l");
        bmp->write(argv[2]);
    }

    */

    BmpPer* bmp = new BmpPer(argv[1]);
    if(bmp->read())
    {
        //dataPcer dpcer(bmp);
        //dpcCore dpcer(bmp->getData(),bmp->getWidth(),bmp->getHeight());
        dpcComm dpcer(bmp->getData(),bmp->getWidth(),bmp->getHeight());
        dpcer.dealManager("M");
        //bmp->write(argv[2],dpcer.retnData(),dpcer.retnWidth(),dpcer.retnHeight());
        //等价于 bmp->write(argv[2],false);
    }
    bmp->write(argv[2],true);
    delete bmp;

    cout<<endl;
    return 0;
}

