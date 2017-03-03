#include "dpcComm.h"
#include "iTransfMatrix.h"

namespace Imaginer
{
namespace DPC
{

/*
 * dataPcer common function
 *
 */

ppPIXELS dpcComm::density(ppPIXELS& allData,float scale)
{
    if(scale <= 0)
    {
        return NULL;
    }
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            allData[y][x] = allData[y][x]*scale;
        }
    }
    return allData;
}

ppPIXELS dpcComm::gray(ppPIXELS& allData)
{
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            allData[y][x] = allData[y][x].toGray();
        }
    }
    return allData;
}

ppPIXELS dpcComm::colorful(ppPIXELS& allData)
{
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            allData[y][x] = allData[y][x].toColor();
        }
    }
    return allData;
}


ppPIXELS dpcComm::shear(ppPIXELS& allData,bool XorY,float angle)
{
    angle = D2R(angle);
    ppPIXELS tmpData = NULL;
    int nx, ny;
    dataDup2(allData, tmpData);
    int H = XorY ? _height : (int)(_height + _width * ABS(tan(angle)));
    int W = XorY ? (int)(_width + _height * ABS(tan(angle))) : _width;
    //printf("Shear after: W:%d H:%d\n",W,H);
    delData(allData, _height);	// free
    if (allData == NULL)	// renew
    {
        newData(allData, W, H);
        // printf("new is ok\n");
    }
    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++)
        {
            /*
            //right up
            nx = XorY ? x + tan(angle) * (y - _height): x;
            ny = XorY ? y : y + tan(angle) * (x - _width);
            //left down
            nx = XorY ? x - tan(angle) * (y - _height)- _height: x;
            ny = XorY ? y : y - tan(angle) * (x - _width) - _width;
            */

            nx = (XorY ? ((angle > 0) ? (x + tan(angle) * (y - _height)) : (x + tan(angle) * (y - _height) - _height)) : x);
            ny = (XorY ? y : ((angle > 0) ? (y + tan(angle) * (x - _width)) :(y + tan(angle) * (x - _width) - _width)));
            //printf("Shear after: x:%d nx:%d y:%d ny:%d\n",x,nx,y,ny);
            if (nx < 0 || ny < 0 || nx >= _width || ny >= _height)
                allData[y][x].setRGBA(255, 255, 255);
            else
                allData[y][x] = tmpData[ny][nx];
        }
    }	// then set allhead
    //TODO:
    _width = W;
    _height= H;
    //reset _height and _width
    delData(tmpData, _height);
    return allData;
}

ppPIXELS dpcComm::revolution(ppPIXELS& allData,int px,int py,float angle)
{
    angle = D2R(angle);
    ppPIXELS tmpData;
    int nx, ny;
    dataDup2(allData, tmpData);
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {

            nx = (x - px) * cos(angle) - (y - py) * sin(angle) + px;
            ny = (x - px) * sin(angle) + (y - py) * cos(angle) + py;
            //printf("angle: %f cos:%f X:%d nx:%d\t Y:%d ny:%d\n",angle, cos(angle),x,nx,y,ny);
            if (nx < 0 || ny < 0 || nx >= _width || ny >= _height)
                allData[y][x].setRGBA(255, 255, 255);
            else
                allData[y][x] = tmpData[ny][nx];
        }
    }
    delData(tmpData, _height);
    return allData;
}

ppPIXELS dpcComm::transpose(ppPIXELS& allData,bool AR)
{
    ppPIXELS tmpData;
    dataDup2(allData, tmpData);
    int W = _height;
    int H = _width;
    delData(allData, _height);	// free
    if (allData == NULL)		// renew
    {
        newData(allData, W, H);
        // printf("new is ok\n");
    }
    // printf("WWW:%d HHH:%d\n",W,H);
#define AorR(x,y) ((AR) ? (tmpData[x][H-1-y]) : (tmpData[W-1-x][y]))
    for (int y = 0;y < H;y++)
    {
        for (int x = 0;x < W;x++)
        {
            allData[y][x] = AorR(x,y);
        }
    }
    // first free tmpData
    delData(tmpData, _height);
    // then set allhead
    //setHead(allhead,W,H);
    //TODO:
    _width = W;
    _height = H;
    //reset _height and _width
    return allData;
}

