#include "dataPcer.h"

namespace Imaginer
{
namespace DPC
{

/*
 * init  data
 *
 */
bool     dataPcer::initData(ppPIXELS data,int width,int height)
{
    if(!data)
    {
        printf("Data provide is fair!\n");
        return false;
    }
    _Data   = data;
    _width  = width;
    _height = height;
    _beginX = _Data[0][0].getX();_beginY = _Data[0][0].getY();//矫正值
    return true;
}

bool     dataPcer::newData(ppPIXELS& imageData, int W, int H)
{
    imageData = PIXELS::allocator(W,H);
    return (imageData != NULL);
    /*
    // malloc some memroy
    // imageData = new pPIXELS[W];
    imageData = (ppPIXELS)malloc(H*sizeof(pPIXELS));
    if(!imageData)
    {
        printf("In %s new imageData wrong!\n",__FUNCTION__);
        return false;
    }
    // printf("new:(W,H):%d,%d\n",W,H);
    int y;
    for (y = 0; y < H; y++)
    {
        // _Data[y] = new PIXELS[W];
        imageData[y] = (pPIXELS)malloc(W*sizeof(PIXELS));
        if (!imageData[y])
        {
            printf("In %s new _Data[y] wrong!\n",__FUNCTION__);
            return false;
        }
    }
    return true;
    */
}

bool     dataPcer::dataDup2(ppPIXELS& imageData, ppPIXELS &tmpData)
{
    return PIXELS::copier(tmpData,imageData,_height,_width);
    /*
    // tmpData = new pPIXELS[_height];
    tmpData = (ppPIXELS)calloc(_height, sizeof(pPIXELS));
    if(!tmpData)
    {
        printf("In %s calloc memory is failed\n",__FUNCTION__);
        return false;
    }
    for (int y = 0; y < _height; y++)
    {
        // tmpData[y] = new PIXELS[_width];
        tmpData[y] = (pPIXELS)calloc(_width, sizeof(PIXELS));
        if(!tmpData[y])
        {
            printf("In %s[y] calloc memory is failedp\n",__FUNCTION__);
            return false;
        }
        memcpy(tmpData[y], imageData[y], sizeof(PIXELS) * _width);
    }
    return true;
    */
}

bool     dataPcer::delData(ppPIXELS& imageData, int H)
{
    return PIXELS::releaser(imageData,H);
    /*
    //printf("H:%d %p\n",H,imageData);
    if (imageData)
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
        //printf("In delData imageData = NULL\n");
    }
    return true;
    */
}

bool     dataPcer::clearData(ppPIXELS& imageData, int H,int color)
{
    //printf("H:%d %p\n",H,imageData);
    if (!imageData)
        return false;
    for (int i = 0; i < H; i++)
    {
        memset(imageData[i],255,sizeof(PIXELS)*_width);
    }
    return true;
}

bool     dataPcer::isEdge(int x,int y)
{
    if(x <= 0 || x >= _width ||
            y <= 0 || y >= _height)
        return true;
    else
        return false;
}

PIXELS   dataPcer::getPix(int x, int y) throw(IException)
{
#define OUTIMAGE(_X,_Y,_W,_H) ((_X >= _W) || (_Y >= _H) || (_X < 0) || (_Y < 0))
    if(! OUTIMAGE(x,y,_width,_height))
        return _Data[y][x];
    else
    {
        throw IException("out of image RANGE!");
    }
#undef  OUTIMAGE
}

/*
 * dataPcer deal Manager and return Data
 *
 */

int      dataPcer::retnWidth()
{
    return _width;
}

int      dataPcer::retnHeight()
{
    return _height;
}

ppPIXELS dataPcer::retnData()
{
    return _Data;
}

}//namespace DPC
}//namespace Imaginer
