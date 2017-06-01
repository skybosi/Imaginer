/*
 * For each of the two adjacent boundary points, there are 4 relationships between them:
 * 1. From left to right (XR)
 * 2. From Right to left (XL)
 * 3. From up   to  down (YD)
 * 4. From down to    up (YU)
 *
 * NOTE: coordinate system (R & D is +, L & U is -)
 *          | -YU
 *          |
 * -XL      |       +XR
 * ---------O--------->
 *          |
 *          |
 *          | +YD
 *          v
 *
 * Because the distance between the two adjacent points is generally of the order of 1 pixels,
 * in order to consider other possibilities in the future, it is now possible to increase the
 * range of 1 ~ 64 pixels. To this end, the 'char' type is divided into 6 intervals:
 *
 * -. 0000 0000 :                             0x00
 * -. [0] [0] 00 0001 ~ [0] [0] 11 1111   XR: 0x00
 * -. [0] [1] 00 0000 ~ [0] [1] 11 1111   XL: 0x40
 * -. [1] [0] 00 0000 ~ [1] [0] 11 1111   YD: 0x80
 * -. [1] [1] 00 0000 ~ [1] [1] 11 1110   YU: 0xc0
 * -. 1111 1111 :                             0xff  multi-boundary's Separation mark
 *
 * NOTE: each boundary data will be Separated with 0xff(not include first boundary), and then
 * 2 byte is (x, y) relative position, the position is relative to the first boundary start point
 * And generally, the start point is set (0, 0), so I just think the relative distance is little
 * then 255, so use a byte to save the position data, maybe a bug;
 *
 * first  bit is define x(0) or y(1)
 * second bit is define R(0) or L(1) and D(0) or D(U)
 * the rest of bits is define the distance between the current point and the previous point
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <limits.h>

using namespace std;

namespace Imaginer
{
namespace Utils
{

#ifndef _IPIXELS_H_
    struct PIXELS
    {
        PIXELS(int x = 0,int y = 0):_x(x),_y(y){}
        ~PIXELS(){}
    private:
        int _x;
        int _y;
    public:
        inline int getX()const{return _x;}
        inline int getY()const{return _y;}
        void setXY(int x,int y){ _x = x; _y = y;}
    };
    typedef vector<PIXELS> dPIXELS;
    typedef vector<dPIXELS> vdPIXELS;
#endif

class cfont
{
#define DEBUG
#ifndef  DEBUG
#define XR(_X)  (0x00 | (_X))
#define XL(_X)  (0x40 | (_X))
#define XFMT(_v) ((_v > 0) ? (XR(_v)) : (XL(-_v)))
#define YD(_Y)  (0x80 | (_Y))
#define YU(_Y)  (0xc0 | (_Y))
#define YFMT(_v) ((_v > 0) ? (YD(_v)) : (YU(-_v)))
#else
public:
    inline char XFMT(int x)
    {
        char rt = (x > 0) ? (0x00 | (x)) : (0x40 | (-x));
        return rt;
    }
    inline char YFMT(int y)
    {
        char rt = (y > 0) ? (0x80 | (y)) : (0xc0 | (-y));
        return rt;
    }
    inline void XRANGE(int x)
    {
        _frames[2] = (x < _frames[2]) ? x : _frames[2];
        _frames[3] = (x > _frames[3]) ? x : _frames[3];
    }
    inline void YRANGE(int y)
    {
        _frames[0] = (y < _frames[0]) ? y : _frames[0];
        _frames[1] = (y > _frames[1]) ? y : _frames[1];
    }
#endif
public:
    cfont();
    cfont(int ch, int size, int nums);
    cfont(const cfont& cf); //deep copy, it's very important
    ~cfont();
public:
    /**
     * @brief encode : encode the boundarys's to the cfont, first boundary start point will be set (0, 0), and the other point will be change to relative to it.
     * @param ch     : character  (unicode)
     * @param fonts  : boundarys data
     * @return
     */
    void  encode(int ch, const vdPIXELS& fonts);
    /**
     * @brief decode : decode the cfont's data to the vdPIXELS data
     * @param ch     : character  (unicode)
     * @param fonts  : return boundarys data
     * @param ox     : as the new postion's start point's x, other point all relative to it.
     * @param oy     : as the new postion's start point's y, other point all relative to it.
     * @return
     */
    void  decode(int& ch, vdPIXELS& fonts, int ox = 0, int oy = 0);
    friend std::ostream & operator<<(std::ostream &out, const cfont &c)
    { 
        int size = c._size + 1 - 2 * c._bnums;
        out << "cfont data size: " << size << "\n"
            << "cfont frame: "
            << c._frames[0] << " " << c._frames[1] << " "
            << c._frames[2] << " " << c._frames[3] << "\n";
        for(int i = 0; i < size; ++i)
        {
            out << hex << "0x" << (unsigned int)(unsigned char)(c._chdata[i]) << " ";
        }
        return out << dec << std::endl;
    }
    inline bool  empty() {return _size == 0;}
    inline bool  size()  {return _size;}
    inline int   width() {return (_frames[3] - _frames[2]);}
    inline int   height() {return (_frames[1] - _frames[0]);}
private:
    int    _curpos;	  // current add position's number
    int    _bnums;    // boundary's number
    void  init(char ox = 0, char oy = 0);  //first postion save (x, y), default init to (0, 0)
    void  add(int chdata, int cindex = 0); // add next point to the _chdata;
public:
    int    _ch;       // save character (unicode)
    int    _size;     // the size of ch boundary data (NOTE: include separate character -128)
    char*  _chdata;   // multi-boundary 's relative position data, each boundary is separate with -128
    int    _frames[4];// save the data's top/bottom - left/right position
};

typedef vector<cfont> vfont;
class iFonts
{
public:
    iFonts();
    iFonts(const char* fpath, const char* mode = "wb+");
    ~iFonts();
public:
    /**
     * @brief loader : read data from fonts's file and decode the fonts data to cfont structure data into memory
     * @param fpath
     * @return
     */
    bool  loader(const char* fpath, const char* mode = "rb");
    /**
     * @brief encoder : write cfont structure data into a font's file
     * @param ch
     * @param fonts
     */
    void  encoder(int ch, const cfont& fonts);
    void  encoder(char* chs, const cfont& fonts);
    /**
     * @brief encoder
     * @param ch
     * @param fonts
     */
    void  encoder(int ch, const vdPIXELS& fonts);
    void  encoder(char* chs, const vdPIXELS& fonts);
    /**
     * @brief decoder : read _fdata and get a character's boundary's data
     * @param ch
     * @return        : a character's boundary's data
     */
    cfont decoder(int ch);

private:
    FILE*  _ffont;       // fonts's file
    vfont  _fdata;       // save all characters (unicode)
};

}
}//namespce Imaginer::Utils