ppPIXELS dpcComm::spherize(ppPIXELS& allData,float radius)
{
    ppPIXELS tmpData;
    float w = _width / 2.0;
    float h = _height / 2.0;
    float wh = w / h;
    float mx1, mx2;
    int nx;
    float radius2 = radius * radius;
    float dealt;
    dataDup2(allData, tmpData);
    if (radius <= 0.0)			// oval
    {
        for (int y = 0; y < _height; y++)
        {
            //mx1 = w + (w * sqrt(2 * h * y - y * y)) / h;
            //mx2 = w - (w * sqrt(2 * h * y - y * y)) / h;
            mx1 = w + wh * sqrt(2 * h * y - y * y);
            mx2 = w - wh * sqrt(2 * h * y - y * y);
            dealt = (mx1 - mx2) / _width;
            //printf("Y:%d, %d - %d %d ;%f\n", y, mx1, mx2, mx1 - mx2, dealt);
            for (int x = 0; x < _width; x++)
            {
                if (x < mx2 || x > mx1 || dealt == 0)
                    allData[y][x].setRGBA(255, 255, 255);
                else
                {
                    nx = (x - mx2) / dealt;
                    if (nx >= _width)
                    {
                        nx = _width - 1;
                    }
                    allData[y][x] = tmpData[y][nx];
                }
            }
        }
    }

    else// circle
    {
        for (int y = 0; y < _height; y++)
        {
            mx1 = w + sqrt(radius2 - (y - h) * (y - h));
            // mx1 = w + sqrt(2*h*y-y*y);
            mx2 = w - sqrt(radius2 - (y - h) * (y - h));
            // mx2 = w - sqrt(2*h*y-y*y);
            dealt = (mx1 - mx2) / (_width * 1.0);
            //printf("Y:%d, %d - %d ;%f\n", y, mx1, mx2, dealt);
            for (int x = 0; x < _width; x++)
            {
                // if(x < mx2 || x > mx1)
                if (x < mx2 || x > mx1 || dealt == 0)
                    allData[y][x].setRGBA(255, 255, 255);
                else
                {
                    nx = (x - mx2) / dealt;
                    if (nx >= _width)
                    {
                        nx = _width - 1;
                    }
                    allData[y][x] = tmpData[y][nx];
                }
            }
        }
    }
    delData(tmpData, _height);
    return allData;
}

ppPIXELS dpcComm::zoom(ppPIXELS& allData,float scalex, float scaley)
{
    ppPIXELS tmpData;
    dataDup2(allData, tmpData);
    if (scalex <= 0.0 || scaley <= 0.0)
    {
        printf("In %s zoom number is <= 0 ,is wrong",__FUNCTION__);
        return NULL;
    }
    else
    {
        int W = _width * scalex;
        int H = _height * scaley;
        delData(allData, _height);	// free
        if (allData == NULL)	// renew
        {
            newData(allData, W, H);
            // printf("new is ok\n");
        }
        //printf("imageZoom: WWW:%d HHH:%d\n",W,H);
        for (int y = 0; y < H; y++)
        {
            for (int x = 0; x < W; x++)
            {
                int nx = (int)(x / scalex);
                int ny = (int)(y / scaley);
                allData[y][x] = tmpData[ny][nx];
                allData[y][x].setXY(x, y);
                // cout << allData[y][x];
                // cout << tmpData[y/scaley][x/scalex];
                // printf("\n");
            }
        }
        // first free tmpData
        delData(tmpData, _height);
        // then set allhead
        //TODO:
        _width = W;
        _height = H;
        //reset _height and _width
    }
    return allData;
}

ppPIXELS dpcComm::getImage3Color(ppPIXELS& allData,colorType color)
{
    switch (color)
    {
        case Red:
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    allData[y][x] = allData[y][x].get3Color(Red);
                }
            }
            break;
        case Green:
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    allData[y][x] = allData[y][x].get3Color(Green);
                }
            }
            break;
        case Blue:
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    allData[y][x] = allData[y][x].get3Color(Blue);
                }
            }
            break;
        default:
            break;
    }
    return allData;
}

