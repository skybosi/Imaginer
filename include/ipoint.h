#ifndef _IPOINT_H_
#define _IPOINT_H_
#include "ibmp.h"
#include <stdio.h>

#ifndef ONLY
// pixel position
//typedef struct Ipixpos
class PIXPOS
{
	public:
		int pix_X;
		int pix_Y;
		bool bEdge;				// edge point
		bool isEdge(PIXPOS pixel, int W,int H);
		void show_PIXPOS();
};
// pixel point
typedef struct Ipixpot
{
	PIXPOS pot;
	RGBQUAD prgb;
} PIXPOT;
// border 8 point pixel
class PIXPOT8
{
	friend class Rbmp;
	private:
	PIXPOT fcspot;				// focus point
	PIXPOT pot4s[4];				// 4 side point
	PIXPOT pot4a[4];				// 4 angle point
	RGBQUAD diff4s[8];			// 4 side point rgb value diff with the fcspot
	RGBQUAD diff4a[8];			// 4 angle point rgb value diff with the fcspot
	public:
	void fix_PIXPOS(PIXPOS& pixel,int W,int H);//fix up the point position
	void fix_PIXPOS8(PIXPOT8& pot8,int W,int H);//fix up the 8 point position
	void show_PIXPOT(PIXPOT pots);
	void show_PIXPOT8(PIXPOT8 pots8);
	RGBQUAD get_diff8RGB(PIXPOT& fcspot,PIXPOT& ppot8);
	void show_PIXPOT8diffRGB(RGBQUAD diffRgb);

};
#endif //ONLY
#endif // ipoint.h :[]
