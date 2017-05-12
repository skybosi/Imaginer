#include "iFonts.h"
using namespace std;
namespace Imaginer
{
namespace iUtils
{

iFonts::cfont::cfont():_size(0),_chdata(NULL){}

iFonts::cfont::cfont(int ch, int size):_curpos(0),_ch(ch),_size(size)
{
    _chdata = (char*)malloc(size * sizeof(char));
}

iFonts::cfont::~cfont()
{
    if(!_chdata){
        free(_chdata);
        _chdata = NULL;
    }
}

void  iFonts::cfont::add(char chdata, int cindex, int sx, int sy) // add next point to the _chdata
{
    if(chdata >= 64){
        printf("out of range !");
        return;
    }
    switch(cindex){
    case 0:  // x change
        _chdata[_curpos++] = XFMT(chdata);
        break;
    case 1:  // y change
        _chdata[_curpos++] = YFMT(chdata);
        break;
    case 2:  // z change: now is no use
        printf("no support now !");
        break;
    case 3:  //
        _chdata[_curpos++] = 0xff;  // boundary separator
        //_chdata[_curpos++] = sx;    // maybe a bug
        //_chdata[_curpos++] = sy;    // maybe a bug
        break;
    default: // invalid index
        printf("invalid !");
        break;
    }
}

void  iFonts::cfont::encode(int ch, const vdPIXLES& fonts)
{
    _ch = ch;
    size_t bsize = fonts.size();
    size_t size = 0;
    /*
     * ox: first boundary's first point's x
     * oy: first boundary's first point's y
     * sx: each boundary's forst's point's x
     * sy: each boundary's forst's point's y
     * px: previous point's x
     * py: previous point's y
     * cx: current point's x
     * cy: current point's x
     *
     */
    int ox = fonts[0][0].getX(), oy = fonts[0][0].getY();
    int sx = ox, sy = oy;
    int px = sx, py = sy;
    int cx = 0, cy = 0;
    int diff = 0;
    for(size_t i = 0; i < bsize; ++i)
    {
        // add each boundary's start point relation (first boundary's first point) position
        sx = fonts[i][0].getX(), sy = fonts[i][0].getY();
        px = sx, py = sy;
        init(sx - ox, sy - oy);
        size = fonts[i].size();
        for(size_t j = 1; j < size; ++j)
        {
            cx = fonts[i][j].getX(), cy = fonts[i][j].getY();
            if(py == cy)
            {
                diff = cx - px;
                add(char(diff), 0);
            }
            else if(px == cx)
            {
                diff = cy - py;
                add(char(diff), 1);
            }else{
                printf("boundary data is error!");
                return;
            }
            px = cx, py = cy;
        }
        // add boundary separator character
        add(0, 3);  // note: 0 is no use,just for call function add
    }
    //delete last add(0, 3)
    _curpos--;
}

void  iFonts::cfont::decode(int& ch, vdPIXLES& fonts, int ox, int oy)
{
    ch = _ch;
    _curpos = 0;
    /*
     * sx: each boundary's forst's point's x
     * sy: each boundary's forst's point's y
     * cx: current point's x
     * cy: current point's x
     *
     */
    int sx = 0, sy =0;
    int cx = ox, cy = oy;
    unsigned char data = 0;
    size_t bsize = fonts.size();
    size_t j = 1;
    for(size_t i = 0; i < bsize; ++i)
    {
        //set each boundary's start point relation (ox, oy) position
        sx = _chdata[_curpos++], sy = _chdata[_curpos++];
        fonts[i][0].setXY(sx + ox, sy + oy);
        cx = sx + ox;
        cy = sy + oy;
        j = 1;
        while((data = _chdata[_curpos++]) != 0xff)
        {
            switch(0xc0 & data)
            {
            case 0x00:  //XR:  x+ right (>) change
                fonts[i][j].setXY(cx+(0x3f & data),cy);
                break;
            case 0x40:	//XL:  x- left  (<) change
                fonts[i][j].setXY(cx-(0x3f & data),cy);
                break;
            case 0x80:  //YD:  y+ down  (^) change
                fonts[i][j].setXY(cx,cy+(0x3f & data));
                break;
            case 0xc0:  //YU:  y- up    (V) change
                fonts[i][j].setXY(cx,cy-(0x3f & data));
                break;
            }
            cx = fonts[i][j].getX();
            cy = fonts[i][j].getY();
            j++;
        }
    }
}

void  iFonts::cfont::init(char ox, char oy)  //first postion save (x, y), default init to (0, 0)
{
    _chdata[_curpos++] = ox;
    _chdata[_curpos++] = oy;
}


iFonts::iFonts(){}
iFonts::iFonts(const char* fpath)
{
    if(NULL != fpath){
        if(!(_ffont = fopen(fpath, "rw"))){
            printf("open file: %s is failed!", fpath);
        }
    }else{
        printf("%s is NULL", fpath);
    }
}
iFonts::~iFonts()
{
    if(!_ffont)
    {
        fclose(_ffont);
        _ffont = NULL;
    }
}

bool  iFonts::loadFonts(const char* fpath)
{
    if(NULL == fpath)
        return false;
    if(NULL == _ffont){
        if(!(_ffont = fopen(fpath, "wb"))){
            printf("open file: %s is failed!", fpath);
        }
    }else{  //load font's file
        cfont cur;
        int ch = 0, size = 0;
        char* chdata = NULL;
        while(!feof(_ffont)){
            fread(&ch, sizeof(int), 1, _ffont);
            cur._ch = ch;
            fread(&size, sizeof(int), 1, _ffont);
            cur._size = size;
            fread(chdata, size, 1, _ffont);
            cur._chdata = chdata;
            _fdata.push_back(cur);
        }
    }
    return true;
}

void  iFonts::writeFont(int ch, const cfont& fonts)
{
    if(NULL == _ffont) return;
    fwrite(&ch, sizeof(int), 1, _ffont);
    fwrite(&fonts._size, sizeof(int), 1, _ffont);
    fwrite(fonts._chdata, fonts._size, 1, _ffont);
}

iFonts::cfont  iFonts::readFont(int ch)
{
    size_t size = _fdata.size();
    for(size_t i = 0; i < size; ++i)
    {
        if( _fdata[i]._ch == ch){
            return  _fdata[i];
        }
    }
    return cfont();
}

}
}//namespce Imaginer::iUtils
