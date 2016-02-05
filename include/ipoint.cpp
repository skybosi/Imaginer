#include "ipoint.h"

#ifndef ONLY
//fix up the point position,if the point is edge point 
void PIXELS::fix_PIXELS(int W,int H)
{
	if(isEdge(*this,W,H))
	{
		if(pix_X < 0)
			pix_X += W;
		if(pix_Y < 0)
			pix_Y += H;
		if(pix_X >= W)
			pix_X -= W;
		if(pix_Y >= H)
			pix_Y -= H;
	}
}
void PIXPOT::fix_PIXPOT(PIXPOT& pots8,int W,int H)
{
	int i = 0;
	while(i<4)
	{
		pots8.pot4s[i].fix_PIXELS(W,H);
		pots8.pot4a[i].fix_PIXELS(W,H);
		i++;
	}
}
//get the diff RGB between the focus point with other 8 point
RGBQUAD PIXPOT::get_diff8RGB(PIXELS& fcspot,PIXELS& ppot)
{
	RGBQUAD diffRgb;
	diffRgb.rgbRed = fcspot.prgb.rgbRed - ppot.prgb.rgbRed;
	diffRgb.rgbGreen = fcspot.prgb.rgbGreen - ppot.prgb.rgbGreen;
	diffRgb.rgbBlue = fcspot.prgb.rgbBlue - ppot.prgb.rgbBlue;
	diffRgb.rgbReserved = 0;
	return diffRgb;
}
void PIXPOT::show_PIXPOT(PIXPOT pots8)
{
	int i = 0;
	while(i<4)
	{
		pots8.pot4s[i].show_PIXELS();
		show_PIXPOT8diffRGB(pots8.diff4s[i]);
		pots8.pot4a[i].show_PIXELS();
		show_PIXPOT8diffRGB(pots8.diff4a[i]);
		i++;
	}
}
void PIXPOT::show_PIXPOT8diffRGB(RGBQUAD diffRgb)
{
	printf("diff:(%03d,%03d,%03d)\n",
		diffRgb.rgbRed,
		diffRgb.rgbGreen,
		diffRgb.rgbBlue);
}
PIXPOT::PIXPOT()
{
	memset(this,0,sizeof(PIXPOT));
}
void PIXPOT::show_PIXPOT()
{
	pot.show_PIXELS();
	int i = 0;
	while(i<4)
	{
		pot4s[i].show_PIXELS();
		show_PIXPOT8diffRGB(diff4s[i]);
		pot4a[i].show_PIXELS();
		show_PIXPOT8diffRGB(diff4a[i]);
		i++;
	}
	/*
	printf("[R,G,B]:(%03d,%03d,%03d)\t",
		prgb.rgbRed,
		prgb.rgbGreen,
		prgb.rgbBlue);
		*/
}
//mix two color
PIXELS PIXPOT::mix(PIXELS& ppot1,PIXELS& ppot2,U8 weight)
{
	PIXELS ppot3;
	ppot3.prgb.rgbRed = (((ppot1.prgb.rgbRed * weight) + (ppot1.prgb.rgbRed *(1 - weight))) >> 2)/100;
	ppot3.prgb.rgbGreen = (((ppot1.prgb.rgbGreen * weight) + (ppot1.prgb.rgbGreen *(1 - weight))) >> 2)/100;
	ppot3.prgb.rgbBlue = (((ppot1.prgb.rgbBlue * weight) + (ppot1.prgb.rgbBlue *(1 - weight))) >> 2)/100;
	ppot3.prgb.rgbReserved = (((ppot1.prgb.rgbReserved * weight) + (ppot1.prgb.rgbReserved *(1 - weight))) >> 2)/100;
	return ppot3;
}
//opposition the point color
PIXELS PIXELS::opposition(PIXELS& ppot)
{
	ppot.prgb.rgbRed = 255 - ppot.prgb.rgbRed;
	ppot.prgb.rgbGreen = 255 - ppot.prgb.rgbGreen;
	ppot.prgb.rgbBlue = 255 - ppot.prgb.rgbBlue;
	return ppot;
}
PIXELS PIXELS::opposition()
{
	prgb.rgbRed = 255 - prgb.rgbRed;
	prgb.rgbGreen = 255 - prgb.rgbGreen;
	prgb.rgbBlue = 255 - prgb.rgbBlue;
	return *this;
}
//get a threshold from a point ppot
U8 PIXELS::set_threshold(PIXELS ppot)
{
	U8 r = ppot.prgb.rgbRed * 0.3;
	U8 g = ppot.prgb.rgbGreen * 0.59;
	U8 b = ppot.prgb.rgbBlue * 0.11;
	rgb_threshold = (r + g + b)/3;
	return rgb_threshold;
}
//binaryzation image with a threshold
void PIXELS::toBin()
{
	U8 r = prgb.rgbRed * 0.3;
	U8 g = prgb.rgbGreen * 0.59;
	U8 b = prgb.rgbBlue * 0.11;
	U8 rgb = (r + g + b)/3;
	if(rgb >= rgb_threshold)
	{
		rgb = 255;
	}
	else
		rgb = 0;
	setRGB(rgb,rgb,rgb);
}
PIXELS PIXELS::toBin(PIXELS& ppot)
{
	ppot.toBin();
	return ppot;
}

