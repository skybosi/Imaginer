/*
 * This is a very important and base class to save a PIXELS the base element of the project
 * The class provide some essential method to get, save, reset and flag each pixel from image
 * All deal with is base of it. so ,this class Determines the superstructure
 */

#ifndef _IPIXELS_H_
#define _IPIXELS_H_
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include "ibmp.h"
#include "iCommons.h"
#include "iColor.h"
#include "iVector.h"
using namespace std;

using namespace Imaginer::Utils;
using namespace Imaginer::Commons;
using namespace Imaginer::BaseData;
namespace Imaginer {
namespace MetaData {
class PIXELS
{
public:    
    typedef PIXELS*  pPIXELS;
    typedef pPIXELS* ppPIXELS;
    typedef std::pair<Position,U8> pix_p;
private:
    iPoint  ppoint;
    iColor  prgb;
    U8      rgb_threshold;
    int     bEdge;				// edge point
    //bool  bEmpty;             //the pixel is NULL ,is false
    pix_p   pix_P;
public:
    PIXELS();
    PIXELS(const iColor& rgb);
    PIXELS(const iPoint& point,const iColor& rgb = iColor::WHITE);
    PIXELS(const iColor& rgb,int x,int y = -1);
    PIXELS(int rgba,int x = -1,int y = -1);
    PIXELS  operator~();        //opposition the point color
    PIXELS& operator=(const PIXELS& pixel);
    PIXELS& operator=(const iPoint& pixel);
    PIXELS& operator=(const iColor& pixel);
    PIXELS& operator=(const int& pixel);
    PIXELS& operator+=(const iVector& where);
    PIXELS  operator+(const iColor& rgb);
    PIXELS  operator+(const PIXELS& pixel);
    PIXELS  operator-(const PIXELS& pixel);
    PIXELS  operator/(const float& scale);
    PIXELS  operator*(const float& scale);
    PIXELS  operator*(const iMatrix& mtx);
    friend  const PIXELS operator-(const PIXELS& pixel1,const PIXELS& pixel2);
    friend  const PIXELS operator+(const iColor& rgb,const PIXELS& pixel);
    friend  const PIXELS operator*(const float& scale,PIXELS& pixel);
    friend  const PIXELS operator/(const float& scale,PIXELS& pixel);
    friend  std::ostream& operator<<(std::ostream &os,const PIXELS& pixel);
    bool    operator==(const PIXELS& pixel);
    bool    operator!=(const PIXELS& pixel);
public:
    //set rgb
    PIXELS&  setRGBA(const PIXELS& ppot);
    PIXELS&  setRGBA(const iColor& rgba);
    PIXELS&  setRGBA(const int& rgba);
    PIXELS&  setRGBA(U8 r,U8 g,U8 b,U8 a = 255);
    bool     setData(BYTE8& b,BYTE8& g,BYTE8& r);
    //set (x,y)
    PIXELS&  setXY(const PIXELS& pixel);
    PIXELS&  setXY(int x, int y);
    PIXELS&  resetXY(int x,int y);
    U8      set_threshold(const PIXELS& ppot);
    void    setempty(bool state = false);
    void    setEdge(int bedge = 0);
    //set pix_P
    //@pos    : the point position(default None)
    //@rtimes : reference times   (default 0)
    void    initpPos();
    void    setPostion(Position pos = None,bool isCorner = false);
    bool    isCorner();
public:
    //get PIXELS info
    U8      getRed()const;
    U8      getGreen()const;
    U8      getBlue()const;
    U8      getAlpha()const;
    int     getX()const;
    int     getY()const;
    int     getZ()const;
    int     getEdge()const;
    iColor  getColor()const;
    int     color()const;
    iColor  get_diff8RGB(PIXELS ppot)const;
    //get PIXELS state
    bool    empty()const;
    bool    isEdge(PIXELS& pixel, int W,int H);
    bool    isEdge(int W,int H);
    //change direction or not
    int     getpPosValues()const;
    bool    getpPosStatus()const;
    pix_p   getpPos()const;
    Position getPostion()const;
public://alloc,release
    static pPIXELS   allocator(int cols);
    static bool      releaser(pPIXELS& data);
    static ppPIXELS  allocator(int rows,int cols,bool grid = false, int dx = 5,int dy = 5,float percent = 0.8);
    static bool      releaser(ppPIXELS& data,int rows);
    static bool      copier(ppPIXELS& des,ppPIXELS& src, int rows,int cols);
    static bool      drawRecCoordina(ppPIXELS& data,const int& width,const int& height,const iPoint& origin,float percent = 0.9);
public://utils
    iPoint   toPoint();
    //binaryzation image
    void    toBin();
    void    toBin(PIXELS& ppot);
    PIXELS& toGray();
    PIXELS& toColor();
    PIXELS& get3Color(colorType color);
    //mix two color
    PIXELS  mix(PIXELS& p1,PIXELS& p2,U8 weight = 50);
    void    fix_PIXELS(int W,int H);//fix up the point position
private:
    int     pixel2rgba(PIXELS& pixel);
    PIXELS  rgba2pixel(int x,int y,int ipixel);
};
// pixel point
class PIXPOT
{
private:
    PIXELS pot;
    PIXELS pot4s[4];				// 4 side point
    PIXELS pot4a[4];				// 4 angle point
    iColor diff4s[8];			// 4 side point rgb value diff with the fcspot
    iColor diff4a[8];			// 4 angle point rgb value diff with the fcspot
public:
    PIXPOT();
    //show the PIXPOT(include focus point,the other 8 point)
    void show_PIXPOT();
    //show the diff between focus point with other 8 point
    void show_PIXPOT8diffRGB(iColor diffRgb);
    //fix the PIXPOT's postion (include 4 side point and 4 angle point)
    void fix_PIXPOT(PIXPOT& pots8,int W,int H);//fix up the 8 point position
    //get the right point postion from resetXY
    PIXELS* get_pos8(PIXELS pixel,PIXELS* pos8,int W,int H);
    //set the right point postion
    PIXPOT& set_pots8(PIXELS* pos8);
    //get the diff value 4 side point
    iColor get_diff4s(int indexs);
    //get the diff value 4 angle point
    iColor get_diff4a(int indexa);
    //pixel similarity
    bool pixelSimilar();
    int  getEdges();
};

typedef PIXELS::pPIXELS  pPIXELS;
typedef PIXELS::ppPIXELS ppPIXELS;
typedef deque<PIXELS>    dPIXELS;
typedef vector<dPIXELS>  vdPIXELS;

}//namespace MetaData
}//namespace Imaginer

#endif // iPixels.h :[]
