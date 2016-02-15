#include "ipoint.h"

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
RGBQUAD PIXELS::get_diff8RGB(PIXELS pixel)
{
	RGBQUAD diffRgb;
	diffRgb.rgbRed = prgb.rgbRed - pixel.prgb.rgbRed;
	diffRgb.rgbGreen = prgb.rgbGreen - pixel.prgb.rgbGreen;
	diffRgb.rgbBlue = prgb.rgbBlue - pixel.prgb.rgbBlue;
	diffRgb.rgbReserved = prgb.rgbReserved - pixel.prgb.rgbReserved;
	return diffRgb;
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
	printf("\n");
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
//get 8 point position(x,y)
PIXELS* PIXPOT::get_pos8(PIXELS pixel,PIXELS* pos8,int W,int H)
{
	int i = 0;
	while(i<4)
	{
		//get 4 side point position(x,y)
		pot4s[i] = pixel;
		//get 4 angle point position(x,y)
		pot4a[i] = pixel;
		i++;
	}
	pot4s[0].resetXY(0,-1);
	pot4s[1].resetXY(1,0);
	pot4s[2].resetXY(0,1);
	pot4s[3].resetXY(-1,0);

	pot4a[0].resetXY(-1,-1);
	pot4a[1].resetXY(1,-1);
	pot4a[2].resetXY(1,1);
	pot4a[3].resetXY(-1,1);
	fix_PIXPOT(*this,W,H);
	pos8[0] = pixel;
//	pos8[0].show_PIXELS();
//	printf("\n");
	i = 1;
	while(i < 5)
	{
		pos8[i] = pot4s[i-1];
//		pos8[i].show_PIXELS();
//		printf("\n");
		pos8[i+4] = pot4a[i-1];
//		pos8[i+4].show_PIXELS();
//		printf("\n");
		i++;
	}
//	printf("???????????????\n");
//	show_PIXPOT();
	return pos8;
}
PIXPOT PIXPOT::set_pots8(PIXELS* pos8)
{
	pot  = pos8[0];
	int i = 0;
	while(i< 4)
	{
		pot4s[i] = pos8[i+1];
		diff4s[i] = pot4s[i].get_diff8RGB(pos8[0]);
		pot4a[i] = pos8[i+5];
		diff4a[i] = pot4a[i].get_diff8RGB(pos8[0]);
		i++;
	}
	//show_PIXPOT();
	return *this;
}
//mix two color
PIXELS PIXELS::mix(PIXELS& ppot1,PIXELS& ppot2,U8 weight)
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
PIXELS PIXELS::resetXY(int x,int y)
{
	pix_X += x;
	pix_Y += y;
	return *this;
}
int PIXELS::getX()
{
	return pix_X;
}
int PIXELS::getY()
{
	return pix_Y;
}
//just copy the position
PIXELS& PIXELS::operator=(const PIXELS& pixel)
{
	if(this == &pixel)
		return *this;
	pix_X = pixel.pix_X;
	pix_Y = pixel.pix_Y;
	memcpy(&prgb,&pixel.prgb,sizeof(RGBQUAD));
	rgb_threshold = pixel.rgb_threshold;
	return *this;
}
