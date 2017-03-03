#include "iVector.h"

namespace Imaginer
{
namespace Utils {

iVector::iVector(const iVector& rh)
{
    _x = rh._x;
    _y = rh._y;
    _fofa = rh._fofa;
}
iVector::iVector(const iPoint& point)
{
    _x = point.X();
    _y = point.Y();
    _fofa = getPosition();
}

iVector iVector::set(const iPoint& point)
{
    _x = point.X();
    _y = point.Y();
    _fofa = getPosition();
    return *this;
}

iVector iVector::operator*(const iMatrix& rhs)
{
    DaTp div_W = 1.0;
    if(rhs[0][3] || rhs[1][3] || rhs[2][3])
    {
        div_W = 1.0/(_x*rhs[0][3] + _y*rhs[1][3] + _z*rhs[2][3] + 1*rhs[3][3]);
    }
    return iVector(
                (_x*rhs[0][0] + _y*rhs[1][0] + _z*rhs[2][0]+1*rhs[3][0])*div_W,
                (_x*rhs[0][1] + _y*rhs[1][1] + _z*rhs[2][1]+1*rhs[3][1])*div_W,
                (_x*rhs[0][2] + _y*rhs[1][2] + _z*rhs[2][2]+1*rhs[3][2])*div_W
                );
}

}//namespace Utils
}//namespace Imaginer
