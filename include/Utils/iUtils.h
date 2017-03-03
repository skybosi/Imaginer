/*
 * I want to provide a utils to deal with all Imaginer's data Process.
 * so It's HERE
 */

#ifndef IUTILS_H
#define IUTILS_H
#include <iostream>
#include "iCommons.h"
#include "iPoint.h"
using namespace Imaginer::MetaData;
using namespace Imaginer::Commons;

namespace Imaginer {
namespace Utils {

class iInit
{
public:
    iInit();
public:
    //init iterator
    template <class Iter,class Value>
    static void initeach(Iter b,Iter e,Value* color)
    {
        while(b != e)
        {
            *b++ = *color++;
        }
    }

    template <class Iter,class Value>
    static void getseach(Iter b,Iter e,Value* color)
    {
        while(b != e)
        {
            *color++ = *b++;
        }
    }

    //init list []
    template <class list,class index,class Value>
    static void initeach(list& l,index b,index e,const Value* color)
    {
        while(b != e)
        {
            l[b++] = *color++;
        }
    }

    template <class list,class index,class Value>
    static void initeach(list& l,index e,Value* color)
    {
        index b = 0;
        while(b != e)
        {
            l[b++] = *color++;
        }
    }

    //get value
    template <class list,class index,class Value>
    static void getseach(list l,index b,index e,Value* color)
    {
        while(b != e)
        {
            *color++ = l[b++];
        }
    }

    template <class list,class index,class Value>
    static void getseach(list l,index e,Value* color)
    {
        index b = 0;
        while(b != e)
        {
            *color++ = l[b++];
        }
    }

    //show
    template <class list,class index>
    static void showeach(list l,index b,index e)
    {
        while(b != e)
        {
            std::cout << l[b++];
        }
    }

    template <class list,class index>
    static void showeach(list l,index e)
    {
        index b = 0;
        while(b != e)
        {
            std::cout << l[b++];
        }
    }
};


}//namespace Utils
}//namespace Imaginer
#endif // IUTILS_H
