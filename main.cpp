//#include <unistd.h>
#include "ibmp.h"
#include "dataPcer.h"
#include "dpcCore.h"
#include "dpcComm.h"
#include "dpcCons.h"
#include "BmpPer.h"
#include "iConsper.h"
using namespace Imaginer::DPC;

void testComm(int argc,char* argv[])
{
    std::cout << "Test dpcCons... " << std::endl;
    BmpPer* bmpComm = new BmpPer(argv[0]);
    if(bmpComm->read())
    {
        //dataPcer dpcer(bmp);
        dpcComm dpcer(bmpComm->getData(),bmpComm->getWidth(),bmpComm->getHeight());
        dpcer.dealManager("M");
        //bmp->write(argv[2],dpcer.retnData(),dpcer.retnWidth(),dpcer.retnHeight());
        //等价于 bmp->write(argv[2],false);
    }
    bmpComm->write(argv[1],true);
    delete bmpComm;
}

void testCore(int argc,char* argv[])
{
    std::cout << "Test dpcCons... " << std::endl;
    BmpPer* bmpCore = new BmpPer(argv[0]);
    if(bmpCore->read())
    {
        //dataPcer dpcer(bmp);
        dpcCore dpcer(bmpCore->getData(),bmpCore->getWidth(),bmpCore->getHeight());
        dpcer.dealManager("c");
        //bmp->write(argv[2],dpcer.retnData(),dpcer.retnWidth(),dpcer.retnHeight());
        //等价于 bmp->write(argv[2],false);
    }
    bmpCore->write(argv[1],true);
    delete bmpCore;
}

void testCons(int argc,char* argv[])
{
    std::cout << "Test dpcCons... " << std::endl;
    std::vector<std::string> funlist;
    funlist.push_back("x + y");
    funlist.push_back("sin(x)*cos(x)");
    funlist.push_back("tan(x)*x");
    funlist.push_back("log(x*x)+tan(x)*x + sin(x)+cos(x)");
    funlist.push_back("sin(x)*x");
    funlist.push_back("cos(x)*x");
    int t = 0;
    /*
    while(t++ < 100)
    {
    */
        BmpPer bm(500,500);
        dpcCons dpcons(bm.getData(),bm.getWidth(),bm.getHeight(),funlist,-10+t,10+t,400);
        dpcons.dealManager("l");
        bm.write(argv[1]);
    /*
      std::cout << "sleep ..." << std::endl;
      sleep(1);
    }*/
}


int main(int argc,char* argv[])
{
    if(argc < 3)
    {
        printf("%s Usage: case(0,1,2) inbmp outbmp\n",argv[0]);
        printf("case 0 : %s 用来处理普通的图像操作(平移，镜像...)\n","COMM");
        printf("case 1 : %s 用来处理core的图像操作(边界获取)\n","CORE");
        printf("case 2 : %s 用来构造一些数学函数图像的操作(sin(x))\n","CONS");
        printf("NOTE   : 由于正在开发相关命令行处理操作没有完善，相关功能请参考代码\n");
        return 0;
    }
    enum CASE{
        COMM,
        CORE,
        CONS
    };
    int sel = atoi(argv[1]);
    argc -= 2;
    argv += 2;
    switch (sel) {
    case COMM:
        testComm(argc,argv);
        break;
    case CORE:
        testCore(argc,argv);
        break;
    case CONS:
        testCons(argc,argv);
        break;
    default:
        break;
    }
    return 0;
}

