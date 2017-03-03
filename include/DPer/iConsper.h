/*
 * iConsPer => Imaginer Constructor Provider
 * This is a class to transform a math function to a Coordinate list
 * Then provide to DPC to get the math funtion image.
 * At first provide 2D math function deal with
 */

#ifndef ICONSPER_H
#define ICONSPER_H
#include <iostream>
#include <cmath>
#include <vector>
#include "iPoint.h"
#include "iPixels.h"
#include "iGenfuner.h"

using namespace Imaginer::MetaData;
namespace Imaginer
{
namespace DPer
{

class iConsPer
{
public:
    //static std::string FUN_LIST[];
    //static bool validity(std::string funName);
private:
    int  _width;        //2D image width
    int  _height;         //2D image height
    int  _depth;        //3D image height
    DaTp _densityX;      //contrl the X coordinates's Unit length
    DaTp _headX,_tailX;  //2D use to contrl the Independent variable
    DaTp _densityY;      //contrl the Y coordinates's Unit length
    DaTp _headY,_tailY;  //3D use to contrl the Independent variable and _headX _tailX
    DaTp _densityZ;      //contrl the Z coordinates's Unit length
    std::vector<std::string> _funlist; //math function name,use to select math function
    vdPoint  _datas;       //save all provide data from math function
public:
    iConsPer(){}
    iConsPer(std::string funname,DaTp headx = 0,DaTp tailx = 10,DaTp densityx = 100);
    iConsPer(std::vector<std::string>& funlist,DaTp headx,DaTp tailx,DaTp densityx);
    ~iConsPer();
public:
    friend  std::ostream& operator<<(std::ostream &os,const iConsPer& datas)
    {
        for (int y = 0; y < 1;++y)
        {
            for (int x = 0; x < datas._densityX; ++x)
            {
                os << datas._datas[y][x];
            }
        }
        return os;
    }
public:
    /**
     * @brief ConsPoints
     *        Use to Construct a math function's point list
     * @return
     */
    bool   Cons2dPoints();
    inline vdPoint&  getDatas(){return _datas;}
    inline void      push(std::string funname){_funlist.push_back(funname);}
private:
    bool   validity(iGenFuner& gf, int index = 0);
};


}//namespace DPer
}//namespace Imaginer
#endif // ICONSPER_H
