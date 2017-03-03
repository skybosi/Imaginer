#include "BmpPer.h"

namespace Imaginer
{
namespace DPer
{

/*
 * bmp image init and destroy
 *
 */
BmpPer::BmpPer(int width,int height,bool grid):DperMum(""),fpi(NULL), fpo(NULL),biBitCount(24), pColorTable(NULL)
{
    initHead(width,height);
    dp_Data = PIXELS::allocator(height,width,grid);
    dp_width = width;
    dp_height = height;
}

BmpPer::BmpPer(const char *imagepath): DperMum(imagepath),fpi(NULL), fpo(NULL), pColorTable(NULL)
{
    // 二进制读方式打开指定的图像文件
    fpi = fopen(_imagePath.c_str(), "rb");
    if(!fpi)
    {
        printf("image file cannot find!\n");
        exit(-1);
    }
    if (decoder())
    {
        printf("In %s decode bmp image is OK!\n",__FUNCTION__);
    }
    else
    {
        printf("In %s init bmp image is failed!\n",__FUNCTION__);
        return;
    }
    cout << "create a BmpPer ...." << endl;
}

BmpPer::~BmpPer(){
    delData(_Data,_height);
    delData(dp_Data,dp_height);
    if(fpi)
    {
        fclose(fpi);
        fpi = NULL;
    }
    if(fpo)
    {
        fclose(fpo);
        fpo = NULL;
    }
}

/*
 * read bmp image
 *
 */
bool    BmpPer::getHead()
{
    if (NULL == fpi)
    {
        printf("In %s open image is FAILED!\n",__FUNCTION__);
        return false;
    }
    U16 BM;
    fread(&BM, sizeof(U16), 1, fpi);
    // printf("BBBBBBB:%0X\n",BM);
    if (BM != 0x4d42)
    {
        printf("In %s is not bmp image!\n",__FUNCTION__);
        return false;
    }
    rewind(fpi);
    //read head information for picture
    fread(&head, sizeof(BITMAPFILEHEADER), 1, fpi);
    // printf("BITMAPFILEHEADER:%ld\n",sizeof(BITMAPFILEHEADER));
    bfOffBits = head.bfOffBits;
    // 位图信息头结构变量,存放在变量head中
    fread(&infohead, sizeof(BITMAPINFOHEADER), 1, fpi);
    // printf("BITMAPINFOHEADER:%ld\n",sizeof(BITMAPINFOHEADER));
    // 获取图像宽、高、每像素所占位数等信息
    _width = infohead.biWidth;
    _height = infohead.biHeight;
    biBitCount = infohead.biBitCount;
    allhead.bmpHead = head;
    allhead.infoHead = infohead;
    return true;
}

bool    BmpPer::decoder()
{
    if(!getHead())
    {
        printf("read picture head information is ERROR!\n");
        return false;
    }
    // 定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
    // int lineByte = (imageWidth * biBitCount/8+3)/4*4;
    // int lineByte = (imageWidth * biBitCount + 31)/32*4;
    int lineByte = (_width * biBitCount + 31) / 32 * 4;
    // 灰度图像有颜色表，且颜色表表项为256
    if (biBitCount == 8)
    {
        // 申请颜色表所需要的空间，读颜色表进内存
        pColorTable = new iColor[256];
        fread(pColorTable, sizeof(iColor), 256, fpi);
        /*
             int i = 0; printf("颜色表:\n"); while(i < 256) { printf("$%d %3d
             ",i,pColorTable[i]); printf("r:%3d ",pColorTable[i].red());
             printf("g:%3d ",pColorTable[i].green());
             printf("b:%3d\n",pColorTable[i].blue()); i++; } */
    }
    BYTE8 *linedata = new BYTE8[lineByte];
    //_Data = new pPIXELS[_height];
    //_Data = (ppPIXELS)malloc(_height*sizeof(pPIXELS));
    _Data = PIXELS::allocator(_height,_width);
    int tablesite;
    int x,k,y = _height - 1;
    for (; y >= 0; y--)			// y is row number
    {
        //_Data[y] = new PIXELS[_width];
        //_Data[y] = (pPIXELS)malloc(_width*sizeof(PIXELS));
        fread(linedata, 1, lineByte, fpi);
        for (x = 0,k = 0; x < _width; k++, x++)
        {
            // printf("%03d ",linedata[k]);
            switch (biBitCount)
            {
                case 24:
                    _Data[y][x].setRGBA(linedata[k], linedata[k+1], linedata[k+2]);
                    //_Data[y][x].setRGBA(linedata[k+2],linedata[k+1],linedata[k]);
                    k += 2;
                    _Data[y][x].setXY(x, y);
                    _Data[y][x].isEdge(_width, _height);
                    _Data[y][x].initpPos();
                    break;
                case 8:
                    tablesite = linedata[k];
                    // printf("%03d\n",linedata[k]);
                    // printf("颜色表位置:%d\n",tablesite);
                    _Data[y][x].setRGBA(pColorTable[tablesite]);
                    _Data[y][x].setXY(x, y);
                    _Data[y][x].isEdge(_width, _height);
                    _Data[y][x].initpPos();
                    break;
                default:
                    break;
            }
        }
    }
    if(linedata){delete[]linedata;linedata = NULL;}
    rewind(fpi);
    return true;
}

bool    BmpPer::read(int width,int height,int beginX,int beginY)
{
//#define RESET(_T,_FLAG) (((_T) < 0) ? 0 : (((_T) > (_FLAG)) ? (_FLAG) : (_T)))
#define WHY(_B,_E,_b,_e,_WH) (                              \
                               (                            \
                                ((_B) < (_E)) ?             \
                                ((_b) = (_B),(_e) = (_E)) : \
                                ((_b) = (_E),(_e) = (_B))   \
                               ) ,(_WH) = (_e) -(_b)        \
                             )
    try
    {
        if ((beginY < 0) | (beginY >= _height))
            throw 0;
        if ((beginX < 0) | (beginX >= _width))
            throw 0;
        dp_beginX = beginX;
        dp_beginY = beginY;
    }
    catch(...)
    {
        printf("In readNline ,You set beginY:%d is out Range!\n", beginY);
        return false;
    }
    if (height == 0 || width == 0)//will return all data
    {
        //dp_Data = _Data;
        dataDup2(dp_Data,_Data);
        dp_width = _width;
        dp_height = _height;
        return true;
    }
    else//read part of image with width and height
    {
        int endX = REPAIR(beginX + width, _width);
        int endY = REPAIR(beginY + height, _height);
        int bx = 0, ex = 0;
        WHY(beginX,endX,bx,ex,width);
        //(endX > beginX) ? (bx = beginX,ex = endX,width = ex - bx) : (bx = endX, ex = beginX,width = ex - bx);
        int by = 0, ey = 0;
        WHY(beginY,endY,by,ey,height);
        //(endY > beginY) ? (by = beginY,ey = endY,height = ey - by) : (by = endY, ey = beginY,height = ey - by);

        //ppPIXELS readData = new pPIXELS[height];
        //dp_Data = (ppPIXELS)malloc(height*sizeof(pPIXELS));
        dp_Data = PIXELS::allocator(height,width);
        int line = 0;
        for (int y = by; y < ey; ++y)	// y is row number
        {
            line = y - by;
            //readData[line] = new PIXELS[width];
            //dp_Data[line] = (pPIXELS)malloc(width*sizeof(PIXELS));
            for (int x = bx; x < ex; ++x)
            {
                //readData[line][x-bx] = _Data[y][x];
                dp_Data[line][x-bx] = _Data[y][x];
                //cout << readData[line][x-bx];
                //printf("\n");
            }
        }
        //dp_Data = readData;
        dp_width = width;
        dp_height = height;
        return true;
    }
#undef WHY
//#undef RESET
}

/*
 * write bmp image
 *
 */
bool    BmpPer::setHead(BMPALLHEAD& newhead,int W,int H)
{
    if(W <= 0 || H <= 0)
    {
        printf("In %s W is %d H is %d!\n",__FUNCTION__,W,H);
        return false;
    }
    dp_width = W;
    dp_height = H;
    newhead.infoHead.biWidth = W;
    newhead.infoHead.biHeight = H;
    int lineByte = (W * biBitCount + 31) / 32 * 4;
    newhead.infoHead.biSizeImage = H * lineByte;
    newhead.bmpHead.bfSize = H * lineByte + 54;
    //printf("allhead:%p,lineByte:%d\n",&allhead,lineByte);
    return true;
}

bool    BmpPer::write(const char *outpath,bool isOverWrite)
{
    if(!dp_Data)
    {
        printf("no Data to be write!\n");
        return false;
    }
    _omagePath = outpath;
    fpo = fopen(_omagePath.c_str(), "wb");
    fseek(fpo, 54, 0);		// 跳过allhead
    if(isOverWrite)
    {
        encoder(dp_width,dp_height,dp_beginX,dp_beginY);
    }
    else//will new a image to save
    {
        encoder(dp_Data,dp_width,dp_height);
    }
    rewind(fpo);
    switch (biBitCount)
    {
        case 24:
            fwrite(&allhead, bfOffBits, 1, fpo);
            break;
        case 8:
            // 申请颜色表所需要的空间，读颜色表进内存
            // fread(pColorTable,sizeof(iColor),256,fp);
            fwrite(&allhead, 54, 1, fpo);
            fwrite(pColorTable, sizeof(iColor) * 256, 1, fpo);
            break;
        default:
            break;
    }
    printf("write OK!\n");
    return true;
}

bool    BmpPer::encoder(ppPIXELS& allData,int width,int height)
{
    setHead(allhead,width,height);
    int lineByte = (width * biBitCount + 31) / 32 * 4;
    BYTE8 *linedata = new BYTE8[lineByte];
    if (pColorTable)
    {
        memset(pColorTable, 0, sizeof(iColor) * 256);
    }
    bool state;
    int x, k;
    for (int y = (height - 1); y >= 0; y--)
    {
        for (x = 0, k = 0; x < width; x++, k++)
        {
            //printf("x:%d\n",x);
            switch (biBitCount)
            {
                case 24:
                    allData[y][x].setData(linedata[k], linedata[k + 1], linedata[k + 2]);
                    k += 2;
                    break;
                case 8:
                    state = addColorTable(allData[y][x], linedata[k]);
                    if (state)
                        linedata[k] = state;
                    break;
                default:
                    break;
            }
        }
        fwrite(linedata, lineByte, 1, fpo);
    }
    // free memory
    if (linedata)
    {
        delete[]linedata;
        linedata = NULL;
    }
    return true;
}

bool    BmpPer::encoder(int width,int height,int beginX,int beginY)
{
#define IN(x,y,bx,by,ex,ey)  ((x >= bx) && (x < ex) && \
                              (y >= by) && (y < ey) )
#define RESET(_t,_T)         ((_t > _T ) ? (_T) : (_t))
    width = RESET(width,dp_width);
    height = RESET(height,dp_height);
    int endX = beginX + width;
    int endY = beginY + height;
    int lineByte = (_width * biBitCount + 31) / 32 * 4;
    BYTE8 *linedata = new BYTE8[lineByte];
    memset(linedata, 0, lineByte);
    if (pColorTable)
    {
        memset(pColorTable, 0, sizeof(iColor) * 256);
    }
    bool state;
    ppPIXELS allData = NULL;
    int x, k ,nx, ny;
    //printf("WWW:%d HHH:%d\n", W, H);
    for (int y = (_height - 1); y >= 0; y--)
    {
        for (x = 0, k = 0; x < _width; x++, k++)
        {
            if(IN(x,y,beginX,beginY,endX,endY))
            {
                allData = dp_Data;
                nx = x - beginX;
                ny = y - beginY;
            }
            else
            {
                allData = _Data;
                nx = x ,ny = y;
            }
            switch (biBitCount)
            {
                case 24:
                    allData[ny][nx].setData(linedata[k], linedata[k + 1], linedata[k + 2]);
                    k += 2;
                    break;
                case 8:
                    state = addColorTable(allData[ny][nx], linedata[k]);
                    if (state)
                        linedata[k] = state;
                    break;
                default:
                    break;
            }
        }
        fwrite(linedata, lineByte, 1, fpo);
    }
#undef IN
#undef RESET
    // free memory
    if (linedata)
        delete[]linedata;
    return true;
}

