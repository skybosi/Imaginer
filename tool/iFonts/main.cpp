#include <time.h>
#include <iostream>
#include "iFonts.h"
#define random(x) (rand()%x)
/*
int main()
{
    int a[10],i,max,min;
    for(i=0;i<10;i++){
        a[i] = random(1000);
    }
    max=0;
    min=10000;
    for(i=0;i<10;i++)
    {
        if(max<a[i])
        {
            max=a[i];
        }
        //if(min>a[i])
        else{
            min=a[i];
        }
    }
    printf("最大值为%d\n最小种为%d\n",max,min);
    return 0;
}
*/

using namespace Imaginer::Utils;

void show(int ch, const vdPIXELS& vna)
{
    std::cout << "character: " << ch << " boundary: " << std::endl;
    for(size_t i = 0; i < vna.size(); ++i)
    {
        for(size_t j = 0; j < vna[i].size(); ++j)
        {
            printf("$%2d: x: %2d; y: %2d\n", j, vna[i][j].getX(), vna[i][j].getY());
        }
    }
}

int main(int argc, char* argv[])
{
    if(argc < 3){
        printf("Usage: %s r/w path\n", argv[0]);
        printf("w: will create a new fonst to file xxx\n");
        printf("R: will read fonst file from xxx\n");
    }
    else if(argv[1][0] == 'w')
    {
        srand((int)time(0));
        vdPIXELS va;
        dPIXELS a;
        dPIXELS a2;
        int x = 0, y = 0;
        a.push_back(PIXELS(x, y));
        int step = 0;
        for(int i = 1; i < 10; ++i)
        {
            step = random(2);
            if(step){
                a.push_back(PIXELS(x += 1, y));
            }
            else{
                a.push_back(PIXELS(x, y += 1));
            }
        }
        for(int i = 0; i < 10; ++i)
        {
            step = random(2);
            if(step){
                a2.push_back(PIXELS(x, y += 1));
            }
            else{
                a2.push_back(PIXELS(x += 1, y));
            }
        }
        va.push_back(a);
        va.push_back(a2);
        for(size_t i = 0; i < va.size(); ++i)
        {
            for(size_t j = 0; j < va[i].size(); ++j)
            {
                printf("$%2d: x: %2d; y: %2d\n", j, va[i][j].getX(), va[i][j].getY());
            }
        }
        cfont c('a', a.size() + a2.size() + va.size() + va.size() - 1, va.size());
        c.encode('a', va);
        // create a new font's file to save a new fonts
        iFonts ifont(argv[2]);
        // encode the cfont to font's file
        ifont.encoder('a', c);
        std::cout << c << std::endl;
        std::cout << "decode cfont ..." << std::endl;
        vdPIXELS vna;
        int ot = 0;
        c.decode(ot, vna, 0, 0);
        show(ot, vna);
    }
    else if(argv[1][0] == 'r')
    {
        iFonts ifont;
        // load the font file from a path
        ifont.loader(argv[2]);
        // use decoder to get a character if exist
        cfont c = ifont.decoder('a');
        vdPIXELS vna;
        int ot = 0;
        c.decode(ot, vna, 0, 0);
        std::cout << c << std::endl;
        std::cout << "decode cfont ..." << std::endl;
        show(ot, vna);
    }else
    {
        printf("invalid option!");
        printf("Usage: %s r/w path\n", argv[0]);
        printf("w: will create a new fonst to file xxx\n");
        printf("R: will read fonst file from xxx\n");
    }
    return 0;
}


