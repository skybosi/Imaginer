/*
 * This is a utils class for coordinate transformation
 * It's subClass of iMatrix, just use to 3d coordinate transformation
 * Provide some default Transfor Mode,You can chose one of them
 * Also You set a transfor iMatrix(4*4).
 */

#ifndef ITRANSFMATRIX_H
#define ITRANSFMATRIX_H
#include "iMatrix.h"

namespace Imaginer {
namespace Utils {

class iTransfMatrix : public iMatrix
{
public:
    typedef enum TransfMode
    {
        MOVE,
        SCALE,
        ROTATEx,
        ROTATEy,
        ROTATEz,
        MIRRORx,
        MIRRORy,
        MIRRORz,
        MIRRORxy,
        MIRRORyz,
        MIRRORzx,
        PERSPECTIVE,
        SHEARxy,
        SHEARxz,
        SHEARyx,
        SHEARyz,
        SHEARzx,
        SHEARzy
    }TsMd;
public:
    iTransfMatrix();
    iTransfMatrix(const MeT& tx,const MeT& ty,const MeT& tz,TsMd tm = MOVE);
    iTransfMatrix(const MeT& theta,TsMd tm = ROTATEx);
    iTransfMatrix(TsMd tm = MIRRORx);
public:
};

}//namespace Utils
}//namespace Imaginer

#endif // ITRANSFMATRIX_H
