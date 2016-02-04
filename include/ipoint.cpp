#include "ipoint.h"

#ifndef ONLY
//fix up the point position,if the point is edge point 
void PIXPOT8::fix_PIXPOS(PIXPOS& pixel,int W,int H)
{
	if(pixel.isEdge(pixel,W,H))
	{
		pixel.bEdge = true;
		if(pixel.pix_X < 0)
			pixel.pix_X += W;
		if(pixel.pix_Y < 0)
			pixel.pix_Y += H;
		if(pixel.pix_X >= W)
			pixel.pix_X -= W;
		if(pixel.pix_Y >= H)
			pixel.pix_Y -= H;
	}
	else
		pixel.bEdge = false;
}
void PIXPOT8::fix_PIXPOS8(PIXPOT8& pots8,int W,int H)
{
	int i = 0;
	while(i<4)
	{
		fix_PIXPOS(pots8.pot4s[i].pot,W,H);
		fix_PIXPOS(pots8.pot4a[i].pot,W,H);
		i++;
	}
}
//get the diff RGB between the focus point with other 8 point
RGBQUAD PIXPOT8::get_diff8RGB(PIXPOT& fcspot,PIXPOT& ppot)
{
	RGBQUAD diffRgb;
	diffRgb.rgbRed = fcspot.prgb.rgbRed - ppot.prgb.rgbRed;
	diffRgb.rgbGreen = fcspot.prgb.rgbGreen - ppot.prgb.rgbGreen;
	diffRgb.rgbBlue = fcspot.prgb.rgbBlue - ppot.prgb.rgbBlue;
	diffRgb.rgbReserved = 0;
	return diffRgb;
}
void PIXPOT8::show_PIXPOT8(PIXPOT8 pots8)
{
	int i = 0;
	while(i<4)
	{
		pots8.pot4s[i].show_PIXPOT();
		show_PIXPOT8diffRGB(pots8.diff4s[i]);
		pots8.pot4a[i].show_PIXPOT();
		show_PIXPOT8diffRGB(pots8.diff4a[i]);
		i++;
	}
}
void PIXPOT8::show_PIXPOT8diffRGB(RGBQUAD diffRgb)
{
	printf("diff:(%03d,%03d,%03d)\n",
		diffRgb.rgbRed,
		diffRgb.rgbGreen,
		diffRgb.rgbBlue);
}
void PIXPOT::show_PIXPOT()
{
	pot.show_PIXPOS();
	printf("[R,G,B]:(%03d,%03d,%03d)\t",
		prgb.rgbRed,
		prgb.rgbGreen,
		prgb.rgbBlue);
}

bool PIXPOS::isEdge(PIXPOS pixel, int W,int H)
{
	if((pixel.pix_X <= 0) | (pixel.pix_X >= W-1) |
			(pixel.pix_Y <= 0) | (pixel.pix_Y >= H-1))
		return true;
	else
		return false;
}
//void PIXPOS::show_PIXPOS(PIXPOS pixel)
void PIXPOS::show_PIXPOS()
{
	printf("X: %-3d Y: %-3d edge:%d\t",
			pix_X,
			pix_Y,
			bEdge);
}
#endif //ONLY
