/*
 * Just define and provide some common use value or method
 */

#ifndef METADATA
#define METADATA
#include <string>
#include <stdlib.h>
#include <math.h>
#include <deque>

namespace Imaginer {
namespace Commons {

typedef double DaTp;
#define ABS(x) ((x) < 0 ? (-x) : (x))
#define MIN(x,y) (x)>(y)?(y):(x)
#define MAX(x,y) (x)<(y)?(y):(x)

enum colorType
{
    Red,
    Green,
    Blue,
    Pricolor
};

// pixel position
enum Position
{
    Up,
    Down,
    Left,
    Right,
    Front,
    Back,
    None
};
//Get Position's string
inline std::string Pos2str(Position pos)
{
    switch(pos)
    {
        case Up:
            return "Up   ";
            break;
        case Down:
            return "Down ";
            break;
        case Left:
            return "Left ";
            break;
        case Right:
            return "Right";
            break;
        case Front:
            return "Front";
            break;
        case Back:
            return "Back ";
            break;
        default:
            return "None ";
            break;
    }
}
//Get colorType's string
inline std::string color2str(colorType color)
{
    switch(color)
    {
        case Pricolor:
            return "All";
            break;
        case Red:
            return "Red";
            break;
        case Green:
            return "Green";
            break;
        case Blue:
            return "Blue";
            break;
        default:
            return "all";
            break;
    }
}

class IException
{
public:
    IException(){}
    IException(const char* str)
    {
        _msg = str;
    }
    IException(std::string str)
    {
        _msg = str;
    }
    IException(const IException& rhs)
    {
        _msg = rhs._msg;
    }
    std::string _msg;
    std::string _type;
};

/*
#ifndef InvSqrt
static float InvSqrt(float x)
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x; // get bits for floating VALUE
    i = 0x5f375a86- (i>>1); // gives initial guess y0
    x = *(float*)&i; // convert bits BACK to float
    x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
    return x;
}
#endif //INVSQRT

#ifndef round
inline double round( DaTp d )
{
    return floor( d + 0.5 );
}
#endif
*/
//frame of axes: Down and Right is positive direction
enum FofA//frame of axes
{
    RU, //第一象限
    LU, //第二象限
    LD, //第三象限
    RD, //第四象限
    R , //x正半轴
    U , //y正半轴
    L , //x负半轴
    D , //y负半轴
    O , //原点
    NO
};

#define bsIsBigEndian() ((((const int*)"\0\x1\x2\x3\x4\x5\x6\x7")[0] & 255) != 0)

#define PI              3.1415926
#define R2D(R)          R*180/PI
#define D2R(D)          D*PI/180
#define Euler_Number    2.7182818
#define NATURE_E        Euler_Number
#define GOLDEN_SECTION  0.618
#define REPAIR(_x,_X) ((_x < 0) ? (0) : ((_x >= _X) ? (_X-1) : (_x)))
//#define RESET(_T,_FLAG) (((_T) < 0) ? 0 : (((_T) > (_FLAG)) ? (_FLAG) : (_T)))

}//namespace Commons
}//namespace Imaginer

#endif // METADATA

