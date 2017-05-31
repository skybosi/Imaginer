#include <string.h>  //memcpy
#include "iFonts.h"
using namespace std;
namespace Imaginer
{
namespace Utils
{

cfont::cfont():_curpos(0),_bnums(0),_size(0),_chdata(NULL)
{
    _frames[0] = INT_MAX;  // top
    _frames[1] = INT_MIN;  // bottom
    _frames[2] = INT_MAX;  // left
    _frames[3] = INT_MIN;  // right
}

cfont::cfont(int ch, int size, int nums):_curpos(0),_bnums(nums),_ch(ch),_size(size)
{
    _chdata = (char*)malloc(size * sizeof(char));
    _frames[0] = INT_MAX;  // top
    _frames[1] = INT_MIN;  // bottom
    _frames[2] = INT_MAX;  // left
    _frames[3] = INT_MIN;  // right
}

cfont::cfont(const cfont& cf)
{
    _curpos= cf._curpos;
    _bnums = cf._bnums;
    _ch    = cf._ch;
    _size  = cf._size;
    _chdata = (char*)malloc(_size * sizeof(char));
    memcpy(_chdata, cf._chdata, _size);
    memcpy(_frames, cf._frames, 4*sizeof(int));
}

cfont::~cfont()
{
    if(_chdata){
        free(_chdata);
        _chdata = NULL;
    }
}

void  cfont::add(int chdata, int cindex) // add next point to the _chdata
{
    // fixup stack overflow, because last add(0,3) maybe use _chdata[_size] position,
    // use this test can be fix this case
    if(_curpos == _size)
        return;
    if(abs(chdata) >= 64){
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

void  cfont::encode(int ch, const vdPIXELS& fonts)
{
    _ch = ch;
    _bnums = fonts.size();
    size_t bsize = fonts.size();
    size_t size = 0;
    /*
     * ox: first boundary's first point's x
     * oy: first boundary's first point's y
     * sx: each boundary's start point's x
     * sy: each boundary's start point's y
     * px: previous point's x
     * py: previous point's y
     * cx: current point's x
     * cy: current point's x
     *
     */
    int ox = fonts[0][0].getX(), oy = fonts[0][0].getY();
    XRANGE(0);YRANGE(0);
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
            XRANGE(cx - ox);YRANGE(cy - oy);
            if(py == cy)
            {
                diff = cx - px;
                add(diff, 0);
            }
            else if(px == cx)
            {
                diff = cy - py;
                add(diff, 1);
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
    //_curpos--;  //see add function first test
}

void  cfont::decode(int& ch, vdPIXELS& fonts, int ox, int oy)
{
    ch = _ch;
    _curpos = 0;
    /*
     * sx: each boundary's start point's x
     * sy: each boundary's start point's y
     * cx: current point's x
     * cy: current point's y
     *
     */
    int sx = 0, sy =0;
    int cx = ox, cy = oy;
    unsigned char data = 0;
    PIXELS  cur(0, 0);
    dPIXELS aboundary;
    while(_curpos < _size)
    {
        //set each boundary's start point relation (ox, oy) position
        sx = _chdata[_curpos++], sy = _chdata[_curpos++];
        cur = PIXELS(sx + ox, sy + oy);
        aboundary.push_back(cur);
        cx = sx + ox;
        cy = sy + oy;
        while((_curpos < _size) && (data = _chdata[_curpos++]) != 0xff)
        {
            switch(0xc0 & data)
            {
            case 0x00:  //XR:  x+ right (>) change
                cur = PIXELS(cx+(0x3f & data), cy);
                break;
            case 0x40:	//XL:  x- left  (<) change
                cur = PIXELS(cx-(0x3f & data), cy);
                break;
            case 0x80:  //YD:  y+ down  (^) change
                cur = PIXELS(cx, cy+(0x3f & data));
                break;
            case 0xc0:  //YU:  y- up    (V) change
                cur = PIXELS(cx, cy-(0x3f & data));
                break;
            }
            cx = cur.getX();
            cy = cur.getY();
            aboundary.push_back(cur);
        }
        fonts.push_back(aboundary);
        _bnums++;
        aboundary.clear();
    }
}

void  cfont::init(char ox, char oy)  //first postion save (x, y), default init to (0, 0)
{
    _chdata[_curpos++] = ox;
    _chdata[_curpos++] = oy;
}


iFonts::iFonts():_ffont(NULL){}

iFonts::iFonts(const char* fpath,const char* mode)
{
    if(NULL != fpath){
        if(!(_ffont = fopen(fpath, mode))){
            printf("open file: %s is failed!", fpath);
        }
    }else{
        printf("%s is NULL", fpath);
    }
}

iFonts::~iFonts()
{
    if(_ffont)
    {
        fclose(_ffont);
        _ffont = NULL;
    }
}

bool  iFonts::loader(const char* fpath, const char* mode)
{
#define BREAK(_V) if(!_V){break;}
    if(NULL == fpath)
        return false;
    if(NULL == _ffont){
        if(!(_ffont = fopen(fpath, mode))){
            printf("open file: %s is failed!", fpath);
            return false;
        }
        //load font's file
        cfont cur;
        int size = 0;
        int rsize = -1;
        char* chdata = NULL;
        //int curpos = 0;
        while(1)
        {
            rsize = fread(&cur._ch, sizeof(int), 1, _ffont);
            BREAK(rsize);
            //curpos = ftell(_ffont);
            rsize = fread(&cur._frames, 4*sizeof(int), 1, _ffont);
            BREAK(rsize);
            //curpos = ftell(_ffont);
            rsize = fread(&size, sizeof(int), 1, _ffont);
            BREAK(rsize);
            //curpos = ftell(_ffont);
            cur._size = size;
            chdata = (char*)malloc(size* sizeof(char));
            rsize = fread(chdata, 1, size, _ffont);
            BREAK(rsize);
            //curpos = ftell(_ffont);
            cur._chdata = chdata;
            _fdata.push_back(cur);
        }
    }
    return true;
}

void  iFonts::encoder(int ch, const cfont& fonts)
{
    if(NULL == _ffont) return;
    fwrite(&ch, sizeof(int), 1, _ffont);
    fwrite(&fonts._frames, 4*sizeof(int), 1, _ffont);
    fwrite(&fonts._size, sizeof(int), 1, _ffont);
    fwrite(fonts._chdata, 1, fonts._size, _ffont);
}

void  iFonts::encoder(char* chs, const cfont& fonts)
{
    if(NULL == _ffont) return;
    fwrite(&chs, sizeof(int), 1, _ffont);
    fwrite(&fonts._frames, 4*sizeof(int), 1, _ffont);
    fwrite(&fonts._size, sizeof(int), 1, _ffont);
    fwrite(fonts._chdata, 1, fonts._size, _ffont);
}

void  iFonts::encoder(int ch, const vdPIXELS& fonts)
{
    if(NULL == _ffont) return;
    int vsize = 0;
    for(size_t i = 0; i < fonts.size(); ++i)
    {
        vsize += (fonts[i].size() + 1);
    }
    int  size = vsize + fonts.size() - 1;
    cfont inc(ch,  size, fonts.size());
    inc.encode(ch, fonts);
    fwrite(&ch, sizeof(int), 1, _ffont);
    fwrite(&inc._frames, 4*sizeof(int), 1, _ffont);
    fwrite(&inc._size, sizeof(int), 1, _ffont);
    fwrite(inc._chdata, 1, inc._size, _ffont);
}

void  iFonts::encoder(char* chs, const vdPIXELS& fonts)
{
    if(NULL == _ffont) return;
    int vsize = 0;
    for(size_t i = 0; i < fonts.size(); ++i)
    {
        vsize += (fonts[i].size() + 1);
    }
    int  size = vsize + fonts.size() - 1;
    std::cout << "encoder: " << chs << std::endl;
    int ch = 0;
    if(chs[0] & 0x80){ // Used to test whether it is Chinese, but it may not work properly
        memcpy(&ch, chs, strlen(chs));
    }else{
        ch = atoi(chs);
    }
    cfont inc(ch,  size, fonts.size());
    inc.encode(ch, fonts);
    fwrite(&ch, sizeof(int), 1, _ffont);
    //fwrite(&chs, sizeof(int), 1, _ffont);
    fwrite(&inc._frames, 4*sizeof(int), 1, _ffont);
    fwrite(&inc._size, sizeof(int), 1, _ffont);
    fwrite(inc._chdata, 1, inc._size, _ffont);
}

cfont iFonts::decoder(int ch)
{
    size_t size = _fdata.size();
    for(size_t i = 0; i < size; ++i)
    {
        if( _fdata[i]._ch == ch){
            return _fdata[i];
        }
    }
    return cfont();
}

}
}//namesapce Imaginer::Utils
