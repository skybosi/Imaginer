/*
 * Data Processing Center Common >> DPCcomm  (P is the most important)
 * This class just provide some common Process method:
 * like:
 *     move,mirror,zoom ...
 * just a show method,but the spherize and spatialize maybe a interested method
 */
#ifndef DPCCOMM_H
#define DPCCOMM_H
#include "dataPcer.h"

namespace Imaginer
{
namespace DPC
{
#define MATRIX 1

class dpcComm :public dataPcer
{
public:
    dpcComm(ppPIXELS& data,int& width,int& height):dataPcer(data,width,height){}
    dpcComm(DperMum* dp):dataPcer(dp){}
    ~dpcComm(){}
    virtual bool dealManager(OPt& opt);
    static const char*  doc();
private://common function
    //Function: generate the image's bar diagram
    //@ imageData: will dealwith data source
    //@ color    : chose a color,default Pricolor
    bool     genBardiagram(ppPIXELS& allData,colorType color = Pricolor,bool isOverWrite = true);
    //Function: generate the image's Histogram
    //@ allData  : will dealwith data source
    //@ color    : chose a color,default Pricolor
    bool     genHistogram(ppPIXELS& allData,colorType color = Pricolor,bool isOverWrite = true);
    //Function: Move the image
    // move the image:x>0,y>0 ->right down ;x<0 y<0 -> left up
    //@ allData   : will dealwith data source
    //@ mx        : The distance at x direction move
    //@ my        : The distance at y direction move
    ppPIXELS move(ppPIXELS& allData,int mx = 0,int my = 0);
    //Function: Mirror the image
    //@ allData   : will dealwith data source
    //@ method    : Mirror method (see enum Method)
    ppPIXELS mirror(ppPIXELS& allData,Method method = NONE);
    //Function: get 3 Color(RGB)
    //@ allData   : will dealwith data source
    //@ color     : The color (see enum colorType)
    ppPIXELS getImage3Color(ppPIXELS& allData,colorType color = Pricolor);
    //Function: Zoom the image
    //@ allData   : will dealwith data source
    //@ scalex    : The zoom rate at x direction move
    //@ scaley    : The zoom rate at y direction move
    ppPIXELS zoom(ppPIXELS& allData,float scalex = 1.0,float scaley = 1.0);
    //Function: Spherize the image
    //@ radius    : Spherize's radius
    //@ allData   : will dealwith data source
    //NOTE: if radius = 0,will Adhered with imageWidth and bmpWidth (oval)
    //else will Adhered with a circle of radius
    ppPIXELS spherize(ppPIXELS& allData,float radius = 0.0);
    //Function: Transpose the image
    //@ allData   : will dealwith data source
    //@ AR        : Antegrade(TRUE) and retrograde(FALSE)
    ppPIXELS transpose(ppPIXELS& allData,bool AR = true);
    //Function: Revolution the image
    //@ allData   : will dealwith data source
    //@ px        : The Revolution point set x
    //@ py        : The Revolution point set y
    //@ angle     : The Revolution angle(+ Antegrade, - retrograde)
    ppPIXELS revolution(ppPIXELS& allData,int px = 0,int py = 0,float angle = 90.0);
    //Function: Shear the image
    //@ allData   : will dealwith data source
    //@ XorY      : Shear the at x or y direction(TRUE :x FALSE: y)
    //@ angle     : The Shear angle(+ Up/Right, - Down/left)
    ppPIXELS shear(ppPIXELS& allData,bool XorY = true,float angle = 45.0);
    //Function: spatialize the image
    //  [0] : up [1] : down [2] : left [3] : right [4] : front [5] : back
    ppPIXELS spatialize(string outpath);//not work
    //Function: Change a image each pixel's Idensity
    //@ allData   : will dealwith data source
    //@ scala     : Thick  or thin scale
    ppPIXELS density(ppPIXELS& allData,float scale);
    /**
     * @brief gray
     *        togray the image
     * @param allData
     * @return
     */
    ppPIXELS gray(ppPIXELS& allData);
    /**
     * @brief colorful
     * @param allData
     * @return
     */
    ppPIXELS colorful(ppPIXELS& allData);
    /**
     * @brief change
     * @param allData
     * @param method
     * @return
     */
    ppPIXELS change(ppPIXELS& allData,const iMatrix& method);
    /**
     * @brief perspective
     * @param allData
     * @param p
     * @param q
     * @param r
     * @return
     */
    ppPIXELS perspective(ppPIXELS& allData,float p,float q,float r);
};

}//namespace DPC
}//namespace Imaginer

#endif // DPCCOMM_H
