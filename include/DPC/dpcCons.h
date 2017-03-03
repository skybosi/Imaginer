#ifndef DPCCONS_H
#define DPCCONS_H
#include <cmath>
#include "dataPcer.h"
#include "iGenfuner.h"

namespace Imaginer
{
namespace DPC
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
     *        Use to Construct a math function's plane point list
     * @return
     */
    bool   Cons2dPoints();
    /**
     * @brief Cons3dPoints
     *        Use to Construct a math function's space point list
     * @return
     */
    bool   Cons3dPoints();
    inline vdPoint&  getDatas(){return _datas;}
    inline void      push(std::string funname){_funlist.push_back(funname);}
private:
    bool   validity(iGenFuner& gf, int index = 0);
};

class dpcCons : public dataPcer
{
private:
    struct FitParms
    {
        int    _stageWS;   //draw on the stage's start point x
        int    _stageW;    //2D width
        int    _stageWE;   //draw on the stage's end point x
        int    _stageHS;   //draw on the stage's start point y
        int    _stageH;    //2D height
        int    _stageHE;   //draw on the stage's end point y
        int    _stageDS;   //draw on the stage's start point z
        int    _stageD;    //3D height
        int    _stageDE;   //draw on the stage's end point z
        float  _scaleX;    //fit the stage,need scale x
        float  _scaleY;    //fit the stage,need scale y
        float  _scaleZ;    //fit the stage,need scale z
        FitParms():_stageWS(0),_stageW(0),_stageWE(0),
            _stageHS(0),_stageH(0),_stageHE(0),
            _stageDS(0),_stageD(0),_stageDE(0),
            _scaleX(0),_scaleY(0),_scaleZ(0){}
        FitParms(int width,int height,int depth,float scaleX,float scaleY,float scaleZ):
            _stageWS(0.1*width),_stageW(width),_stageWE(0.9*width),
            _stageHS(0.1*height) ,_stageH(height) ,_stageHE(0.9*height),
            _stageDS(0.1*depth),_stageD(depth),_stageDE(0.9*depth),
            _scaleX(scaleX),_scaleY(scaleY),_scaleZ(scaleZ){}
        void setparms(int width,int height,int depth,float scaleX,float scaleY,float scaleZ)
        {
            _stageWS = 0.1*width; _stageW  = width; _stageWE = 0.9*width;
            _stageHS = 0.1*height;  _stageH  = height;  _stageHE = 0.9*height;
            _stageDS = 0.1*depth; _stageD  = depth; _stageDE = 0.9*depth;
            _scaleX = scaleX;      _scaleY = scaleY;  _scaleZ = scaleZ;
        }
    }fitparms;
    iPoint _origin;
    iConsPer _ic;
    std::vector<std::string> _funlist; //math function name,use to select math function
public:
    dpcCons(ppPIXELS& data,int& width,int& height,std::string funname,DaTp startX,DaTp endX,DaTp densityx = 100);
    dpcCons(ppPIXELS& data,int& width,int& height,std::vector<std::string> funlist,DaTp startX,DaTp endX,DaTp densityx = 100);
    dpcCons(DperMum* dp,std::string funname,DaTp startX,DaTp endX,DaTp densityx = 100);
    ~dpcCons(){}
    virtual bool dealManager(const char* dealType);
private:
    bool       consPoint();
    bool       consLine ();
private:
    void initConsPer(DaTp startX,DaTp endX);
    /**
     * @brief fitter
     *        self-fit the image-size,contrl the math-function all point at range of image's size(eg: 80%)
     * @param x  independent variable
     * @param y  math-function's calculated's function value
     * @return   self-fit Transformed value will be save a point,then to be returned
     */
    iPoint fitter(float x,float y,const int& color = iColor::RED);
    /**
     * @brief drawLine
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @param rgb
     * @param size
     */
    void   drawLine(int x1, int y1, int x2, int y2, int rgb,int size = 1) throw(IException);
    void   drawLine(const iPoint& start,const iPoint& end,int rgb,int size = 1) throw(IException);
};

}//namespace DPC
}//namespace Imaginer

#endif // DPCCONS_H