ppPIXELS dpcComm::mirror(ppPIXELS& allData,Method method)
{
    ppPIXELS tmpData;
    dataDup2(allData, tmpData);
    switch (method)
    {
        case UD:					// up down change
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    allData[y][x] = tmpData[_height - 1 - y][x];
                }
            }
            break;
        case LR:					// left right change
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    allData[y][x] = tmpData[y][_width - 1 - x];
                }
            }
            break;
        case UR:					// up down & left right change
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    allData[y][x] = tmpData[_height - 1 - y][_width - 1 - x];
                }
            }
            break;
        default:					// NONE
            for (int y = 0; y < _height; y++)
            {
                for (int x = 0; x < _width; x++)
                {
                    allData[y][x] = tmpData[y][x];
                }
            }
            break;
    }
    delData(tmpData, _height);
    return allData;
}

ppPIXELS dpcComm::change(ppPIXELS& allData,const iMatrix& method)
{
    ppPIXELS tmpData;
    dataDup2(allData, tmpData);
    PIXELS tmp;
    std::cout << method << std::endl;
    int newX = 0,newY = 0;
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            allData[y][x] = iColor::WHITE;
            tmp  = tmpData[y][x] * method;
            newX = REPAIR(tmp.getX(),_width);
            newY = REPAIR(tmp.getY(),_height);
            allData[newY][newX] = tmp;
        }
    }
    delData(tmpData, _height);
    return allData;
}


ppPIXELS dpcComm::move(ppPIXELS& allData,int mx, int my)
{
#define MOVEX(Mx,x) ((Mx)>0 ? (x <= Mx) : (x > _width-1+Mx))
#define MOVEY(My,y) ((My)>0 ? (y <= My) : (y > _height-1+My))
    ppPIXELS tmpData;
    dataDup2(allData, tmpData);
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            if ((MOVEY(my, y) || MOVEX(mx, x)))
                allData[y][x].setRGBA(255, 255, 255);
            else
                allData[y][x] = tmpData[y - my][x - mx];
        }
    }
    delData(tmpData, _height);
    return allData;
}

/*
ppPIXELS dpcComm::imageSpatialize(string outpath)
{
    if (initSpatialize(bmppathl))
    {
        printf("In %s init bmp list image is OK!\n",__FUNCTION__);
        //show_6path(rbmp);
    }
    else
    {
        cout << "" << endl;
        printf("In %s init bmp list image is fair!\n",__FUNCTION__);
        return NULL;
    }
    map<Position,string>::iterator it;
    ppPIXELS _Data[6] = {NULL};
    int i;
    int LH = 0,RH = 0, UH = 0,DH = 0, FH = 0,BH = 0, H = 0,W = 0;
    for(i = 0,it = rbmp.begin(); it != rbmp.end() ; ++it,++i)
    {
        cout<<"key: " << it->first <<" (" << Pos2str(it->first)
            << ")" << "\tvalue: " << it->second << endl;
        bmppath = it->second;
        fp = fopen(bmppath.c_str(), "rb");
        if (!init_image())
        {
            printf("init_image %s is failed\n",(it->second).c_str() );
        }
        printf("it->first:%d\n",it->first);
        switch(it->first)
        {
            case Left:
                imageShear(allData,false,45.0);
                H = allhead.infoHead.biHeight;
                LH = H;
                break;
            case Right:
                imageShear(allData,false,-45.0);
                H = allhead.infoHead.biHeight;
                RH = H;
                break;
            case Up:
                imageShear(allData,true,45.0);
                W = allhead.infoHead.biWidth;
                UH = allhead.infoHead.biHeight;
                break;
            case Down:
                imageShear(allData,true,-45.0);
                W = allhead.infoHead.biWidth;
                DH = allhead.infoHead.biHeight;
                break;
            case Front:
                FH = allhead.infoHead.biHeight;
                break;
            case Back:
                BH = allhead.infoHead.biHeight;
                break;
            default:
                printf("why:%d\n",it->first);
                break;
        }
        printf("shear is ok... W:%d H:%d\n",W,H);
        get_image_msg();
        deleteAll();
    }
    //open OutFile
    size_t pos = bmppath.rfind(".bmp");
    if(pos != string::npos)
        outpath = bmppath.substr(0,pos) +"_Spl.bmp";
    else
        outpath = bmppath +"_Spl.bmp";
    fpo = fopen(outpath.c_str(), "wb");
    if (!fpo)
    {
        printf("In %s open out image is FAILED!\n",__FUNCTION__);
    }
    //write image
    setHead(allhead,W,H);
    //write head
    fwrite(&allhead, 54, 1, fpo);
    //write body
    ppPIXELS newImage = NULL;
    newData(newImage,W,H);
    //memset(newImage,255,sizeof(PIXELS)*W*H);
    //printf("imageData:%p,fpo:%p",_Data[Down],fpo);
    //cout << _Data[Down][0][0];
    writeAllData(newImage);
    //free a image memory!
    if(LH)
    {
        //printf("Lwwww%d\n",LH);
        delData(_Data[Left],LH);
    }
    if(RH)
    {
        //printf("Rwwww%d\n",RH);
        delData(_Data[Right],RH);
    }
    if(UH)
    {
        //printf("Uwwww%d\n",UH);
        delData(_Data[Up],UH);
    }
    if(DH)
    {
        //printf("Dwwww%d ",DH);
        delData(_Data[Down],DH);
    }
    if(FH)
    {
        //printf("Fwwww%d\n",FH);
        delData(_Data[Front],FH);
    }
    if(BH)
    {
        //printf("Bwwww%d\n",BH);
        delData(_Data[Back],BH);
    }
    return NULL;
}*/

