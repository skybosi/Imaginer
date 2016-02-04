#ifndef _IPOINT_H_
#define _IPOINT_H_
#include "ibmp.h"
#include <stdio.h>
#include <iostream>
using namespace std;
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
class PIXPOT
{
	public:
		PIXPOS pot;
		RGBQUAD prgb;
		void show_PIXPOT();
};
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
	//PIXPOT8(){cout << "PIXPOT8 ...\n";}
	//~PIXPOT8(){cout << "~PIXPOT8 ...\n";}
	void fix_PIXPOS(PIXPOS& pixel,int W,int H);//fix up the point position
	void fix_PIXPOS8(PIXPOT8& pots8,int W,int H);//fix up the 8 point position
	void show_PIXPOT8(PIXPOT8 pots8);
	RGBQUAD get_diff8RGB(PIXPOT& fcspot,PIXPOT& ppot);
	void show_PIXPOT8diffRGB(RGBQUAD diffRgb);
};
#endif //ONLY
#endif // ipoint.h :[]
