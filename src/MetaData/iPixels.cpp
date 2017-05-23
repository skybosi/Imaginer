#include "iPixels.h"
#include <cmath>

namespace Imaginer {
namespace MetaData {

#define LOS(RGBvar) ((ABS((char)RGBvar.red()) <= 25) && \
                      (ABS((char)RGBvar.green()) <= 25) &&\
                      (ABS((char)RGBvar.blue()) <= 25))        //level of similarity
#define BETWEEN(_V,_S,_E) (((_V) > (_S)) && ((_V) < (_E)))
PIXPOT::PIXPOT()
{
    //memset(this,0,sizeof(PIXPOT));
}

void PIXPOT::fix_PIXPOT(PIXPOT& pots8,int W,int H)
{
    int i = 0;
    while(i<4)
    {
        pots8.pot4s[i].fix_PIXELS(W,H);
        pots8.pot4a[i].fix_PIXELS(W,H);
        i++;
    }
}


void PIXPOT::show_PIXPOT8diffRGB(iColor diffRgb)
{
    printf("diff:(%03d,%03d,%03d)\taverage:%-3.3f\t",
        (char)diffRgb.red(),
        (char)diffRgb.green(),
        (char)diffRgb.blue(),
        ((char)diffRgb.blue() + (char)diffRgb.green() + (char)diffRgb.red())/3.0);
    char min = MIN(MIN((char)diffRgb.red(),(char)diffRgb.green()),(char)diffRgb.blue());
    char max = MAX(MAX((char)diffRgb.red(),(char)diffRgb.green()),(char)diffRgb.blue());
    char diff;
    if((min < 0 && max < 0))
        diff = min - 0;
    else if( (min > 0 && max > 0))
        diff = 0 - max;
    else
    {
        diff = min - max;
    }
    printf("色差:%d\n",ABS(diff));
}

PIXPOT& PIXPOT::set_pots8(PIXELS* pos8)
{
    pot  = pos8[0];
    int i = 0;
    while(i< 4)
    {
        pot4s[i] = pos8[i+1];
        diff4s[i] = pot4s[i].get_diff8RGB(pos8[0]);
        pot4a[i] = pos8[i+5];
        diff4a[i] = pot4a[i].get_diff8RGB(pos8[0]);
        i++;
    }
    //show_PIXPOT();
    return *this;
}

void PIXPOT::show_PIXPOT()
{
    std::cout << pot;
    int i = 0;
    while(i<4)
    {
        std::cout << pot4s[i];
        show_PIXPOT8diffRGB(diff4s[i]);
        std::cout << pot4a[i];
        show_PIXPOT8diffRGB(diff4a[i]);
        i++;
    }
}

//get the diff value 4 side point
iColor PIXPOT::get_diff4s(int indexs)
{
    /*
    iColor tmp;
    memset(&tmp,255,sizeof(iColor));
    */
    iColor tmp(iColor::WHITE);
    if(indexs < 0 || indexs > 3)
        return tmp;
    else
        return diff4s[indexs];
}

//get the diff value 4 angle point
iColor PIXPOT::get_diff4a(int indexa)
{
    /*
    iColor tmp;
    memset(&tmp,255,sizeof(iColor));
    */
    iColor tmp(iColor::WHITE);
    if(indexa < 0 || indexa > 3)
        return tmp;
    else
        return diff4a[indexa];
}

//get 8 point position(x,y)
PIXELS* PIXPOT::get_pos8(PIXELS pixel,PIXELS* pos8,int W,int H)
{
    int i = 0;
    while(i<4)
    {
        //get 4 side point position(x,y)
        pot4s[i] = pixel;
        //get 4 angle point position(x,y)
        pot4a[i] = pixel;
        i++;
    }
    pot4s[0].resetXY(0,-1);
    pot4s[1].resetXY(1,0);
    pot4s[2].resetXY(0,1);
    pot4s[3].resetXY(-1,0);

    pot4a[0].resetXY(-1,-1);
    pot4a[1].resetXY(1,-1);
    pot4a[2].resetXY(1,1);
    pot4a[3].resetXY(-1,1);
    fix_PIXPOT(*this,W,H);
    pos8[0] = pixel;
//	cout << pos8[0];
//	printf("\n");
    i = 1;
    while(i < 5)
    {
        pos8[i] = pot4s[i-1];
//		cout << pos8[i];
//		printf("\n");
        pos8[i+4] = pot4a[i-1];
//		cout << pos8[i+4];
//		printf("\n");
        i++;
    }
//	show_PIXPOT();
    return pos8;
}

bool PIXPOT::pixelSimilar()
{
    int level = 0;
    int i = 0;
    while(i<4)
    {
        if(LOS(diff4a[i]))
            level++;
        if( LOS(diff4s[i]))
            level++;
        i++;
    }
    printf("level:%d\n",level);
    if(level >= 6)
        return true;
    else
        return false;
}

int  PIXPOT::getEdges()
{
    int i = 0;
    int Edges = 0;
    while(i<4)
    {
        Edges += pot4s[i].getEdge() + pot4a[i].getEdge();
        i++;
    }
    return Edges;
}

PIXELS::PIXELS():ppoint(),prgb(),rgb_threshold(128),bEdge(0),pix_P(None,0)
{
    //memset(&prgb,0,sizeof(iColor));
    //rgb_threshold  = 0;
}

PIXELS::PIXELS(int x, int y, int rgba):ppoint(x,y),prgb(rgba),rgb_threshold(128),bEdge(0),pix_P(None,0){}

PIXELS::PIXELS(const iColor& rgb):ppoint(),prgb(rgb),rgb_threshold(128),bEdge(0),pix_P(None,0)
{
    /*
    prgb.Blue() = rgb.blue();
    prgb.Green() = rgb.green();
    prgb.Red() = rgb.red();
    prgb.Alpha() = rgb.alpha();
    */
}

PIXELS::PIXELS(const iPoint& point,const iColor& rgb):ppoint(point),prgb(rgb),rgb_threshold(128),bEdge(0),pix_P(None,0)
{

}

PIXELS::PIXELS(const iColor& rgb,int x,int y):ppoint(x,y),prgb(rgb),rgb_threshold(128),bEdge(0),pix_P(None,0)
{
    /*
    prgb.Blue() = rgb.blue();
    prgb.Green() = rgb.green();
    prgb.Red() = rgb.red();
    prgb.Alpha() = rgb.alpha();
    */
}

//just copy the position
PIXELS& PIXELS::operator=(const PIXELS& pixel)
{
    if(this == NULL)
        printf("kaokao\n");
    if(&pixel == NULL)
        printf("TMDTMD\n");
    if(this == &pixel)
        return *this;
    ppoint = pixel.ppoint;
    this->bEdge = pixel.bEdge;
    this->pix_P = pixel.pix_P;
    //memcpy(&prgb,&pixel.prgb,sizeof(iColor));
    prgb = pixel.prgb;
    rgb_threshold = pixel.rgb_threshold;
    return *this;
}

PIXELS& PIXELS::operator=(const iPoint& point)
{
    ppoint = point;
    prgb = point.color();
    return *this;
}

PIXELS& PIXELS::operator=(const iColor& pixel)
{
    prgb = pixel;
    return *this;
}

PIXELS& PIXELS::operator=(const int& pixel)
{
    prgb = pixel;
    return *this;
}

//just change the (x,y)
PIXELS& PIXELS::operator+=(const iVector& where)
{
    ppoint.X() = where.X();
    ppoint.Y() = where.Y();
    return *this;
}

//opposition the point color
PIXELS  PIXELS::operator~()
{
    /*
    prgb.Red() = 255 - prgb.red();
    prgb.Green() = 255 - prgb.green();
    prgb.Blue() = 255 - prgb.blue();
    prgb.Alpha() = 255 - prgb.alpha();
    return *this;
    */
    return PIXELS(~prgb);
}

//just diff of two pixel
PIXELS PIXELS::operator-(const PIXELS& pixel)
{
    /*
    PIXELS diff;
    diff.pix_X = pix_X;
    diff.pix_Y = pix_Y;
    diff.prgb.Red() = abs(prgb.red() - pixel.prgb.red());
    diff.prgb.Green() = abs(prgb.green() - pixel.prgb.green());
    diff.prgb.Blue() = abs(prgb.blue() - pixel.prgb.blue());
    diff.prgb.Alpha() = abs(prgb.alpha() - pixel.prgb.alpha());
    return diff;
    */
    return PIXELS(ppoint,prgb-pixel.getColor());
}

//just diff of two pixel
const PIXELS operator-(const PIXELS& pixel1,const PIXELS& pixel2)
{
    /*
    PIXELS diff;
    diff.pix_X = pixel1.pix_X;
    diff.pix_Y = pixel1.pix_Y;
    diff.prgb.Red() = abs(pixel1.prgb.red() - pixel2.prgb.red());
    //printf("diff Red:%d\n",diff.prgb.red());
    diff.prgb.Green() = abs(pixel1.prgb.green() - pixel2.prgb.green());
    //printf("diff Green:%d\n",diff.prgb.green());
    diff.prgb.Blue() = abs(pixel1.prgb.blue() - pixel2.prgb.blue());
    //printf("diff Blue:%d\n",diff.prgb.blue());
    diff.prgb.Alpha() = abs(pixel1.prgb.alpha() - pixel2.prgb.alpha());
    return diff;
    */
    return PIXELS(pixel1.getColor()-pixel2.getColor(),pixel1.getX(),pixel1.getY());
}

//just add of two pixel
PIXELS PIXELS::operator+(const iColor& rgb)
{
    return operator +(iColor(rgb));
}

//just add of two pixel
PIXELS PIXELS::operator+(const PIXELS& pixel)
{
    return operator +(iColor(pixel.prgb));
}

//just add of two pixel
const PIXELS operator+(const iColor& rgb,const PIXELS& pixel)
{
    return operator +(iColor(rgb),pixel);
}

PIXELS PIXELS::operator*(const float& scale)
{
    return PIXELS(prgb*scale);
}

PIXELS  PIXELS::operator*(const iMatrix& mtx)
{
    ppoint = ppoint * mtx;
    return *this;
}

const PIXELS operator*(const float& scale,PIXELS& pixel)
{
    return PIXELS(pixel.prgb*scale);
}

PIXELS PIXELS::operator/(const float& scale)
{
    return PIXELS(prgb/scale);
}

const PIXELS operator/(const float& scale,PIXELS& pixel)
{
    return PIXELS(pixel.prgb/scale);
}

std::ostream & operator<<(std::ostream &os,const PIXELS& pixel)
{
    os << pixel.ppoint << "  "
       << "Edge : " << pixel.bEdge << "\t";
       //<< "[Position,rtime]" << Pos2str(pixel.pix_P.first) << "\t"
       //<< pixel.pix_P.first  << (pixel.pix_P.second & 0x7F) << "\t";
    os << pixel.prgb;
    return os;
}

//just compare pix_Y,pix_X
bool PIXELS::operator ==(const PIXELS& pixel)
{
    return (ppoint == pixel.ppoint);
}

bool PIXELS::operator !=(const PIXELS& pixel)
{
    return (ppoint != pixel.ppoint);
}

//get a threshold from a point ppot
U8 PIXELS::set_threshold(const PIXELS& ppot)
{
    U8 r = ppot.prgb.red() * 0.3;
    U8 g = ppot.prgb.green() * 0.59;
    U8 b = ppot.prgb.blue() * 0.11;
    rgb_threshold = (r + g + b)/3;
    return rgb_threshold;
}

inline PIXELS PIXELS::rgba2pixel(int x,int y,int ipixel)
{
    /*
    iColor rgba;
    rgba.alpha() = (ipixel & 0xFF000000) >> 24;
    rgba.red()      = (ipixel & 0x00FF0000) >> 16;
    rgba.green()    = (ipixel & 0x0000FF00) >> 8;
    rgba.blue()     = (ipixel & 0x000000FF);
    return PIXELS(rgba,x,y);
    */
    return PIXELS(iColor(ipixel),x,y);
}

inline int    PIXELS::pixel2rgba(PIXELS& pixel)
{
    return pixel.color();
}

PIXELS& PIXELS::get3Color(colorType color)
{
    switch(color)
    {
    case Red:
        prgb.clear(iColor::GN);
        prgb.clear(iColor::BL);
        break;
    case Green:
        prgb.clear(iColor::RE);
        prgb.clear(iColor::BL);
        break;
    case Blue:
        prgb.clear(iColor::RE);
        prgb.clear(iColor::GN);
        break;
    default:
        break;
    }
    return *this;
}

//get the diff RGB between the focus point with other 8 point
iColor PIXELS::get_diff8RGB(PIXELS pixel)const
{
    iColor diffRgb;
    diffRgb.Red()   = prgb.red() - pixel.prgb.red();
    diffRgb.Green() = prgb.green() - pixel.prgb.green();
    diffRgb.Blue()  = prgb.blue() - pixel.prgb.blue();
    diffRgb.Alpha() = prgb.alpha() - pixel.prgb.alpha();
    return diffRgb;
}

bool PIXELS::setData(BYTE8& r,BYTE8& g,BYTE8& b)
{
    r = prgb.red();
    g = prgb.green();
    b = prgb.blue();
    return true;
}

//set rgb with r g b
PIXELS& PIXELS::setRGBA(U8 r,U8 g,U8 b,U8 a)
{
    prgb.Red()   = r;
    prgb.Green() = g;
    prgb.Blue()  = b;
    prgb.Alpha() = a;
    return *this;
}

//set rgb with iColor
PIXELS& PIXELS::setRGBA(const int& rgba)
{
    prgb = rgba;
    return *this;
}

//set rgb with iColor
PIXELS& PIXELS::setRGBA(const iColor& rgba)
{
    prgb = rgba;
    return *this;
}

//set rgb with PIXELS
PIXELS& PIXELS::setRGBA(const PIXELS& ppot)
{
    prgb = ppot.prgb;
    return *this;
}

void PIXELS::setEdge(int bedge)
{
    bEdge = bedge;
}

void PIXELS::initpPos()
{
    pix_P.first = None;
    pix_P.second = 0;
}

void PIXELS::setPostion(Position pos,bool isCorner/* = false*/)
{
    pix_P.first = pos;
    if(isCorner)
        pix_P.second |= 0x80;
    else
        pix_P.second |= 0;
}

bool PIXELS::isCorner()
{
    return ((pix_P.second & 0x80) == 0x80);
}

bool PIXELS::isEdge(PIXELS& pixel, int W,int H)
{
    if(pixel.ppoint.isEdge(W,H))
    {
        pixel.bEdge = 1;
        return true;
    }
    else
    {
        pixel.bEdge = 0;
        return false;
    }
}

bool PIXELS::isEdge(int W,int H)
{
    if(ppoint.isEdge(W,H))
    {
        bEdge = 1;
        return true;
    }
    else
    {
        bEdge = 0;
        return false;
    }
}

bool PIXELS::empty()const
{
    return ppoint.valid();
}

//set (x,y) PIXELS
PIXELS& PIXELS::setXY(const PIXELS& pixel)
{
    ppoint = pixel.ppoint;
    bEdge = pixel.bEdge;
    return *this;
}

//set (x,y) with x y
PIXELS& PIXELS::setXY(int x,int y)
{
    ppoint.X() = x;
    ppoint.Y() = y;
    return *this;
}

PIXELS& PIXELS::resetXY(int x,int y)
{
    ppoint.X() += x;
    ppoint.Y() += y;
    return *this;
}

int PIXELS::getX()const
{
    //return round(ppoint.X());
    return ppoint.X();
}

int PIXELS::getY()const
{
    //return round(ppoint.Y());
    return ppoint.Y();
}

int PIXELS::getZ()const
{
    //return round(ppoint.Z());
    return ppoint.Z();
}

int PIXELS::getEdge()const
{
    return bEdge;
}

iColor PIXELS::getColor()const
{
    return prgb;
}

int    PIXELS::color()const
{
    return prgb.color();
}

PIXELS::pix_p PIXELS::getpPos()const
{
    return pix_P;
}

bool PIXELS::getpPosStatus()const
{
    return pix_P.second & 0x80;
}

int  PIXELS::getpPosValues()const
{
    return pix_P.second & 0x7F;
}

Position PIXELS::getPostion()const
{
    return pix_P.first;
}

U8 PIXELS::getRed()const
{
    return prgb.red();
}

U8 PIXELS::getGreen()const
{
    return prgb.green();
}

U8 PIXELS::getBlue()const
{
    return prgb.blue();
}

U8 PIXELS::getAlpha()const
{
    return prgb.alpha();
}

/**
 *  Utils
 */

pPIXELS   PIXELS::allocator(int cols)
{
    //pPIXELS rt = new PIXELS[cols];
    pPIXELS rt = (pPIXELS)malloc(cols*sizeof(PIXELS));
    if(!rt)
    {
        std::cout << "In " << __FUNCTION__ << "calloc memory is failed\n";
        return NULL;
    }
    return rt;
}

bool      PIXELS::releaser(pPIXELS& data)
{
    if (data)
    {
        free(data);
        data = NULL;
        return true;
    }
    //std::cout << "In releas data,but data is NULL\n";
    return false;
}


ppPIXELS  PIXELS::allocator(int rows,int cols,bool grid,int dx,int dy,float percent)
{
    float lt = (1-percent)/2.0;
    float rd = lt + percent;
    int sy = round(lt * rows),ey = round(rd * rows);
    int sx = round(lt * cols),ex = round(rd * cols);
    //ppPIXELS rt = new pPIXELS[rows];
    ppPIXELS rt = (ppPIXELS)malloc(rows*sizeof(pPIXELS));
    if(!rt)
    {
        std::cout << "In " << __FUNCTION__ << "calloc memory is failed\n";
        return NULL;
    }
    for (int y = 0; y < rows; y++)
    {
        //rt[y] = new PIXELS[cols];
        rt[y] = (pPIXELS)malloc(cols*sizeof(PIXELS));
        if(!rt[y])
        {
            std::cout << "In " << __FUNCTION__ << " : "
                      << y << "calloc memory is failed\n";
            return NULL;
        }
        if(!grid)
        {
            memset(rt[y],0xFF,cols*sizeof(PIXELS));
        }
        else
        {
            if(BETWEEN(y,sy,ey) && !(y%dy))
            {
                memset(rt[y],0xFF,sx*sizeof(PIXELS));
                memset(rt[y]+sx,0x80,(ex+1-sx)*sizeof(PIXELS));
                memset(rt[y]+ex,0xFF,sx*sizeof(PIXELS));
            }
            else
            {
                memset(rt[y],0xFF,cols*sizeof(PIXELS));
                for(int x = 0;x < cols;++x)
                {
                    if(BETWEEN(x,sx,ex) && BETWEEN(y,sy,ey) && !(x%dx))
                        rt[y][x] = 0xFF808080;
                }
            }
        }
    }
    return rt;
}

bool      PIXELS::releaser(ppPIXELS& data,int rows)
{
    if (data && rows > 0)
    {
        for (int i = 0; i < rows; i++)
        {
            // delete []data[i];
            free(data[i]);
        }
        // delete []data;
        free(data);
        data = NULL;
        return true;
    }
    //std::cout << "In releas data,but data is NULL\n";
    return false;
}

bool      PIXELS::copier(ppPIXELS& des,ppPIXELS& src, int rows,int cols)
{
    // des = new pPIXELS[_height];
    des = (ppPIXELS)calloc(rows, sizeof(pPIXELS));
    if(!des)
    {
        printf("In %s calloc memory is failed\n",__FUNCTION__);
        return false;
    }
    for (int y = 0; y < rows; y++)
    {
        // des[y] = new PIXELS[_width];
        des[y] = (pPIXELS)calloc(cols, sizeof(PIXELS));
        if(!des[y])
        {
            printf("In %s[y] calloc memory is failedp\n",__FUNCTION__);
            return false;
        }
        memcpy(des[y], src[y], sizeof(PIXELS) * cols);
    }
    return true;
}

//draw a Rectangular coordinate system will a origin point
bool   PIXELS::drawRecCoordina(ppPIXELS& data,const int& width,const int& height,const iPoint& origin,float percent)
{
    if(origin.X() < 0 || origin.Y() < 0)
        return false;
    float lt = (1-percent)/2.0;
    float rd = lt + percent;
    int sy = round(lt * height),ey = round(rd * height);
    int sx = round(lt * width),ex = round(rd * width);
    int x = origin.X();
    int y = origin.Y();
    memset(data[y]+sx,0,sizeof(PIXELS)*width*percent);
    for(int iy = sy;iy < ey;++iy)
    {
        data[iy][x] = 0x0;
    }
    return true;
}

iPoint PIXELS::toPoint()
{
    return ppoint;//-1:2D
}

//binaryzation image with a threshold
void PIXELS::toBin()
{
    U8 r = prgb.red() * 0.3;
    U8 g = prgb.green() * 0.59;
    U8 b = prgb.blue() * 0.11;
    U8 rgb = (r + g + b)/3;
    if(rgb >= rgb_threshold)
    {
        rgb = 255;
    }
    else
        rgb = 0;
    this->setRGBA(rgb,rgb,rgb);
}

void PIXELS::toBin(PIXELS& ppot)
{
    return ppot.toBin();
}

PIXELS&  PIXELS::toGray()
{
    prgb.gray();
    return *this;
}

PIXELS&  PIXELS::toColor()
{
    prgb.colorful();
    return *this;
}

//fix up the point position,if the point is edge point
void PIXELS::fix_PIXELS(int W,int H)
{
    if(isEdge(*this,W,H))
    {
        if(ppoint.X() < 0)
            ppoint.X() += W;
        if(ppoint.Y() < 0)
            ppoint.Y() += H;
        if(ppoint.X() >= W)
            ppoint.X() -= W;
        if(ppoint.Y()  >= H)
            ppoint.Y() -= H;
    }
}

//mix two color
PIXELS PIXELS::mix(PIXELS& p1,PIXELS& p2,U8 weight)
{
    float scale = weight/100.0;
    return ((p1 * scale) + (p2 * (1-scale)));
}

}//namespace MetaData
}//namespace Imaginer
