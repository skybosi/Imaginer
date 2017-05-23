/*
 * bmp Data Provider >> Dper  (bmp D is the most important)
 * This is a subClass of DperMum,is just to r/w *.bmp(24) format image
 * provider read and write method to get Data from *.bmp image file
 *
 *    NOTE: Now(2016-12-99:22) It's just can read/write 24bit Bmp image file
 */
#ifndef _BMPPER_H_
#define _BMPPER_H_
#include <stdlib.h>
#include "DperMum.h"

namespace Imaginer
{
namespace DPer
{

class BmpPer :public DperMum
{
private:
    FILE* fpi;
    FILE* fpo;
    int   bfOffBits;
    int   biBitCount;//图像类型，每像素位数
    iBMPALLHEAD allhead;
    iBITMAPFILEHEADER head;
    iBITMAPINFOHEADER infohead;
    iColor* pColorTable;//颜色表指针
public:
    BmpPer(int width,int height,bool grid = false);
    BmpPer(const char* imagepath, char* mode = "rb", int width = 500,int height = 500,bool grid = false);
    ~BmpPer();
    //Processing request
    /**
     * @brief ProcesReq
     *        send a command to DPC to chose witch deal method
     * @param dealType
     * @return
     */
    bool     ProcesReq(const char* dealType = NULL);

public://read
    /**
     * @brief read
     *        read a part of image file,default all image,if you set width height and begin point
     *    you will get this part of the image,if you set those value if invaild will auto reset in the function
     * @param width
     * @param height
     * @param beginX
     * @param beginY
     * @return
     */
    bool     read(int width = 0,int height  = 0,int beginX = 0,int beginY = 0);

public://write
     /**
     * @brief write
     *        write the image data(default _Data/dp_Data) to a file that you set out path
     * @param outpath
     * @param isOverWrite
     * @return
     */
    bool     write(const char* outpath,bool isOverWrite = false);
    /**
     * @brief write
     *        write the image data(which You want to  write) to a file that you set out path
     * @param outpath
     * @param data
     * @param width
     * @param height
     * @return
     */
    bool     write(const char* outpath,int width,int height,ppPIXELS data = NULL);
public:
    /**
     * @brief showData
     *        show all pixel info. ,just for debug
     */
    void       showData();
    /**
     * @brief getPix
     *        get the (x,y) pixel info
     * @param x
     * @param y
     * @return
     */
    PIXELS     getPix(int x,int y)throw(IException);

private:
    /**
     * @brief decoder
     *        decode a image file to a ppPIXELS when open or read a image
     * @return
     */
    bool     decoder();
    /**
     * @brief getHead
     *        read a format image head info.
     * @return
     */
    bool     getHead();
    /**
     * @brief encoder
     *        Write the iamge Data to a image file each pixel
     * @param allData
     * @param width
     * @param height
     * @return
     */
    bool     encoder(ppPIXELS& allData,int width = 0,int height = 0);
    /**
     * @brief initHead
     * @param width
     * @param height
     * @return
     */
    bool     initHead(int width,int height);
    /**
     * @brief encoder
     *        Write the image Data to a image file each pixel,from a position,just like read
     * @param width
     * @param height
     * @param beginX
     * @param beginY
     * @return
     */
    bool     encoder(int width = 0,int height  = 0,int beginX = 0,int beginY = 0);
    /**
     * @brief setHead
     *        Set a image allhead info
     * @param newhead
     * @param W
     * @param H
     * @return
     */
    bool     setHead(iBMPALLHEAD& newhead,int W,int H);
    /**
     * @brief addColorTable
     *        Add or Create ColorTable if the bmp is 256 or smaller (NOT WORK)
     * @param pixel
     * @param linedata
     * @return
     */
    bool     addColorTable(PIXELS pixel,BYTE8& linedata);
    /**
     * @brief dataDup2
     *        copy some imageData to other where you want
     *        NOTE: if have pointer will occur memory leak
     * @param imageData
     * @param tmpData
     */
    bool     dataDup2(ppPIXELS& desData, ppPIXELS &srcData);
    /**
     * @brief delData
     *        free some imageData you malloc/calloc/new
     * @param imageData
     * @param H
     * @return
     */
    bool     delData(ppPIXELS& imageData, int H);
};

}//namespace DPer
}//namespace Imaginer

#endif // BmpPer.h :[]
