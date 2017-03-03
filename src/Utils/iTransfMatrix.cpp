#include <cmath>
#include "iTransfMatrix.h"

namespace Imaginer {
namespace Utils {

iTransfMatrix::iTransfMatrix():iMatrix(4,4)
{
}
iTransfMatrix::iTransfMatrix(const MeT& tx,const MeT& ty,const MeT& tz,TsMd tm/* = MOVE*/)
    :iMatrix(4,4)
{
    switch (tm) {
    case MOVE:
        _Mvs[3][0] = tx;
        _Mvs[3][1] = ty;
        _Mvs[3][2] = tz;
        _Mvs[0][0] = 1;
        _Mvs[1][1] = 1;
        _Mvs[2][2] = 1;
        _Mvs[3][3] = 1;
        break;
    case SCALE:
        _Mvs[0][0] = tx;
        _Mvs[1][1] = ty;
        _Mvs[2][2] = tz;
        _Mvs[3][3] = 1;
        break;
    default:
        break;
    }

}

iTransfMatrix::iTransfMatrix(const MeT& theta,TsMd tm):iMatrix(4,4)
{
    switch (tm) {
    case ROTATEx:
        _Mvs[0][0] = 1;
        _Mvs[1][1] = cos(theta);
        _Mvs[1][2] = sin(theta);
        _Mvs[2][1] = -sin(theta);
        _Mvs[2][2] = cos(theta);
        _Mvs[3][3] = 1;
        break;
    case ROTATEy:
        _Mvs[0][0] = cos(theta);
        _Mvs[0][2] = -sin(theta);
        _Mvs[1][1] = 1;
        _Mvs[2][0] = sin(theta);
        _Mvs[2][2] = cos(theta);
        _Mvs[3][3] = 1;
        break;
    case ROTATEz:
        _Mvs[0][0] = cos(theta);
        _Mvs[0][1] = sin(theta);
        _Mvs[1][0] = -sin(theta);
        _Mvs[1][1] = cos(theta);
        _Mvs[2][2] = 1;
        _Mvs[3][3] = 1;
        break;
    default:
        break;
    }
}

}//namespace Utils
}//namespace Imaginer
