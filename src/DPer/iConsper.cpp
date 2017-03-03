#include "iConsper.h"

namespace Imaginer
{
namespace DPer
{

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
    for(size_t i = 0;i < _funlist.size();++i)
    {
        if(validity(gf,i))//判断函数是否合理
        {
            float deataX = (_tailX-_headX)/_densityX;
            for(float x = _headX;x < _tailX;x += deataX)
            {
                afun.push_back(iPoint(x,gf(x),-1,iColor::BLACK));
            }
            _datas.push_back(afun);
            afun.clear();
        }
        else
        {
            std::cout << "Function: " << _funlist[i] << " is invaild" << std::endl;
            continue;
        }
    }
    return true;
}

}//namespace DPer
}//namespace Imaginer
