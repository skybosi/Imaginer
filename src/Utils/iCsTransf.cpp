#include "iCsTransf.h"

namespace Imaginer {
namespace Utils {

//part A
iCsTransf iCsTransf::mirror(const iMatrix& base)
{
    return *this*base;
}

iCsTransf iCsTransf::scales(const iMatrix& base)
{
    return *this*base;
}

iCsTransf iCsTransf::shears(const iMatrix& base)
{
    return *this*base;
}

iCsTransf iCsTransf::rotate(const iMatrix& base)
{
    return *this*base;
}
//part B

iCsTransf iCsTransf::move(const iMatrix& base)
{
    return *this*base;
}
/*
    //part C
    iCsTransf iCsTransf::perspective()
    {
        return *this*base;
    }
 */
}//namespace Utils
}//namespace Imaginer
