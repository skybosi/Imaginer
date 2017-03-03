#include "dpcCons.h"

namespace Imaginer
{
namespace DPC
{

///////////////////iConsPer///////////////////////

iConsPer::iConsPer(std::string funname,DaTp headx,DaTp tailx,DaTp densityx)
    :_width(0),_height(0),_depth(0),
      _densityX(densityx),_headX(headx),_tailX(tailx),
      _densityY(0),_headY(0),_tailY(0),_densityZ(0)
{
    _funlist.push_back(funname);
}

iConsPer::iConsPer(std::vector<std::string>& funlist,DaTp headx,DaTp tailx,DaTp densityx)
    :_width(0),_height(0),_depth(0),
      _densityX(densityx),_headX(headx),_tailX(tailx),
      _densityY(0),_headY(0),_tailY(0),_densityZ(0),_funlist(funlist)
{
}

iConsPer::~iConsPer()
{
}

/*
std::string iConsPer::FUN_LIST[] =
{//cmath provide such math funtion:
    "abs",    //绝对值          "acos",   //反余弦         "acosf",   //反余弦
    "acosl",  //反余弦          "asin",   //反正弦         "asinf",   //反正弦
    "asinl",  //反正弦          "atan",   //反正切         "atan2",   //y/x的反正切
    "atan2f", //y/x的反正切     "atan2l", //y/x的反正切     "atanf",   //反正切
    "atanl",  //反正切          "ceil",   //上取整         "ceilf",   //上取整
    "ceill",  //上取整          "cos",    //余弦           "cosf",    //余弦
    "cosh",   //双曲余弦        "coshf",  //双曲余弦        "coshl",   //双曲余弦
    "cosl",   //余弦           "exp",    //指数值          "expf",    //指数值
    "expl",   //指数值         "fabs",   //绝对值          "fabsf",   //绝对值
    "fabsl",  //绝对值         "floor",  //下取整          "floorf",  //下取整
    "floorl", //下取整         "fmod",   //求余            "fmodf",   //求余
    "fmodl",  //求余
    "frexp",  //返回value=x*2n中x的值,n存贮在eptr中
    "frexpf", //返回value=x*2n中x的值,n存贮在eptr中
    "frexpl", //返回value=x*2n中x的值,n存贮在eptr中
    "ldexp",  //返回value*2exp的值
    "ldexpf", //返回value*2exp的值
    "ldexpl", //返回value*2exp的值
    "log",    //对数           "log10",  //对数            "log10f",  //对数
    "log10l", //对数           "logf",   //对数            "logl",    //对数
    "modf",   //将双精度数value分解成尾数和阶
    "modff",  //将双精度数value分解成尾数和阶
    "modfl",  //将双精度数value分解成尾数和阶
    "pow",    //计算幂         "powf",   //计算幂           "powl",    //计算幂
    "sin",    //正弦           "sinf",   //正弦             "sinh",   //双曲正弦
    "sinhf",  //双曲正弦       "sinhl",   //双曲正弦         "sinl",   //正弦
    "sqrt",   //开方          "sqrtf",   //开方             "sqrtl",  //开方
    "tan",    //正切          "tanf",    //正切             "tanh",   //双曲正切
    "tanhf",  //双曲正切       "tanhl",   //双曲正切         "tanl",   //正切
};
*/

/*
//判断函数是否合理
bool iConsPer::validity(std::string funName)
{
    int size = sizeof(FUN_LIST) / sizeof(FUN_LIST[0]);
    for(int i = 0;i < size; ++i)
    {
        //std::cout << FUN_LIST[i] << std::endl;
        if(!funName.compare(FUN_LIST[i]))return true;
    }
    return false;
}
*/

bool iConsPer::validity(iGenFuner& gf, int index)
{
    return gf.genRpn(_funlist[index]);
}

bool iConsPer::Cons2dPoints()
{
    dPoint afun;
    iGenFuner gf;
    float deataX = (_tailX-_headX)/_densityX;
    for(size_t i = 0;i < _funlist.size();++i)
    {
        if(validity(gf,i))//判断函数是否合理
        {
            for(float x = _headX;x < _tailX;x += deataX)
            {
                afun.push_back(iPoint(x,gf(x),-1,iColor::BLACK));
            }
            _datas.push_back(afun);
            afun.clear();
        }
        //std::cout << "Function: " << _funlist[i] << " is invaild" << std::endl;
    }
    return false;
}

bool iConsPer::Cons3dPoints()
{
    dPoint afun;
    iGenFuner gf;
    float deataX = (_tailX-_headX)/_densityX;
    float deataY = (_tailY-_headY)/_densityY;
    for(size_t i = 0;i < _funlist.size();++i)
    {
        if(validity(gf,i))//判断函数是否合理
        {
            for(float y = _headY;y < _tailY;y += deataY)
            {
                for(float x = _headX;x < _tailX;x += deataX)
                {
                    afun.push_back(iPoint(x,y,gf(x,y),iColor::BLACK));
                }
            }
            _datas.push_back(afun);
            afun.clear();
        }
        //std::cout << "Function: " << _funlist[i] << " is invaild" << std::endl;
    }
    return false;
}

///////////////////dpcCons///////////////////////

dpcCons::dpcCons(ppPIXELS& data,int& width,int& height,std::string funname,DaTp startX,DaTp endX,DaTp densityx):
    dataPcer(data,width,height),_ic(funname,startX,endX,densityx)
{
    initConsPer(startX,endX);
}

dpcCons::dpcCons(ppPIXELS& data,int& width,int& height,std::vector<std::string> funlist,DaTp startX,DaTp endX,DaTp densityx):
    dataPcer(data,width,height),_ic(funlist,startX,endX,densityx)
{
    initConsPer(startX,endX);
}

dpcCons::dpcCons(DperMum* dp,std::string funname,DaTp startX,DaTp endX,DaTp densityx):dataPcer(dp),_ic(funname,startX,endX,densityx)
{
    initConsPer(startX,endX);
}

bool   dpcCons::consLine()
{
    if(_ic.getDatas().empty())
        return false;
    dPoint afun;
    size_t lsize = _ic.getDatas().size();
    size_t psize;
    iColor color[7] = { iColor::RED,
                        iColor::ORANGE,
                        iColor::YELLOW,
                        iColor::GREEN,
                        iColor::BLUE,
                        iColor::INDIGO,
                        iColor::VIOLET};
    for(size_t index = 0;index < lsize;++index)
    {
        afun = _ic.getDatas()[index];
        iPoint first;
        iPoint second;
        psize  = afun.size() - 1;
        for(size_t i = 0;i < psize;++i)
        {
            first =  fitter(afun[i].X(),afun[i].Y(),afun[i].color());
            second =  fitter(afun[i+1].X(),afun[i+1].Y(),afun[i+1].color());
            if(first.valid() && second.valid())
                drawLine(first,second,color[index%7]);
        }
    }
    return true;
}

bool   dpcCons::consPoint()
{
    if(_ic.getDatas().empty())
        return false;
    dPoint afun;
    size_t lsize = _ic.getDatas().size();
    size_t psize;
    for(size_t index = 0;index < lsize;++index)
    {
        afun = _ic.getDatas()[index];
        iPoint point;
        psize  = afun.size() - 1;
        for(size_t i = 0;i < psize;++i)
        {
            point =  fitter(afun[i].X(),afun[i].Y(),afun[i].color());
            if(point.valid())
                _Data[int(point.Y())][int(point.X())] = point;
        }
    }
    return true;
}

void   dpcCons::initConsPer(DaTp startX,DaTp endX)
{
    //init fitparms
    float scalex = (_width * 0.8)/(endX-startX);
    float scaley = GOLDEN_SECTION * scalex;
    fitparms.setparms(_width,_height,0,scalex,scaley,0);
    //get start point and alloc image memory
    _origin = iPoint(abs(startX/(endX-startX))*_width,0.5*_height);
    //draw a Rectangular coordinate system
    PIXELS::drawRecCoordina(_Data,_width,_height,_origin);
    _ic.Cons2dPoints();
}

inline iPoint dpcCons::fitter(float x,float y,const int& color)
{
    //contrl the x y can not out of image
#define RESET(_T,_S,_E,_FLAG) (((_T != _T) || (_T) < (_S)) ? (_FLAG = true,_S) : (((_T) > (_E)) ? (_FLAG = true,_E) : (_T)))
    ((x *= fitparms._scaleX) += _origin.X());
    ((y *= fitparms._scaleY) += _origin.Y());
    //if(y != y)std::cout << "NAN" << std::endl;
    bool edge = false;
    x = RESET(x,fitparms._stageWS,fitparms._stageWE,edge);
    y = _height - 1 - RESET(y,fitparms._stageHS,fitparms._stageHE,edge);
    iPoint p(x,y,-1);
    (edge)?(p.setValid(false)):(p.bindColor(color));
    return p;
#undef RESET
}

bool   dpcCons::dealManager(const char* dealType)
{
    if(!_Data)
    {
        printf("cannot deal with,there are not Data!\n");
        return false;
    }
    if(!dealType)
    {
        printf("Not deal with!,will output itself\n");
        return true;
    }
    while(*dealType)
    {
        switch(*dealType)
        {
        case 'p':
            cout << "  -p     consPoint   : get a math function point\n";
            consPoint();
            break;
        case 'l':
            cout << "  -l     consLine    : get a math function curve line\n";
            consLine();
            break;
        default:
            printf("Not deal with!\n");
            break;
        }
        dealType++;
    }
    //set data
    if(_dp)
    {
        _dp->setData(_Data);
        _dp->setWidth(_width);
        _dp->setHeight(_height);
    }
    return true;
}

void     dpcCons::drawLine(int x1, int y1, int x2, int y2, int rgb,int size/* = 1*/) throw(IException)
{
    if(size < 0)
    {
        throw("error paint size\n");
    }
    //(!rgb) ? (tmp = *rgb) : (tmp.setRGBA(0,255,0));
    int x = x1,y = y1;
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int s1 = (x2 > x1) ? 1 : -1;
    int s2 = (y2 > y1) ? 1 : -1;
    bool interchange = false;	// 默认不互换 dx、dy
    if (dy > dx)				// 当斜率大于 1 时，dx、dy 互换
    {
        int temp = dx;
        dx = dy;
        dy = temp;
        interchange = true;
    }
    int p = (dy << 1) - dx;
    for(int i = 0;i < size ;++i)
    {
        if(interchange)
        {
            (dx)?(++x1):(++y1);//参考上一版本的设计思路
        }else
        {
            (dx)?(++y1):(++x1);//参考上一版本的设计思路
        }
        for(int j = 0; j < dx; ++j)
        {
            _Data[y][x].setRGBA(rgb);
            if (p >= 0)
            {
                if (!interchange)		// 当斜率 < 1 时，选取上下象素点
                    y += s2;
                else					// 当斜率 > 1 时，选取左右象素点
                    x += s1;
                p -= dx << 1;
            }
            if (!interchange)
                x += s1;				// 当斜率 < 1 时，选取 x 为步长
            else
                y += s2;				// 当斜率 > 1 时，选取 y 为步长
            p += dy << 1;
        }
        x = x1,y = y1;
    }
}

void   dpcCons::drawLine(const iPoint& start,const iPoint& end,int rgb,int size/* = 1*/) throw(IException)
{
    int sx = start.X();int ex = start.Y();
    int sy = end.X();int ey = end.Y();
    drawLine(sx,ex,sy,ey,rgb,size);
}

}//namespace DPC
}//namespace Imaginer
