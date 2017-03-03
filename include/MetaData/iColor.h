/*
 * This a color system,just control the pixel color
 * It's just a parameter to save pixel's rgba into a int32,and provide some method to get
 * each color ,also provide some set method to reset color,and a swap method to exchage two
 * Color channel's value if need;
 * just use Bit operation,maybe is high and fast efficient,sure All have to be in the real
 * scene test to get the real conclusion
 */
#ifndef ICOLOR_H
#define ICOLOR_H
#include <iostream>
namespace Imaginer{
namespace MetaData
{
class iColor{
public:
    typedef int int32;
    //record ccurrent get part of color state
    enum COLOR
    {
        RE,   //red
        GN,   //green
        BL,   //blue
        AL,   //alpha
        NC    //no color
    };
    enum SWAP
    {
        RB,BR, //r-b | b-r
        RG,GR, //r-g | g-r
        GB,BG, //g-b | b-g
        NN
    };
public:
    static int32 RED;
    static int32 ORANGE;
    static int32 YELLOW;
    static int32 GREEN;
    static int32 BLUE;
    static int32 INDIGO;
    static int32 VIOLET;
    static int32 BLACK;
    static int32 WHITE;
    static int32 GRAY;
    static int32 GOLD;
    static int32 SILVER;
    static int32 NCOLOR;
private:
    int32 _color;
    COLOR _curc;
//    int   _blue;
//    int   _green;
//    int   _red;
//    int   _alpha;
public:
    iColor();
    iColor(const int& rgba);
    iColor(const iColor& rgba);
    iColor(int R,int G,int B,int A = 255);    
public:
    inline  int32 color()const{return _color;}
    inline  int   alpha()const{return ((_color & 0xFF000000) >> 24);}
    inline  int   red()  const{return ((_color & 0x00FF0000) >> 16); }
    inline  int   green()const{return ((_color & 0x0000FF00) >> 8);}
    inline  int   blue() const{return ((_color & 0x000000FF));}
    inline  iColor& setRGBA(int r,int g,int b,int a = 255)
    {
        _color = (a << 24) | (r << 16) | (g << 8) | b;
        return *this;
    }
    inline  iColor& gray()
    {
        int grey = (red() * 38 + green() * 75 + blue() *15) >> 7;
        (((_color &= 0xFF000000) |= (grey << 16)) |= (grey << 8)) |= grey;
        return *this;
    }
    iColor& colorful()
    {
        int gray = (red() + green() + blue())/3;
        if(gray < 64)
        {
            red(0);
            green(gray<<2);
            blue(256);
        }
        else if(gray <= 128)
        {
            red(0);
            green(256);
            blue(512-(gray<<2));
        }
        else if(gray <= 192)
        {
            red((gray<<2)-512);
            green(256);
            blue(0);
        }else
        {
            red(256);
            green(1024-(gray<<2));
            blue(0);
        }
        return *this;
    }
    inline  void  clear(COLOR ctype = NC)
    {
        switch(ctype)
        {
        case RE:
            _color &= 0xFF00FFFF;
            break;
        case GN:
            _color &= 0xFFFF00FF;
            break;
        case BL:
            _color &= 0xFFFFFF00;
            break;
        case AL:
            _color &= 0x00FFFFFF;
            break;
        case NC:
            _color = 0;
            break;
        }
    }

public:
    iColor  operator+(const iColor& rgba);
    iColor  operator+(const int&    rgba);
    iColor  operator-(const iColor& rgba);
    iColor& operator=(const iColor& rgba);
    iColor& operator=(const int& rgba);
    iColor  operator*(const float& scale);
    iColor  operator/(const float& scale);
    iColor  operator~();
    //iColor* operator&(){return this;}
    //iColor&  operator*(){return *this;}
    operator bool()const;
    operator int()const;
    friend  std::ostream& operator<<(std::ostream &os,const iColor& color);
public:
    inline  iColor&  Color() {_curc=NC;return *this;}
    inline  iColor&  Alpha() {_curc=AL;return *this;}
    inline  iColor&  Red()   {_curc=RE;return *this;}
    inline  iColor&  Green() {_curc=GN;return *this;}
    inline  iColor&  Blue()  {_curc=BL;return *this;}
    inline  void     swap(SWAP stype)
    {
        switch(stype)
        {
        case RB:
        case BR:
            _color = ((_color & 0xFF00FF00) | ((_color & 0x00FF0000) >> 16))
                   | ((_color & 0xFF00FF00) | ((_color & 0x000000FF) << 16));
            break;
        case RG:
        case GR:
            _color = ((_color & 0xFF0000FF) | ((_color & 0x00FF0000) >> 8))
                   | ((_color & 0xFF0000FF) | ((_color & 0x0000FF00) << 8));
            break;
        case GB:
        case BG:
            _color = ((_color & 0xFFFF0000) | ((_color & 0x0000FF00) >> 8))
                   | ((_color & 0xFFFF0000) | ((_color & 0x000000FF) << 8));
            break;
        default:
            break;
        }
    }
private:
    inline  void  color(int color){ _color = color; }
    inline  void  alpha(int alpha){(_color &= 0x00FFFFFF)|= (alpha << 24); }
    inline  void  red(int red)    {(_color &= 0xFF00FFFF)|= (red   << 16); }
    inline  void  green(int green){(_color &= 0xFFFF00FF)|= (green << 8); }
    inline  void  blue(int blue)  {(_color &= 0xFFFFFF00)|= (blue); }
    iColor& operator&=(int mask);
    iColor& operator|=(int mask);
    iColor& operator>>=(int mask);
    iColor& operator<<=(int mask);
};

}//namespace MetaData
}//namespace Imaginer

#endif // ICOLOR_H
