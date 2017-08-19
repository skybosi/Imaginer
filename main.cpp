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
    std::cout << "Test dpcComm... " << std::endl;
    OPt opt(argc, argv, "i:o:bB:cC:d:gH:m:M:R:sS:TZ:p:",dpcComm::doc());
    if(!opt.getOpt())
    {
        printf("deal with option error!\n");
        return;
    }
    OPt::vargv vg = opt.opt2Sargvs('i');
    char* in = NULL;
    char* out = NULL;
    if(!vg.empty()){
        in = vg[0];
    }
    OPt::vargv vg1 = opt.opt2Sargvs('o');
    if(!vg1.empty()){
        out = vg1[0];
    }
    BmpPer* bmpComm = new BmpPer(in);
    if(bmpComm->read())
    {
        //dataPcer dpcomm(bmp);
        dpcComm dpcomm(bmpComm->getData(),bmpComm->getWidth(),bmpComm->getHeight());
        if(dpcomm.dealManager(opt))
        {
            //bmp->write(argv[2],dpcer.retnData(),dpcer.retnWidth(),dpcer.retnHeight());
            //等价于 bmp->write(argv[2],false);
            bmpComm->write(out,true);
        }
	}
    delete bmpComm;
}

void testCore(int argc,char* argv[])
{
    std::cout << "Test dpcCore... " << std::endl;
    OPt opt(argc, argv, "i:o:bhcLldpE:D:",dpcCore::doc());
    if(!opt.getOpt())
    {
        printf("deal with option error!\n");
        return;
    }
    OPt::vargv vg = opt.opt2Sargvs('i');
    char* in = NULL;
    char* out = NULL;
    if(!vg.empty()){
        in = vg[0];
    }
    vg = opt.opt2Sargvs('o');
    if(!vg.empty()){
        out = vg[0];
    }
    BmpPer* bmpCore;
    if(!in){
        bmpCore = new BmpPer(out, "wb");  // wb user to create a new image
    }else{
        bmpCore = new BmpPer(in);  // wb user to create a new image
    }
    if(bmpCore->read())
    {
        dpcCore dpcer(bmpCore);
        //dpcCore dpcer(bmpCore->getData(),bmpCore->getWidth(),bmpCore->getHeight());
        if(dpcer.dealManager(opt))
        {
            //bmp->write(argv[2],dpcer.retnData(),dpcer.retnWidth(),dpcer.retnHeight());
            //等价于 bmp->write(argv[2],false);
            bmpCore->write(out,true);
        }
    }
    delete bmpCore;
}

void testCons(int argc,char* argv[])
{
    OPt opt(argc, argv, "i:o:pl",dpcCons::doc());
    if(!opt.getOpt())
    {
        printf("deal with option error!\n");
        return;
    }
    OPt::vargv vg = opt.opt2Sargvs('i');
    char* in = NULL;
    char* out = NULL;
    if(!vg.empty()){
        in = vg[0];
    }
    vg = opt.opt2Sargvs('o');
    if(!vg.empty()){
        out = vg[0];
    }
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
        if(dpcons.dealManager(opt))
        {
            bm.write(out);
        }
    /*
      std::cout << "sleep ..." << std::endl;
      sleep(1);
    }*/
}

void testCuts(int argc,char* argv[])
{
    std::cout << "Test dpcCut... " << std::endl;

    BmpPer* bmpCore = new BmpPer(argv[0]);
    char suffix[8];
    char prev[100] = {0};
    strcpy(prev, argv[1]);
    char* file;
    int W = bmpCore->Width(), H = bmpCore->Height();
    for(int y = 0, i = 0; y < H; y += 33, ++i)
    {
        for(int x = 0, j = 0; x < W; x += 50, ++j)
        {
            if(bmpCore->read(28,28,x,y))
            {
               sprintf(suffix, "%02d%02d.bmp", i, j);
               file = strcat(prev,suffix);
               bmpCore->write(file,false);
               strcpy(prev, argv[1]);
               memset(suffix,0,8);
            }
        }
    }
    printf("cut all OK!\n");
    delete bmpCore;
}


int main(int argc,char* argv[])
{
    if(argc < 3)
    {
        printf("%s Usage: case(0,1,2) inbmp outbmp option\n",argv[0]);
        printf("case 0 : %s 用来处理普通的图像操作(平移，镜像...)\n","COMM");
        printf("case 1 : %s 用来处理core的图像操作(边界获取)\n","CORE");
        printf("case 2 : %s 用来构造一些数学函数图像的操作(sin(x))\n","CONS");
        printf("case 3 : %s 用来大图片切割\n","cut picture");
        printf("NOTE   : 由于正在开发相关命令行处理操作没有完善，相关功能请参考代码\n");
        return 0;
    }
    enum CASE{
        COMM,
        CORE,
        CONS,
        CUTS
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
    case CUTS:
        testCuts(argc,argv);
        break;
    default:
        break;
    }
    return 0;
}