bool     dpcComm::genHistogram(ppPIXELS& allData,colorType color,bool isOverWrite )
{
    if(NULL == allData)
    {
        printf("In %s allData is NULL\n",__FUNCTION__);
        return false;
    }
    float PIXELSCOUNT = _width * _height;
    int RED[256] = {0};
    float FRED[256] = {0};
    int GRE[256] = {0};
    float FGRE[256] = {0};
    int BLU[256] = {0};
    float FBLU[256] = {0};
    int ALL[256] = {0};
    float FALL[256] = {0};
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            RED[allData[y][x].getRed()]++;
            GRE[allData[y][x].getGreen()]++;
            BLU[allData[y][x].getBlue()]++;
        }
    }
    for(int i = 0; i < 256; i++)
    {
        FRED[i] = RED[i] / PIXELSCOUNT;
        FGRE[i] = GRE[i] / PIXELSCOUNT;
        FBLU[i] = BLU[i] / PIXELSCOUNT;
        ALL[i] = RED[i] + GRE[i] + BLU[i];
        FALL[i] = ALL[i] / PIXELSCOUNT;
        //FALL[i] = FRED[i] + FGRE[i] + FBLU[i];
        //printf("i:%d R:%d(%f) G:%d(%f) B:%d(%f)\n",i,RED[i],FRED[i],GRE[i],FGRE[i],BLU[i],FBLU[i]);
        //printf("%d %d %d %d %d\n",i,RED[i],GRE[i],BLU[i],ALL[i]);
    }
    float Rmax = 0,Gmax = 0,Bmax = 0,Amax = 0;
    for(int i = 0; i < 256; i++)
    {
        if(FRED[i] > Rmax)
            Rmax=FRED[i];
        if(FGRE[i] > Gmax)
            Gmax=FGRE[i];
        if(FBLU[i] > Bmax)
            Bmax=FBLU[i];
    }
    Amax  = Rmax + Gmax + Bmax;
    //printf("PIXELSCOUNT: %f\n",PIXELSCOUNT);
    //printf("R:%f G:%f B:%f A:%f\n",Rmax,Gmax,Bmax,Amax);
    //set allhead
    //set Histogram's frame
    int border = 1;
    int Lmargin = 20, Rmargin = 20;
    int Dmargin = 20, Umargin = 20;
    int Lb = Lmargin + border;
    int Rb = Rmargin + border;
    int Ub = Umargin + border;
    int Db = Dmargin + border;
