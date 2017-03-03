/*
 *  This is class that save each pixel where at the image,so Processe it will
 *  contrl the pixel.
 *
 */
#ifndef IPOINT_H
#define IPOINT_H
#include <deque>
#include <iostream>
#include "iColor.h"
#include "iCommons.h"
#include "iMatrix.h"
#include "iTransfMatrix.h"

using namespace Imaginer::Commons;
using namespace Imaginer::Utils;

namespace Imaginer {
namespace MetaData {
class iPoint
{
private:
    DaTp _x;
    DaTp _y;
    DaTp _z;
    int  _bcolor;
    bool _valid;
public:
    iPoint();
    iPoint(DaTp x,DaTp y,DaTp z = -1,int color = 0xFFFFFFFF);
    iPoint(const iPoint& rh);
public:
    iPoint operator-(const iPoint& rhs);
    iPoint operator+(const iPoint& rhs);
    iPoint operator*(const iMatrix& rhs);
    iPoint& operator=(const iPoint& rhs);
    bool operator==(const iPoint& rhs);
    bool operator!=(const iPoint& rhs);
    //iPoint& operator+=(const mvect& rhs);
    friend  std::ostream& operator<<(std::ostream &os,const iPoint& color);
public:
    inline DaTp X()const {return _x;}
    inline DaTp Y()const {return _y;}
    inline DaTp Z()const {return _z;}
    inline int  color()const {return _bcolor;}
    inline DaTp& X(){return _x;}
    inline DaTp& Y(){return _y;}
    inline DaTp& Z(){return _z;}
    inline int&  color(){return _bcolor;}
    inline void  format(){(_x != _x)?(_x = 0):(_x),(_y != _y)?(_y = 0):(_y),(_z != _z)?(_z = 0):(_z);}
    inline iMatrix toRmarix(){iMatrix rt(1,4);rt[0][0]= _x;rt[0][1] = _x;rt[0][2] = _z;rt[0][3] = 1;return rt;}
    //inline iMatrix toRmarix(){MeT  point[] = {_x,_y,_z,1};/*齐次坐标向量*/ return iMatrix(1,4,4,point);}
    inline iMatrix toCmarix(){iMatrix rt(4,1);rt[0][0]= _x;rt[1][0] = _x;rt[2][0] = _z;rt[3][0] = 1;return rt;}
    //inline iMatrix toCmarix(){MeT  point[] = {_x,_y,_z,1};/*齐次坐标向量*/ return iMatrix(4,1,4,point);}
    //inline bool empty()const{ return _valid;}
public:
    bool   isEdge(DaTp W,DaTp H);
    inline void   bindColor(const int& color){_bcolor = color;}
    inline void   setValid(bool status = false){ _valid = status;}
    inline bool   valid()const{return _valid;}

};
typedef std::deque<iPoint> dPoint;
typedef std::deque<dPoint> vdPoint;

}//namespace MetaData
}//namespace Imaginer
#endif // IPOINT_H
