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
//#define ONLY
#ifdef ONLY
// pixel position
typedef struct Ipixpos
{
	int pix_X;
	int pix_Y;
	bool bEdge;				// edge point
} PIXPOS;

// pixel point
typedef struct Ipixpot
{
	PIXPOS pot;
	RGBQUAD prgb;
} PIXPOT;
// border 8 point pixel
typedef struct Ipixpot8
{
	PIXPOT fcspot;				// focus point
	PIXPOT pot4s[4];				// 4 side point
	PIXPOT pot4a[4];				// 4 angle point
	RGBQUAD diff4s[4];			// 4 side point rgb value diff with the fcspot
	RGBQUAD diff4a[4];			// 4 angle point rgb value diff with the fcspot
} PIXPOT8;
#endif

#pragma pack(pop)

#endif // ibmp.h :[]