#define Hh 158
    int W = 255 + Lb + Rb;
    int H = Hh + Ub + Db;// 158 = 256*0.618 golden section
    int H_D = H - Dmargin;
    int W_R = W - Rmargin;
    int H_Db = H - Db;
    int W_Rb = W - Rb;
    float Rscale = Hh/Rmax;
    float Gscale = Hh/Gmax;
    float Bscale = Hh/Bmax;
    float Ascale = Hh/Amax;
    //printf("R:%f G:%f B:%f A:%f\n",Rscale,Gscale,Bscale,Ascale);
    if(isOverWrite)
    {
        delData(allData, _height);	// free
        if (allData == NULL)	// renew
        {
            newData(allData, W, H);
            _width = W;
            _height = H;
            // printf("new is ok\n");
        }
    }
    bool flag = true;
    for (int y = H - 1; y >= 0; y--)
    {
        for (int x = 0; x < W; x++)
        {
            if(y < Umargin || y > H_D || x < Lmargin || x > W_R)
            {
                allData[y][x].setRGBA(255,255,255);
            }
            else if((y >= Umargin && y < Ub) || (y <= H_D && y > H_Db)
                    || (x >= Lmargin && x < Lb) || (x > W_Rb && x <= W_R))
            {
                allData[y][x].setRGBA(0,0,0);
            }
            else
            {
                switch(color)
                {
                    case Red:
                        if(FRED[x-Rb]*Rscale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGBA(0,0,255);

                        }
                        else
                            flag = true;
                        break;
                    case Green:
                        if(FGRE[x-Rb]*Gscale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGBA(0,255,0);
                        }
                        else
                            flag = true;
                        break;
                    case Blue:
                        if(FBLU[x-Rb]*Bscale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGBA(255,0,0);
                        }
                        else
                            flag = true;
                        break;
                    case Pricolor:
                        if(FALL[x-Rb]*Ascale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGBA(57,77,118);
                        }
                        else
                            flag = true;
                        break;
                    default:
                        printf("NO THIS COLOR!\n");
                        break;
                }
                if(flag)
                {
                    allData[y][x].setRGBA(255,255,255);
                }
            }
        }
    }
#undef  Hh
    return true;
}

bool     dpcComm::genBardiagram(ppPIXELS& allData,colorType color,bool isOverWrite )
{
    if(NULL == allData)
    {
        printf("In %s allData is NULL\n",__FUNCTION__);
        return false;
    }
    int RED[256] = {0};
    int GRE[256] = {0};
    int BLU[256] = {0};
    int ALL[256] = {0};
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            RED[allData[y][x].getRed()]++;
            GRE[allData[y][x].getGreen()]++;
            BLU[allData[y][x].getBlue()]++;
        }
    }
    float Rmax = 0,Gmax = 0,Bmax = 0,Amax = 0;
    for(int i = 0; i < 256; i++)
    {
        ALL[i] = RED[i] + GRE[i] + BLU[i];
        if(RED[i] > Rmax)
            Rmax=RED[i];
        if(GRE[i] > Gmax)
            Gmax=GRE[i];
        if(BLU[i] > Bmax)
            Bmax=BLU[i];
        //printf("i:%d R:%d G:%d B:%d\n",i,RED[i],GRE[i],BLU[i]);
    }
    Amax = Rmax + Gmax + Bmax;
    //set allhead
    //set Histogram's frame
    int border = 1;
    int Lmargin = 20, Rmargin = 20;
    int Dmargin = 20, Umargin = 20;
    int Lb = Lmargin + border;
    int Rb = Rmargin + border;
    int Ub = Umargin + border;
    int Db = Dmargin + border;