//set rgb with r g b
PIXELS PIXELS::setRGB(U8 r,U8 g,U8 b)
{
	prgb.rgbRed = r;
	prgb.rgbGreen = g;
	prgb.rgbBlue = b;
	return *this;
}
//set rgb with RGBQUAD
PIXELS PIXELS::setRGB(RGBQUAD rgb)
{
	prgb.rgbRed = rgb.rgbRed;
	prgb.rgbGreen = rgb.rgbGreen;
	prgb.rgbBlue = rgb.rgbBlue;
	prgb.rgbReserved = rgb.rgbReserved;
	return *this;
}
//set rgb with PIXELS
PIXELS PIXELS::setRGB(PIXELS ppot)
{
	prgb.rgbRed = ppot.prgb.rgbRed;
	prgb.rgbGreen = ppot.prgb.rgbGreen;
	prgb.rgbBlue = ppot.prgb.rgbBlue;
	prgb.rgbReserved = ppot.prgb.rgbReserved;
	return *this;
}

PIXELS::PIXELS()
{
	memset(this,0,sizeof(PIXELS));
}
bool PIXELS::isEdge(PIXELS& pixel, int W,int H)
{
	if((pixel.pix_X <= 0) | (pixel.pix_X >= W-1) |
			(pixel.pix_Y <= 0) | (pixel.pix_Y >= H-1))
	{
		pixel.bEdge = true;
		return true;
	}
	else
	{
		pixel.bEdge = false;
		return false;
	}
}
bool PIXELS::isEdge(int W,int H)
{
	if((pix_X <= 0) | (pix_X >= W-1) |
			(pix_Y <= 0) | (pix_Y >= H-1))
	{
		bEdge = true;
		return true;
	}
	else
	{
		bEdge = false;
		return false;
	}
}
//void PIXELS::show_PIXELS(PIXELS pixel)
void PIXELS::show_PIXELS()
{
	printf("X: %-3d Y: %-3d edge:%d\t",
			pix_X,
			pix_Y,
			bEdge);
	printf("[R,G,B]:(%03d,%03d,%03d)\t",
		prgb.rgbRed,
		prgb.rgbGreen,
		prgb.rgbBlue);
}
//set (x,y) PIXELS
PIXELS PIXELS::setXY(PIXELS pixel)
{
	pix_X = pixel.pix_X;
	pix_Y = pixel.pix_Y;
	bEdge = pixel.bEdge;
	return *this;
}
//set (x,y) with x y
PIXELS PIXELS::setXY(int x,int y)
{
	pix_X = x;
	pix_Y = y;
	return *this;
}
#endif //not ONLY
