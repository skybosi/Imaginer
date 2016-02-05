#ifndef _IPOINT_H_
#define _IPOINT_H_
#include "ibmp.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;
#ifndef ONLY
// pixel position
class PIXELS
{
	public:
		int pix_X;
		int pix_Y;
		RGBQUAD prgb;
		U8 rgb_threshold;
		bool bEdge;				// edge point
		bool isEdge(PIXELS& pixel, int W,int H);
		bool isEdge(int W,int H);
		void show_PIXELS();
		//set (x,y)
		PIXELS setXY(PIXELS pixel);
		PIXELS setXY(int x, int y);
		PIXELS();
		//opposition the point color
		PIXELS opposition(PIXELS& ppot);
		PIXELS opposition();
		void toBin();
		PIXELS toBin(PIXELS& ppot);
		U8 set_threshold(PIXELS ppot);
		//set rgb
		PIXELS setRGB(PIXELS ppot);
		PIXELS setRGB(RGBQUAD rgb);
		PIXELS setRGB(U8 r,U8 g,U8 b);
		void fix_PIXELS(int W,int H);//fix up the point position
};
// pixel point
class PIXPOT
{
	public:
		PIXELS pot;
		PIXELS pot4s[4];				// 4 side point
		PIXELS pot4a[4];				// 4 angle point
		RGBQUAD diff4s[8];			// 4 side point rgb value diff with the fcspot
		RGBQUAD diff4a[8];			// 4 angle point rgb value diff with the fcspot
	public:
		PIXPOT();
		void show_PIXPOT();
		void show_PIXPOT(PIXPOT pots8);
		void show_PIXPOT8diffRGB(RGBQUAD diffRgb);
		//mix two color
		PIXELS mix(PIXELS& ppot1,PIXELS& ppot2,U8 weight = 50);
		void fix_PIXPOT(PIXPOT& pots8,int W,int H);//fix up the 8 point position
		RGBQUAD get_diff8RGB(PIXELS& fcspot,PIXELS& ppot);
		//binaryzation image
};
/*
// border 8 point pixel
class PIXPOT8
{
	private:
		PIXPOT fcspot;				  // focus point
		PIXPOT pot4s[4];				// 4 side point
		PIXPOT pot4a[4];				// 4 angle point
		RGBQUAD diff4s[8];			// 4 side point rgb value diff with the fcspot
		RGBQUAD diff4a[8];			// 4 angle point rgb value diff with the fcspot
	public:
		//PIXPOT8(){cout << "PIXPOT8 ...\n";}
		//~PIXPOT8(){cout << "~PIXPOT8 ...\n";}
		void fix_PIXELS(PIXELS& pixel,int W,int H);//fix up the point position
		void fix_PIXELS8(PIXPOT8& pots8,int W,int H);//fix up the 8 point position
		void show_PIXPOT8(PIXPOT8 pots8);
		RGBQUAD get_diff8RGB(PIXPOT& fcspot,PIXPOT& ppot);
		void show_PIXPOT8diffRGB(RGBQUAD diffRgb);
		friend class Rbmp;
};
*/
#endif //ONLY
#endif // ipoint.h :[]
