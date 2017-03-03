/*
 * Imaginer-Generate-function-er =>(iGenfuner) : Function generator and parser
 *
 * This class is used to input the string function of the identification and analysis tools class,
 * Which uses the class iStack and iRpn parser function, in order to obtain the expression corresponding to
 * the operation stream;
 * And when you want to get the math function a ( X -> Y )
 * will use operation stream to change X to Y
 * At last to get Y
 *
 */
#ifndef IGENFUNER_H
#define IGENFUNER_H

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include "iRpn.h"
#include "iStack.h"

namespace Imaginer{
namespace Utils {

class iGenFuner
{
#define FUNNUM  20
#define FUN_INDEX_RANGE (-128+FUNNUM)
public:
    friend class iRpn;
    typedef double (*Fun) (double &, double, double);
    typedef double (*sysFun) (double x);
    class _funer
    {
    private:
        Fun    _fun;
        double _value;
    public:
        _funer():_fun(NULL),_value(0){}
        _funer(const Fun& f,double v = 0):_fun(f),_value(v){}
    public:
        static inline double _assign(double &y, double x,double value)
        {
            return (y += (x+value));
        }
        static inline double _add(double &y, double x,double value)
        {
            return (y += (x+value));
        }
        static inline double _sub(double &y, double x,double value)
        {
            return (y -= (x-value));
        }
        static inline double _mut(double &y, double x,double value)
        {
            return (y *= (x*value));
        }
        static inline double _div(double &y, double x,double value)
        {
            return (y /= (x/value));
        }
        static inline double _pow(double &y, double x,double value)
        {
            return (y += pow(x,value));
        }
        inline double value()const{return _value;}//get value
        inline double& value(){return _value;}    //set value
        inline operator Fun(){return _fun;} //very important,default type conversion(_funer->Fun)
    };
private:
    double _x;
    double _y;
    double _z;
    int    _curpos;
    //std::string          _expname;
    char*                  _expname;
    iRpn                   _rpn;
    iStack<_funer>         _opstream;
    iStack<iRpn::RPNnode>* _operands;
private:
    static sysFun          _sysfun[];
    static std::string     _sysfunS[];
private:
    static sysFun user1;
    static sysFun user2;
    inline static double self(double x){return x;}
    inline static double zero(double x){return (x = 0.0);}
    inline static double one(double  x){return (x = 1.0);}
    static double factorial(double x)
    {
        double result = 1.0;
        for (long i = 1; i <= long(x); ++i)
        {
            result = result * i;
        }
        return result;
    }
public:
    iGenFuner();
    iGenFuner(char *expname);
private:
    inline char next(){return /*_expname[_curpos++]*/ *_expname++;}
    inline char prev(){return /*_expname[_curpos--]*/ *_expname--;}
    inline void push(const Fun& fun){_opstream.push(fun);}
    inline void setExp(std::string expname){_expname = (char*)expname.c_str();}
    void   gen();
    bool   generater();
    double generater(double x,double y = 0);
    inline Fun  pop(){return _opstream.pop();}
    double Parser(iStack<iRpn::RPNnode>& result,int& curpos,double x,double y = 0);
public:
    bool   genRpn(std::string& expname);
    void   bind1(sysFun you){ _sysfun[18] = user1 = you;}
    void   bind2(sysFun you){ _sysfun[19] = user2 = you;}
    sysFun& User1(){return _sysfun[18];}
    sysFun& User2(){return _sysfun[19];}
    double operator() (double x);
    double operator() (double x,double y);
    void   clear(){_rpn.clear();_operands->clear();}
};
}//namespace Utils
}//namespace Imaginer

#endif // IGENFUNER_H
