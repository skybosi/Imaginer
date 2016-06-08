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
	public:
		typedef std::pair<Position,int> pix_p;
	private:
		int pix_X;
		int pix_Y;
		RGBQUAD prgb;
		U8 rgb_threshold;
		int bEdge;				// edge point
		bool bEmpty;      //the pixel is NULL ,is false
		pix_p pix_P;
	public:
		PIXELS();
		PIXELS(const RGBQUAD& rgb);
		bool empty();
		bool isEdge(PIXELS& pixel, int W,int H);
		bool isEdge(int W,int H);
		void show_PIXELS();
		//set rgb
		PIXELS setRGB(PIXELS ppot);
		PIXELS setRGB(RGBQUAD rgb);
		PIXELS setRGB(U8 r,U8 g,U8 b);
		bool setData(BYTE8& b,BYTE8& g,BYTE8& r);
		//set (x,y)
		PIXELS setXY(PIXELS pixel);
		PIXELS setXY(int x, int y);
		PIXELS resetXY(int x,int y);
		void   setempty(bool state = false);
		void   setEdge(int bedge = 0);
		//set pix_P
		//@pos    : the point position(default None)
		//@rtimes : reference times   (default 0)
		void   initpPos();
		void   setpPos(Position pos = None);
		U8 set_threshold(PIXELS ppot);
		RGBQUAD get_diff8RGB(PIXELS ppot);
	public:
		int getX()const;
		int getY()const;
		U8  getRed()const;
		U8  getGreen()const;
		U8  getBlue()const;
		int getEdge()const;
		RGBQUAD getRGB()const;
		pix_p getpPos();
		//binaryzation image
		void toBin();
		PIXELS toBin(PIXELS& ppot);
		PIXELS get3Color(colorType color);
		//opposition the point color
		PIXELS opposition();
		PIXELS opposition(PIXELS& ppot);
		//opposition the point color
		PIXELS&  operator~();
		PIXELS& operator=(const PIXELS& pixel);
		PIXELS  operator-(const PIXELS& pixel);
		friend const PIXELS operator-(const PIXELS& pixel1,const PIXELS& pixel2);
		bool operator==(const PIXELS& pixel);
		bool operator!=(const PIXELS& pixel);
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
		//show the PIXPOT(include focus point,the other 8 point)
		void show_PIXPOT();
		//show the diff between focus point with other 8 point
		void show_PIXPOT8diffRGB(RGBQUAD diffRgb);
		//fix the PIXPOT's postion (include 4 side point and 4 angle point) 
		void fix_PIXPOT(PIXPOT& pots8,int W,int H);//fix up the 8 point position
		//get the right point postion from resetXY
		PIXELS* get_pos8(PIXELS pixel,PIXELS* pos8,int W,int H);
		//set the right point postion
		PIXPOT set_pots8(PIXELS* pos8);
		//get the diff value 4 side point
		RGBQUAD get_diff4s(int indexs);
		//get the diff value 4 angle point
		RGBQUAD get_diff4a(int indexa);
		//pixel similarity
		bool pixelSimilar();
};
#endif // ipoint.h :[]
