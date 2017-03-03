/*
 * This is a class use  RPN(Reverse Polish notation) to parser a function expression,
 * and than use to generate a opearator stream.
 * After use the op-stream to get each number's function value;
 *
 */
#ifndef IRPN_H
#define IRPN_H
#include <iostream>
#include <string>
#include <stdlib.h>
#include "iStack.h"

namespace Imaginer{
namespace Utils {

//Reverse Polish notation
class iRpn
{
#define FUNNUM  20
#define FUN_INDEX_RANGE (-128+FUNNUM)
public:
    //save a RPN expression data node
    struct RPNnode
    {
    private:
        union
        {
            double  _value;
            char    _sign;  //will be save operator or system cmath function's index
        }_node;
        bool  _is;          // (isnumber) ? (true) : (false)
    public:
        RPNnode():_is(false){_node._value = 0;}
        RPNnode(double value):_is(true){ _node._value = value;_node._sign = 0;}
        RPNnode(char   value):_is(false){ _node._sign  = value;}
        inline operator char()  { return _node._sign;}
        inline operator double(){ return _node._value;}
        inline operator bool()  { return _is;}
        inline char sign(){return _node._sign;}
        inline double value(){return _node._value;}
        inline double   operator +(const RPNnode& rhs){ return (_node._value + rhs._node._value);}
        inline double   operator -(const RPNnode& rhs){ return (_node._value - rhs._node._value);}
        inline double   operator -(){ _node._value = - _node._value; return *this;}
        inline double   operator *(const RPNnode& rhs){ return (_node._value * rhs._node._value);}
        inline double   operator /(const RPNnode& rhs){ return (_node._value / rhs._node._value);}
        inline double   operator ^(const RPNnode& rhs){ return (pow(_node._value,rhs._node._value));}
        inline RPNnode& operator=(const RPNnode& rhs)
        {
            if(this != &rhs)
            {
                _node = rhs._node;
                _is   = rhs._is;
            }
            return *this;
        }
        inline bool operator==(char value) { return ( (_is) ? (false): ((value == _node._sign) ? true : false) ); }
        inline bool operator!=(char value) { return ( (_is) ? (true): ((value != _node._sign) ? true : false) ); }
        friend  std::ostream& operator<<(std::ostream &os,const RPNnode& node)
        {
            (node._is)? ( os << node._node._value ) : (os <<  node._node._sign );
            return os;
        }
    };
private:
    char*               _expnames;  //save expression
    int                 _curpos;
    iStack< char >*     _operator;  //save operator
    iStack<RPNnode>*    _operands;  //save operands
private:
    static bool         _btPri[][8]; //big pri
    static bool         _bePri[][8]; //big-equl pri
    static std::string  _sysfunS[];
private:
    int  PRI(char sign);
    int  getPRI(char cursign);   //get a operator's PRI from operator PRI table
    bool cmpPRI(char cursign);   //compare current operator sign with top operator,bigger-equal return true,else false
    void upPRI(char& cursign,double& value); //update the operator
    inline char next(){_curpos++;return *_expnames++;}
    inline char prev(){_curpos--;return *_expnames--;}
    inline bool operater(char c);
    inline bool digit(char c);
    void cut();
public:
    iRpn();
    iRpn(char* expname);
    ~iRpn();
    inline void setExp(std::string& expnames){_expnames = (char*)expnames.c_str();}
    bool genRpn();               //generate a expression from the infix expression to suffix expression(RPN)
    void Parser();               //Parser a infix expression to a operator stream
    inline iStack< char >*  getOperator(){ return _operator;}
    inline iStack<RPNnode>* getOperands(){ return _operands;}
    inline bool isVariable(const char& sign){return (sign == 'x' || sign == 'y' || sign == 'X' || sign == 'Y');}
    inline bool isFun(const char& sign){return (sign <= FUN_INDEX_RANGE);}//_sysFun's number - 1 - 128
    char   getFun(std::string funname);
    void   clear(){
        _expnames = NULL;
        _curpos = 0;
        _operator->clear();
        _operands->clear();
    }
};

}//namespace Utils
}//namespace Imaginer

#endif // IRPN_H
