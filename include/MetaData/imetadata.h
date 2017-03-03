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
namespace MeteData {

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

class ImgException
{
public:
    ImgException(){}
    ImgException(const char* str)
    {
        msg = str;
    }
    ImgException(std::string str)
    {
        msg = str;
    }
    ImgException(const ImgException& rhs)
    {
        this->msg = rhs.msg;
    }
    std::string msg;
};
/*
#ifndef INVSQRT
#define INVSQRT

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

struct Point
{
    int _x;
    int _y;
    int _z;
    Point():_x(-1),_y(-1),_z(-1){}
    Point(int x,int y,int z = -1):_x(x),_y(y),_z(z){}
    Point(const Point& rh)
    {
        _x = rh._x;
        _y = rh._y;
        _z = rh._z;
    }

    Point operator-(const Point& rhs){
        return Point(_x-rhs._x,_y-rhs._y,_z-rhs._z);
    }
};
typedef std::deque<Point> dPoint;

struct mvect //Move Vector
{
private:
    FofA getPosition()
    {
        if(_x & _y)
        {
            if((_x ^_y) >= 0) //同号
            {
                if(_x > 0/*&& _y >0*/)
                    return RD;
                else
                    return LU;
            }
            else//异号
            {
                if(_x  > _y)
                    return RU;
                else
                    return LD;
            }
        }
        else
        {
            if(_x == _y) return O;
            if(_x > 0) return R;
            if(_x < 0) return L;
            if(_y < 0) return U;  //frame of axes: Down and Right is positive direction
            if(_y > 0) return D;

        }
        return NO;
    }
public:
    bool isLineMove(){return !(_x & _y);}
    mvect():_x(0),_y(0){
        _fofa = NO;
    }
    mvect(int x,int y):_x(x),_y(y){
        _fofa = getPosition();
    }
    mvect(Point& point){_x = point._x;_y = point._y;_fofa = getPosition();}
    mvect(const mvect& rh)
    {
        _x = rh._x;
        _y = rh._y;
        _fofa = rh._fofa;
    }
    mvect set(int x,int y){_x = x;_y = y;_fofa = getPosition();return *this;}
    mvect set(const Point& point){_x = point._x;_y = point._y;_fofa = getPosition();return *this;}
    mvect reset(int xORy,bool VorH = true){
        mvect ret(*this);
        int meta_x = _x/abs(_x);
        int meta_y = _y/abs(_y);
        xORy = abs(xORy);
        if(VorH)//V(y)
        {
            ret._x = meta_x*abs((xORy*_x)/_y);
            ret._y = meta_y*abs(xORy);
        }
        else//H(x)
        {
            ret._y = meta_x*abs((xORy*_y)/_x);
            ret._x = meta_y*abs(xORy);
        }
        return ret;
    }
    mvect operator+(mvect& rhs){
        _x += rhs._x;
        _y += rhs._y;
        return *this;
    }
    float getDistence()
    {
        return sqrt(_x*_x + _y*_y);
    }
public:
    int _x;
    int _y;
    FofA _fofa;
};
typedef std::deque<mvect> dmvect;

}//namespace MetaData
}//namespace Imaginer

#endif // METADATA