#define Hh 158
    int W = 255 + Lb + Rb;
    int H = Hh + Ub + Db;// 158 = 256*0.618 golden section
    int H_D = H - Dmargin;
    int W_R = W - Rmargin;
    int H_Db = H - Db;
    int W_Rb = W - Rb;
    float Rscale = Hh/Rmax;
    float Gscale = Hh/Gmax;
    float Bscale = Hh/Bmax;
    float Ascale = Hh/Amax;
    if(isOverWrite)
    {
        delData(allData, _height);	// free
        if (allData == NULL)	// renew
        {
            newData(allData, W, H);
            _width = W;
            _height = H;
            // printf("new is ok\n");
        }
    }
    bool flag = true;
    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++)
        {
            if(y < Umargin || y > H_D || x < Lmargin || x > W_R)
            {
                allData[y][x].setRGBA(255,255,255);
            }
            else if((y >= Umargin && y < Ub) || (y <= H_D && y > H_Db)
                    || (x >= Lmargin && x < Lb) || (x > W_Rb && x <= W_R))
            {
                allData[y][x].setRGBA(0,0,0);
            }
            else
            {
                switch(color)
                {
                    case Red:
                        if(RED[x-Rb]*Rscale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGBA(0,0,255);
                        }
                        else
                            flag = true;
                        break;
                    case Green:
                        if(GRE[x-Rb]*Gscale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGBA(0,255,0);
                        }
                        else
                            flag = true;
                        break;
                    case Blue:
                        if(BLU[x-Rb]*Bscale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGBA(0,0,255);
                        }
                        else
                            flag = true;
                        break;
                    case Pricolor:
                        if(ALL[x-Rb]*Ascale > y-Db)
                        {
                            flag = false;
                            allData[y][x].setRGBA(57,77,118);
                        }
                        else
                            flag = true;
                        break;
                    default:
                        printf("NO THIS COLOR!\n");
                        break;
                }
                if(flag)
                {
                    allData[y][x].setRGBA(255,255,255);
                }
            }
        }
    }
#undef  Hh
    return true;
}

bool     dpcComm::dealManager(const char* dealType)
{
    if(!_Data)
    {
        printf("cannot deal with,there are not Data!\n");
        return false;
    }
    if(!dealType)
    {
        printf("Not deal with!,will output itself\n");
        return true;
    }
    iTransfMatrix m(D2R(-45),iTransfMatrix::ROTATEz);
    while(*dealType)
    {
        switch(*dealType)
        {
        case 'T':
            cout << "  -T     imageTranspose  : Transpose a iamge\n";
           transpose(_Data);
            break;
        case 'R':
            cout << "  -R     imageRevolution : Revolution a image\n";
            revolution(_Data,_width/2,_height/2,-45);
            break;
        case 's':
            cout << "  -s     imageSpherize   : Spherize a image\n";
            spherize(_Data);
            break;
        case 'Z':
            cout << "  -Z     imageZoom       : zoom a image\n";
            zoom(_Data,0.5,0.5);
            break;
        case 'M':
            cout << "  -M     imageMirror     : Mirror a image\n";
            //mirror(_Data,UD);
            change(_Data,m);
            break;
        case 'S':
            cout << "  -S     imageShear      : Shear a image\n";
            shear(_Data,true,-45.0);
            break;
        case 'm':
            cout << "  -m     imageMove       : move a image\n";
            move(_Data,100,100);
            break;
        case 'C':
            cout << "  -C     getImage3Color  : get a image's 3(R,G,B) color image\n";
            getImage3Color(_Data,Green);
            break;
        case 'H':
            cout << "  -H     genHistogram    : get a image's 3(R,G,B) color Histogram\n";
            genBardiagram(_Data,Red);
            //genBardiagram(_Data,Green);
            //genBardiagram(_Data,Blue);
            //genBardiagram(_Data);
            break;
        case 'B':
            cout << "  -B     genBardiagram   : get a image's 3(R,G,B) color Bar diagram\n";
            //genHistogram(_Data,Red);
            genHistogram(_Data,Green);
            //genHistogram(_Data,Blue);
            //genHistogram(_Data);
            break;
        case 'b':
            cout << "  -b     backGround_ize   : get a image's part of backGround\n";
            //backGround_ize();
            break;
        case 'd':
            cout << "  -d     imageDensity     : Change a image each pixel's Idensity\n";
            density(_Data,200);
            break;
        case 'g':
            cout << "  -g     imageGray        : get a image's gray image\n";
            gray(_Data);
            break;
        case 'c':
            cout << "  -c     imageColorful    : make a image to colorfull\n";
            colorful(_Data);
            break;
        default:
            printf("Not deal with!\n");
            break;
        }
        dealType++;
    }
    //set data
    if(_dp)
    {
        _dp->setData(_Data);
        _dp->setWidth(_width);
        _dp->setHeight(_height);
    }
    return true;
}

}//namespace DPC
}//namespace Imaginer