bool    BmpPer::write(const char* outpath,int width,int height,ppPIXELS data)
{
    if(!_Data && !dp_Data)
    {
        initHead(width,height);
    }
    if(!data)
    {
        printf("will create a blank bmp image\n");
        data = dp_Data = PIXELS::allocator(height,width);
    }
    _omagePath = outpath;
    fpo = fopen(_omagePath.c_str(), "wb");
    fseek(fpo, 54, 0);		// 跳过allheade
    encoder(data,width,height);
    rewind(fpo);
    switch (biBitCount)
    {
        case 24:
            fwrite(&allhead, bfOffBits, 1, fpo);
            break;
        case 8:
            // 申请颜色表所需要的空间，读颜色表进内存
            // fread(pColorTable,sizeof(iColor),256,fp);
            fwrite(&allhead, 54, 1, fpo);
            fwrite(pColorTable, sizeof(iColor) * 256, 1, fpo);
            break;
        default:
            break;
    }
    printf("write OK!\n");
    return true;

}

bool    BmpPer::initHead(int width,int height)
{
	if(width <= 0|| height <= 0)
		return false;
    //颜色表大小，以字节为单位，灰度图像颜色表为1024字节，彩色图像颜色表大小为0
    int colorTablesize=0;
    if(biBitCount == 8)
        colorTablesize = 1024;
    //待存储图像数据每行字节数为4的倍数
    //int lineBYTE=(width * biBitCount/8+3)/4*4;
    int lineBYTE=(width*biBitCount+31)/32*4;
    //1.申请位图文件头结构变量，填写文件头信息
    head.bfType = 0x4D42;
    //bmp类型
    //bfSize是图像文件4个组成部分之和
    head.bfSize = sizeof(BITMAPFILEHEADER)+
        sizeof(BITMAPINFOHEADER)+ colorTablesize +
        lineBYTE * height;
    head.bfReserved1 = 0;
    head.bfReserved2 = 0;
    //bfOffBits是图像文件前3个部分所需空间之和
    head.bfOffBits = 54 + colorTablesize;
    //2.申请位图信息头结构变量，填写信息头信息
    infohead.biBitCount = biBitCount;
    infohead.biClrImportant = 0;
    infohead.biClrUsed = 0;
    infohead.biCompression = 0;
    infohead.biHeight = height;
    infohead.biPlanes = 1;
    infohead.biSize = 40;
    infohead.biSizeImage = lineBYTE*height;
    infohead.biWidth = width;
    infohead.biXPelsPerMeter = 0;
    infohead.biYPelsPerMeter = 0;
    //3.set allhead
    allhead.bmpHead = head;
    allhead.infoHead = infohead;

    _width = width;
    _height = height;
    bfOffBits = 54 + colorTablesize;
	return true;
}

