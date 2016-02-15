#ifndef _IPOINT_H_
#define _IPOINT_H_
#include "ibmp.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;

// pixel position
class PIXELS
{
	private:
		int pix_X;
		int pix_Y;
		RGBQUAD prgb;
		U8 rgb_threshold;
		bool bEdge;				// edge point
	public:
		PIXELS();
		bool isEdge(PIXELS& pixel, int W,int H);
		bool isEdge(int W,int H);
		void show_PIXELS();
		//set rgb
		PIXELS setRGB(PIXELS ppot);
		PIXELS setRGB(RGBQUAD rgb);
		PIXELS setRGB(U8 r,U8 g,U8 b);
		//set (x,y)
		PIXELS setXY(PIXELS pixel);
		PIXELS setXY(int x, int y);
		PIXELS resetXY(int x,int y);
		U8 set_threshold(PIXELS ppot);
		RGBQUAD get_diff8RGB(PIXELS ppot);
	public:
		int getX();
		int getY();
		//binaryzation image
		void toBin();
		PIXELS toBin(PIXELS& ppot);
		//opposition the point color
		PIXELS opposition();
		PIXELS opposition(PIXELS& ppot);
		PIXELS& operator=(const PIXELS& pixel);
		//mix two color
		PIXELS mix(PIXELS& ppot1,PIXELS& ppot2,U8 weight = 50);
		void fix_PIXELS(int W,int H);//fix up the point position
};
// pixel point
class PIXPOT
{
	private:
		PIXELS pot;
		PIXELS pot4s[4];				// 4 side point
		PIXELS pot4a[4];				// 4 angle point
		RGBQUAD diff4s[8];			// 4 side point rgb value diff with the fcspot
		RGBQUAD diff4a[8];			// 4 angle point rgb value diff with the fcspot
	public:
		PIXPOT();
		void show_PIXPOT();
		void show_PIXPOT8diffRGB(RGBQUAD diffRgb);
		void fix_PIXPOT(PIXPOT& pots8,int W,int H);//fix up the 8 point position
		PIXELS* get_pos8(PIXELS pixel,PIXELS* pos8,int W,int H);
		PIXPOT set_pots8(PIXELS* pos8);
};
#endif // ipoint.h :[]
