/* 
 * define some marco of bmp file, if not a windows 
 * operate system.
 * define some marco about pixel point
 */
#ifndef _IBMP_H_
#define _IBMP_H_

#if defined (_WIN32) || defined(_WIN64)  || defined(_WINDOWS) || defined(_CONSOLE)
#define WINDOWS_HERE
#endif

#ifndef WINDOWS_HERE

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

typedef struct tagRGBQUAD
{
	U8 rgbBlue;
	U8 rgbGreen;
	U8 rgbRed;
	U8 rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[1];
} BITMAPINFO;


typedef struct tagBITMAP
{
	BITMAPFILEHEADER bfHeader;
	BITMAPINFO biInfo;
} BITMAPFILE;

#endif // pixel marco :[]
#pragma pack(pop)

#endif // ibmp.h :[]
