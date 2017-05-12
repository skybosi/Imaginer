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
using namespace std;

namespace Imaginer
{
namespace iUtils
{
class iFonts
{
public:
    class cfont
    {
#define DEBUG
#ifndef  DEBUG
#define XR(_X)  (0x00 | (_X))
#define XL(_X)  (0x40 | (_X))
#define XFMT(_v) ((_v > 0) ? (XR(_v)) : (XL(_v)))
#define YD(_Y)  (0x80 | (_Y))
#define YU(_Y)  (0xc0 | (_Y))
#define YFMT(_v) ((_v > 0) ? (YD(_v)) : (YU(_v)))
#else
    public:
        inline char XFMT(char x)
        {
            char rt = (x > 0) ? (0x00 | (x)) : (0x40 | (x));
            return rt;
        }
        inline char YFMT(char y)
        {
            char rt = (y > 0) ? (0x80 | (y)) : (0xc0 | (y));
            return rt;
        }
#endif
    public:
        cfont();
        cfont(int ch, int size);
        ~cfont();
    public:

#ifndef dPIXLES
        struct PIXELS
        {
            PIXELS(int x = 0,int y = 0):_x(x),_y(y){}
        private:
            int _x;
            int _y;
        public:
            inline int getX()const{return _x;}
            inline int getY()const{return _y;}
            void setXY(int x,int y){ _x = x; _y = y;}
        };
        typedef vector<PIXELS> dPIXLES;
        typedef vector<dPIXLES> vdPIXLES;
#endif
        void  encode(int ch, const vdPIXLES& fonts);
        void  decode(int& ch, vdPIXLES& fonts, int ox = 0, int oy = 0);
        friend std::ostream & operator<<(std::ostream &out, const cfont &c)
        {
			out << "cfont data size: " << c._size " ";
			for(int i = 0; i < c._size; ++i)
			{
				out << hex << "0x" << (unsigned int)(unsigned char)(c._chdata[i]) << " ";
			}
            return out;
        }
    private:
        int    _curpos;	  // current add position's number
        void  init(char ox = 0, char oy = 0);  //first postion save (x, y), default init to (0, 0)
        void  add(char chdata, int cindex = 0, int sx = 0, int sy = 0); // add next point to the _chdata;
    public:
        int    _ch;       // save character
        int    _size;     // the size of ch boundary data (NOTE: include separate character -128)
        char*  _chdata;   // multi-boundary 's relative position data, each boundary is separate with -128
    };
    typedef vector<cfont> vfont;
public:
    iFonts();
    iFonts(const char* fpath);
    ~iFonts();
public:
    /**
     * @brief loadFonts : read data from fonts's file and encode the fonts data to cfont structure data into memory
     * @param fpath
     * @return
     */
    bool  loadFonts(const char* fpath);
    /**
     * @brief writeFont : write cfont structure data into a font's file
     * @param ch
     * @param fonts
     */
    void  writeFont(int ch, const cfont& fonts);
    //void  writeFont(int ch, const DPIXLES& fonts);
    cfont readFont(int ch);
private:
    FILE*  _ffont;       // fonts's file
    vfont  _fdata;       // save all character
};

}
}//namespce Imaginer::iUtils
