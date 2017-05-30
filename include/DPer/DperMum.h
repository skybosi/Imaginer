/*
 * Data Provider >> Dper  (D is the most important)
 * get DATA from from diffrent image type to generate Data source :
 * jus like .bmp,.jpeg(.jpg),gif......
 * The Data Provider will provider such Data:
 *     1.ppPIXLES imageData
 *     2.int imageWidth
 *     3.int imageHeight
 *     4.Opearator Command
 * Dper should have image's path and output path
 *
 */

#ifndef _DPER_H_
#define _DPER_H_
#include "iPixels.h"

namespace Imaginer
{
using namespace Imaginer::MetaData;
namespace DPer
{

typedef struct RGBAXY
{
    int x;
    int y;
    int rgba;
}RGBA;
class DperMum
{
protected:
    string    _imagePath;
    string    _omagePath;
    //char*   OpertCmd;
    //all data
    int       _width;
    int       _height;
    ppPIXELS  _Data;
    //part of data
    int       dp_beginX;
    int       dp_beginY;
    int       dp_width;
    int       dp_height;
    ppPIXELS  dp_Data;
public:
    DperMum(const DperMum& dp){
        _imagePath = dp._imagePath;_omagePath = dp._omagePath ;
        _width = dp._width;_height = dp._height;_Data = dp._Data;
        dp_beginX = dp.dp_beginX; dp_beginY =dp.dp_beginY;
        dp_width = dp.dp_width;dp_height = dp.dp_height;dp_Data = dp.dp_Data;
    }
    DperMum(const char *imagepath):_imagePath(imagepath),_omagePath(""),
                                   _width(0),_height(0),_Data(NULL),
                                   dp_beginX(0),dp_beginY(0),
                                   dp_width(0),dp_height(0),dp_Data(NULL){}
    virtual ~DperMum(){}
    //processing request
    virtual bool ProcesReq(const char* dealType = NULL) = 0;
public:
    virtual ppPIXELS&   getData(){return dp_Data;}
    virtual int&        getWidth(){return dp_width;}
    virtual int&        getHeight(){return dp_height;}
    virtual int&        Width(){return _width;}
    virtual int&        Height(){return _height;}
    virtual void        setData(ppPIXELS data){dp_Data = data;}
    virtual void        setWidth(int width){dp_width = width;}
    virtual void        setHeight(int height){dp_height = height;}
public:
    /**
     * @brief read
     *        read a exist image(if not exist,return false),You can read a part of it,you must set a begin point,
     *        also you can set the size you want get (width,and height,must bigger 0),default read all image data
     * @param width
     * @param height
     * @param beginX
     * @param beginY
     * @return
     */
    virtual bool  read(int width,int height,int beginX = 0,int beginY = 0) = 0;
public:
    /**
     * @brief write
     * @param outpath
     * @param isOverWrite
     *                true -> will over write the read part of origin After deal with
     *                false-> will crete a new image to save the dealed with [ default isOverWrite = false ]
     * @return
     */
    virtual bool write(const char* outpath,bool isOverWrite = false) = 0;
    /**
     * @brief write
     *        write a ppPIXELS data to image file,just liake create a image file
     * @param outpath
     * @param data
     * @param width
     * @param height
     * @return
     */
    virtual bool write(const char* outpath,int width,int height,ppPIXELS data = NULL) = 0;
private://utils
};

}//namespace DPer
}//namespace Imaginer

#endif // Dper.h :[]

