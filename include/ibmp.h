/* 
 * define some marco of bmp file, if not a windows 
 * operate system.
 * define some marco about pixel point
 */
#ifndef _IBMP_H_
#define _IBMP_H_
#include <string>

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
typedef struct tagALLHEAD
{
	BITMAPFILEHEADER bmpHead;
	BITMAPINFOHEADER infoHead;
} BMPALLHEAD;

#endif // pixel marco :[]
#pragma pack(pop)
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

#endif // ibmp.h :[]
