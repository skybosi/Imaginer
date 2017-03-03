#include "iPoint.h"

namespace Imaginer {
namespace MetaData {

iPoint::iPoint():_x(-1),_y(-1),_z(-1),_bcolor(0xFFFFFFFF),_valid(true){}

iPoint::iPoint(DaTp x,DaTp y,DaTp z,int color):_x(x),_y(y),_z(z),_bcolor(color),_valid(true){}

iPoint::iPoint(const iPoint& rh)
{
    _x = rh._x;
    _y = rh._y;
    _z = rh._z;
    _bcolor = rh._bcolor;
    _valid = rh._valid;
}

iPoint iPoint::operator-(const iPoint& rhs){
    return iPoint(_x-rhs._x,_y-rhs._y,_z-rhs._z);
}

iPoint iPoint::operator+(const iPoint& rhs){
    return iPoint(_x+rhs._x,_y+rhs._y,_z+rhs._z);
}

iPoint iPoint::operator*(const iMatrix& rhs)
{
    DaTp div_W = 1.0;
    if(rhs[0][3] || rhs[1][3] || rhs[2][3])
    {
        div_W = 1.0/(_x*rhs[0][3] + _y*rhs[1][3] + _z*rhs[2][3] + 1*rhs[3][3]);
    }
    this->format();
    return iPoint(
                (_x*rhs[0][0] + _y*rhs[1][0] + _z*rhs[2][0]+1*rhs[3][0])*div_W,
                (_x*rhs[0][1] + _y*rhs[1][1] + _z*rhs[2][1]+1*rhs[3][1])*div_W,
                (_x*rhs[0][2] + _y*rhs[1][2] + _z*rhs[2][2]+1*rhs[3][2])*div_W
                );
}

iPoint& iPoint::operator=(const iPoint& rhs)
{
    if(this != &rhs)
    {
        _x = rhs._x;
        _y = rhs._y;
        _z = rhs._z;
        _bcolor = rhs._bcolor;
        _valid = rhs._valid;
    }
    return *this;
}

bool iPoint::operator==(const iPoint& rhs)
{
    return _x == rhs._x && _y == rhs._y && _z == rhs._z;
}
bool iPoint::operator!=(const iPoint& rhs)
{
    return _x == rhs._x || _y != rhs._y || _z != rhs._z;
}

std::ostream& operator<<(std::ostream &os,const iPoint& point)
{
    os << "X: " << point._x << "\t"
       << "Y: " << point._y << "\t"
       << "Z: " << point._z << std::endl;
    return os;
}

bool   iPoint::isEdge(DaTp W,DaTp H)
{
    return (_x <= 0 || _y >= W-1 || _y <= 0 || _y >= H-1);
}

}//namespace MetaData
}//namespace Imaginer
