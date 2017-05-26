#include <stdlib.h>
#include "iColor.h"

namespace Imaginer{
namespace MetaData
{

iColor::iColor(const int& rgba):
    _color(rgba),_curc(NC)
    /*
    _blue((rgba & 0x000000FF)),
    _green((rgba & 0x0000FF00) >> 8),
    _red((rgba & 0x00FF0000) >> 16),
    _alpha((rgba & 0xFF000000) >> 24)
    */
{}

iColor::iColor():_color(0xFF000000),_curc(NC)/*,_blue(0),_green(0),_red(0),_alpha(255)*/{}

iColor::iColor(const iColor& rgba):_color(rgba._color),_curc(NC){}

iColor::iColor(int R,int G,int B,int A):
    _color((A << 24) | (R << 16) | (G << 8) | B),_curc(NC)
    /*,_blue(B),_green(G),_red(R),_alpha(A)*/{}

iColor::int32 iColor::RED    = 0xFFFF0000;
iColor::int32 iColor::ORANGE = 0xFFFF8000;
iColor::int32 iColor::YELLOW = 0xFFFFFF00;
iColor::int32 iColor::GREEN  = 0xFF00FF00;
iColor::int32 iColor::BLUE   = 0xFF0000FF;
iColor::int32 iColor::INDIGO = 0xFF00FFFF;
iColor::int32 iColor::VIOLET = 0xFF8000FF;
iColor::int32 iColor::BLACK  = 0xFF000000;
iColor::int32 iColor::WHITE  = 0xFFFFFFFF;
iColor::int32 iColor::GRAY   = 0xFF808080;
iColor::int32 iColor::SILVER = 0xFFC0C0C0;
iColor::int32 iColor::GOLD   = 0xFFFFD700;
iColor::int32 iColor::NCOLOR = 0;

iColor  iColor::operator+(const iColor& rgba){
    /*
    return iColor(_red + rgba._red,
                   _green + rgba._green,
                   _blue + rgba._blue,
                   _alpha + rgba._alpha);
    */
    return iColor((this->red()  + rgba.red())  & 0x00FF0000,
                  (this->green()+ rgba.green())& 0x0000FF00,
                  (this->blue() + rgba.blue()) & 0x000000FF,
                  (this->alpha()+ rgba.alpha())& 0xFF000000);
}

iColor  iColor::operator+(const int&    rgba)
{
    return iColor((this->red()  + rgba) & 0x00FF0000,
                  (this->green()+ rgba) & 0x0000FF00,
                  (this->blue() + rgba) & 0x000000FF,
                  (this->alpha()+ rgba) & 0xFF000000);
}

iColor  iColor::operator-(const iColor& rgba){
    /*
    return iColor(_red - rgba._red,
                   _green - rgba._green,
                   _blue - rgba._blue,
                   _alpha - rgba._alpha);
    */
    return iColor(abs(this->red()  - rgba.red()),
                  abs(this->green()- rgba.green()),
                  abs(this->blue() - rgba.blue()),
                  abs(this->alpha()- rgba.alpha()));
}

iColor& iColor::operator=(const iColor& rgba){
    if(this != &rgba)
    {
        _color = rgba._color;
        _curc  = rgba._curc;
    }
    return *this;
}

//TODO: not thread safe
iColor& iColor::operator=(const int& rgba)
{
    switch (_curc) {
    case AL:
        alpha(rgba);
        break;
    case RE:
        red(rgba);
        break;
    case GN:
        green(rgba);
        break;
    case BL:
        blue(rgba);
        break;
    case NC:
    default:
        color(rgba);
        break;
    }
    _curc = NC;
    return *this;
}

iColor  iColor::operator*(const float& scale)
{
    /*
    return iColor(_red*scale,
                   _green*scale,
                   _blue *scale,
                   _alpha*scale);
    */
    //return iColor(_color*scale);
    return iColor((int)(this->red()   * scale) & 0x00FF0000,
                  (int)(this->green() * scale) & 0x0000FF00,
                  (int)(this->blue()  * scale) & 0x000000FF,
                  (int)(this->alpha() * scale) & 0xFF000000);
}

iColor  iColor::operator/(const float& scale)
{
    /*
    return iColor(_red/scale,
                   _green/scale,
                   _blue/scale,
                   _alpha/scale);
    */
    //return iColor(_color/scale);
    return iColor((int)(this->red()   / scale) & 0x00FF0000,
                  (int)(this->green() / scale) & 0x0000FF00,
                  (int)(this->blue()  / scale) & 0x000000FF,
                  (int)(this->alpha() / scale) & 0xFF000000);
}

iColor iColor::operator~()
{
    return iColor(iColor(iColor::WHITE)-*this);
}

iColor::operator bool()const
{
    return _color;
}

iColor::operator int()const
{
    return _color;
}

iColor& iColor::operator&=(int mask)
{
    _color &= mask;
    return *this;
}

iColor& iColor::operator|=(int mask)
{
    _color |= mask;
    return *this;
}

iColor& iColor::operator>>=(int mask)
{
    _color >>= mask;
    return *this;
}

iColor& iColor::operator<<=(int mask)
{
    _color <<= mask;
    return *this;
}

std::ostream & operator<<(std::ostream &os,const iColor& color)
{
    os << std::hex
       << "color: " << color._color  << "\t"
       //<< "color: " << color.color()  << "\t"
       << "red  : " << color.red()   << "\t"
       << "green: " << color.green() << "\t"
       << "blue : " << color.blue()  << "\t"
       << "alpha: " << color.alpha() << std::endl;
    return os;
}

}//namespace MetaData
}//namespace Imginner
