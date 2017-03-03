/*
 * define some marco of bmp file,Especially some struct of BMP format
 */
#ifndef _IBMP_H_
#define _IBMP_H_
#include <string>
#include "iColor.h"

using namespace Imaginer::MetaData;
namespace Imaginer {
namespace BaseData {

//BMP image format Message
#pragma pack(push, 1)

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef U8 BYTE8;

typedef struct tagBITMAPFILEHEADER
{

    U16 bfType;
    U32 bfSize;
    U16 bfReserved1;
    U16 bfReserved2;
    U32 bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    U32 biSize;
    U32 biWidth;
    U32 biHeight;
    U16 biPlanes;
    U16 biBitCount;
    U32 biCompression;
    U32 biSizeImage;
    U32 biXPelsPerMeter;
    U32 biYPelsPerMeter;
    U32 biClrUsed;
    U32 biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO
{
    BITMAPINFOHEADER bmiHeader;
    iColor bmiColors[1];
} BITMAPINFO;

typedef struct tagBITMAP
{
    BITMAPFILEHEADER bfHeader;
    BITMAPINFO biInfo;
} BITMAPFILE;

typedef struct tagALLHEAD
{
    BITMAPFILEHEADER bmpHead;
    BITMAPINFOHEADER infoHead;
} BMPALLHEAD;

#pragma pack(pop)
}//namespace BaseData
}//namespace Imaginer

#endif // ibmp.h :[]
