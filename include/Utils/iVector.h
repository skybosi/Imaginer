/*
 * This is a tool for the packaging of mathematical space vector class,
 * with the space to move the location of the vector operation
 *
 */

#ifndef IVECTOR_H
#define IVECTOR_H
#include <iostream>
#include "iCommons.h"
#include "iPoint.h"
using namespace Imaginer::MetaData;
using namespace Imaginer::Commons;

namespace Imaginer {
namespace Utils {

class iVector
{
private:
    FofA getPosition()
    {
        if(int(_x) & int(_y))
        {
            if((int(_x) ^int(_y)) >= 0) //同号
            {
                if(_x > 0/*&& _y >0*/)
                    return RD;
                else
                    return LU;
            }
            else//异号
            {
                if(_x  > _y)
                    return RU;
                else
                    return LD;
            }
        }
        else
        {
            if(_x == _y) return O;
            if(_x > 0) return R;
            if(_x < 0) return L;
            if(_y < 0) return U;  //frame of axes: Down and Right is positive direction
            if(_y > 0) return D;

        }
        return NO;
    }
public:
    //bool isLineMove(){return !(_x & _y);}
    iVector():_x(0),_y(0),_z(0),_fofa(NO){}
    iVector(DaTp x,DaTp y,DaTp z = 0):_x(x),_y(y),_z(z){
        _fofa = getPosition();
    }
    iVector(const iPoint& point);
    iVector(const iVector& rh);
    iVector set(DaTp x,DaTp y){_x = x;_y = y;_fofa = getPosition();return *this;}
    iVector set(const iPoint& point);
    iVector reset(DaTp xORy,bool VorH = true){
        iVector ret(*this);
        DaTp meta_x = _x/fabs(_x);
        DaTp meta_y = _y/fabs(_y);
        xORy = abs(xORy);
        if(VorH)//V(y)
        {
            ret._x = meta_x*abs((xORy*_x)/_y);
            ret._y = meta_y*abs(xORy);
        }
        else//H(x)
        {
            ret._y = meta_x*abs((xORy*_y)/_x);
            ret._x = meta_y*abs(xORy);
        }
        return ret;
    }
    iVector operator+(iVector& rhs){
        _x += rhs._x;
        _y += rhs._y;
        return *this;
    }
    DaTp getDistence()
    {
        return sqrt(_x*_x + _y*_y);
    }
    inline DaTp    X()const {return _x;}
    inline DaTp    Y()const {return _y;}
    inline DaTp    Z()const {return _z;}
    inline DaTp&   X() {return _x;}
    inline DaTp&   Y() {return _y;}
    inline DaTp&   Z() {return _z;}
    inline FofA   getFofA()const {return _fofa;}
    iVector operator*(const iMatrix& rhs);
    inline iMatrix toRmarix(){iMatrix rt(1,4);rt[0][0]= _x;rt[0][1] = _x;rt[0][2] = _z;rt[0][3] = 1;return rt;}
    inline iMatrix toCmarix(){iMatrix rt(4,1);rt[0][0]= _x;rt[1][0] = _x;rt[2][0] = _z;rt[3][0] = 1;return rt;}
private:
    DaTp _x;
    DaTp _y;
    DaTp _z;
    FofA _fofa;
};
typedef std::deque<iVector> dmvect;

}//namespace Utils
}//namespace Imaginer

#endif // IVECTOR_H
