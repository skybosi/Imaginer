/*
 * Data Processing Center >> DPC  (P is the most important)
 * deal with Data source from diffrent image type:
 * jus like .bmp,.jpeg(.jpg),gif......
 * The Data Souroce must Provider such Data:
 *     1.ppPIXLES imageData
 *     2.int imageWidth
 *     3.int imageHeight
 *     4.Opearator Command
 * will return the dealed imageData,and imageWidth,imageHeight
 */

#ifndef _DPC_H_
#define _DPC_H_
#include <vector>
#include <deque>
#include <map>
#include <stdlib.h>
#include "iPixels.h"
#include "DperMum.h"

#ifdef WIN32
    #include <Windows.h>
#endif // _WINDOWS


using namespace Imaginer::DPer;
//using namespace Imaginer;
namespace Imaginer
{
namespace DPC
{

enum Method
{
    UD,
    LR,
    UR,
    NONE
};

//boundary point's Catagory,I = Imaginer
#define IBASE   0
#define IPOLE  -1
#define IEDGE  -2

//virtual class
class dataPcer
{
protected:
    DperMum*  _dp;
    ppPIXELS  _Data;
    int       _width;
    int       _height;
    char*     _OpertCmd;
    int       _beginX;
    int       _beginY;

protected:
    dataPcer(ppPIXELS& data,int& width,int& height):_dp(NULL),
        _Data(data),_width(width),_height(height),_OpertCmd(NULL){
        _beginX = _Data[0][0].getX();_beginY = _Data[0][0].getY();//矫正值
    }
    dataPcer(DperMum* dp):_dp(dp){
        //_Data = _dp->getData();_width = _dp->getWidth();_height = _dp->getHeight();
        //_beginX = _Data[0][0].getX();_beginY = _Data[0][0].getY();//矫正值
        initData(_dp->getData(),_dp->getWidth(),_dp->getHeight());
    }
    virtual ~dataPcer(){printf("out the dataPcer\n");}
    virtual bool dealManager(const char* dealType){ return false;}

public://return all Data
    int      retnWidth();
    int      retnHeight();
    ppPIXELS retnData();

protected: //base
    /**
     * @brief newData
     *        new/malloc some memory to a save the image data will be deal with
     * @param imageData
     * @param W  the row Create two-dimensional array
     * @param H  the col Create two-dimensional array
     * @return
     */
    bool    newData(ppPIXELS& imageData,int W,int H);
    /**
     * @brief dataDup2
     *        Copy the imageData to tmpimageData
     * @param imageData    source
     * @param tmpimageData destination
     * @return
     */
    bool    dataDup2(ppPIXELS& imageData,ppPIXELS& tmpimageData);
    /**
     * @brief initData
     *        init some value ,lisk _width,_height,_beginX,_beginY
     * @param data
     * @param width
     * @param height
     * @return
     */
    bool    initData(ppPIXELS data,int width,int height);
    /**
     * @brief delData
     *        delete/free the memory image Data after deal with
     * @param imageData
     * @param H
     * @return
     */
    bool    delData(ppPIXELS& imageData,int H);
    /**
     * @brief clearData
     *        clear ppPIXELS each pixel to iColor::NCOLOR
     * @param imageData
     * @param H
     * @return
     */
    bool    clearData(ppPIXELS& imageData, int H,int color = iColor::WHITE);
    /**
     * @brief isEdge
     *        test (x,y) is edge of the image or not
     * @param x
     * @param y
     * @return
     */
    bool    isEdge(int x,int y);
    /**
     * @brief getPix
     *        get a point RGB value from the (x,y)
     *        if the x,y is out range ,return empty PIXELS
     *        get PIXELS with a point(x,y),maybe add z
     * @param x
     * @param y
     * @return
     */
    PIXELS getPix(int x,int y)throw(IException);
};

}//namespace DPC
}//namespace Imaginer

#endif // dataPcer.h :[]
