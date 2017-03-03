/*
 * iCsTransf => (Imaginer Coordinate system transform)
 * This is a class that provide some method trancform the Coordinate system
 * NOW(2016-12-11 10:24): it's support Cartesian coordinates only
 *
 * This is a Matrix that deal with the iCsTransf,It's have four parts;
 * A : Use to  mirror,rotate,scale,shear...
 * B : Use to  move
 * C : Use to  perspective projection
 * D : Use to  all scale
 *
 *    **          |    **       **    A    **  %    ** C **
 *    *   a  b  c | p   *       *  a  b  c  *  %    *  p  *
 *    *   c  e  f | q   *       *  c  e  f  *  %    *  q  *
 *    *   g  h  i | r   *       *  g  h  i  *  %    *  r  *
 *    *  ______________ *  ==>  **         **  %    **   **
 *    *           |     *       ===========================
 *    *   l  m  n | s   *       **    B    **  %    ** D **
 *    **          |    **       *  l  m  n  *  %    *  s  *
 *                              **         **  %    **   **
 *
 */
#ifndef ICSTRANSF_H
#define ICSTRANSF_H

#include <iostream>
#include "iPoint.h"
#include "iMatrix.h"

using namespace Imaginer::MetaData;
namespace Imaginer {
namespace Utils {

class iCsTransf
{
private:
    DaTp _x;
    DaTp _y;
    DaTp _z;
public:
    iCsTransf():_x(0),_y(0),_z(0){}
    iCsTransf(DaTp x,DaTp y,DaTp z):_x(x),_y(y),_z(z){}
    iCsTransf(const iMatrix& vec):_x(vec[0][0]),_y(vec[0][1]),_z(vec[0][2]){}
    iCsTransf(const iCsTransf& rhs):_x(rhs._x),_y(rhs._y),_z(rhs._z){}
public:
    inline iCsTransf  operator+(const iCsTransf& rhs)
    {
        return iCsTransf(_x+rhs._x,_x+rhs._y,_z+rhs._z);
    }
    inline iCsTransf  operator-(const iCsTransf& rhs)
    {
        return iCsTransf(_x-rhs._x,_x-rhs._y,_z-rhs._z);
    }
    inline iCsTransf& operator=(const iCsTransf& rhs)
    {
        if(this != &rhs)
        {
            _x = rhs._x;
            _y = rhs._y;
            _z = rhs._z;
        }
        return *this;
    }
    inline iCsTransf  operator*(const float& scale)
    {
        return iCsTransf(_x*scale,_y*scale,_z*scale);
    }
    inline iCsTransf  operator*(const iMatrix& base)
    {
        return iCsTransf(toMatrix()*base);
    }
    inline iCsTransf  operator/(const float& scale)
    {
        if(scale)
        {
            return iCsTransf(_x/scale,_y/scale,_z/scale);
        }
        return *this;
    }
    inline bool       operator==(const iCsTransf& rhs)
    {
        return ((_x == rhs._x) && (_y == rhs._y) && (_z == rhs._z));
    }
    friend  std::ostream& operator<<(std::ostream &os,const iCsTransf& rhs)
    {
        os << "_x: " << rhs._x << "\t"
           << "_y: " << rhs._y << "\t"
           << "_z: " << rhs._z << "\t" << std::endl;
        return os;
    }

public:
    //part A
    iCsTransf mirror(const iMatrix& base);
    iCsTransf scales(const iMatrix& base);
    iCsTransf shears(const iMatrix& base);
    iCsTransf rotate(const iMatrix& base);
    //part B
    iCsTransf move(const iMatrix& base);
    /*
    //part C
    iCsTransf perspective()
    {
        return *this*base;
    }
    */
private:
    inline iMatrix toMatrix()
    {
        //int  point[] = {_x,_y,_z,1};//齐次坐标向量
        //return iMatrix(1,4,4,point);
        return iPoint(_x,_y,_z).toRmarix();
    }
    inline void getMatrix(iMatrix& data)
    {
        _x = data[0][0];
        _y = data[0][1];
        _z = data[0][2];
    }
};

}//namespace Utils
}//namespace Imaginer
#endif // ICSTRANSF_H