void    BmpPer::showData()
{
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            cout << _Data[y][x];
            printf("\n");
        }
    }
}

/*
 * bmp image utils
 *
 */
PIXELS  BmpPer::getPix(int x, int y) throw(IException)
{
#define OUTIMAGE(_X,_Y,_W,_H) ((_X >= _W) || (_Y >= _H) || (_X < 0) || (_Y < 0))
    if(! OUTIMAGE(x,y,_width,_height))
        return _Data[y][x];
    else
        throw("Out of image Ranges");
#undef  OUTIMAGE
}

bool    BmpPer::addColorTable(PIXELS pixel, BYTE8 & linedata)
{
    int i = 0;
    while (i < 256)
    {
        // pColorTable[i++] == pixel.getColor()
        if (pColorTable[i].blue() == pixel.getColor().blue() &&
                pColorTable[i].red() == pixel.getColor().red() &&
                pColorTable[i].green() == pixel.getColor().green())
        {
            linedata = i;
	    return true;
        }
        i++;
    }
    return false;
    /*
    pColorTable[globalI] = pixel.getColor(); printf("new颜色表:%d
            ",globalI); cout << pixel; printf("\n"); globalI++;
    */
}

bool    BmpPer::dataDup2(ppPIXELS& desData, ppPIXELS &srcData)
{
    return PIXELS::copier(desData,srcData,_height,_width);
    /*
    // tmpData = new pPIXELS[_height];
    tmpData = (ppPIXELS)calloc(_height, sizeof(pPIXELS));
    if(!tmpData)
    {
        printf("In %s calloc memory is failed\n",__FUNCTION__);
    }
    for (int y = 0; y < _height; y++)
    {
        // tmpData[y] = new PIXELS[_width];
        tmpData[y] = (pPIXELS)calloc(_width, sizeof(PIXELS));
        if(!tmpData[y])
        {
            printf("In %s[y] calloc memory is failedp\n",__FUNCTION__);
        }
        memcpy(tmpData[y], imageData[y], sizeof(PIXELS) * _width);
    }
    */
}

bool    BmpPer::delData(ppPIXELS& imageData, int H)
{
    return PIXELS::releaser(imageData,H);
    /*
    //printf("H:%d %p\n",H,imageData);
    if (imageData && H > 0)
    {
        for (int i = 0; i < H; i++)
        {
            // delete []imageData[i];
            free(imageData[i]);
        }
        // delete []imageData;
        free(imageData);
        imageData = NULL;
    }
    else
    {
        //printf("In delImageData imageData = NULL\n");
    }
    return true;
    */
}

bool    BmpPer::ProcesReq(const char* dealType)
{
    if(dp_Data == NULL)
    {
        dp_Data = _Data;
        dp_width = _width;
        dp_height = _width;
    }
    printf("%s",dealType);
    return true;
}

}//namespace DPer
}//namespace Imaginer
