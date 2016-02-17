#ifndef _RBMP_H_
#define _RBMP_H_
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include "ibmp.h"
#include "image.h"
#include "ipoint.h"
using namespace std;
//class Rbmp: public Image
class Rbmp
{
	private:
		FILE* fp;
		FILE* fpo;
		int bmpWidth;//图像的宽
		int bmpHeight;//图像的高
		int bfOffBits;
		int biBitCount;//图像类型，每像素位数
		string bmppath;//图像的路径
		BYTE8* allhead;
		BYTE8* pBmpBuf;//读入图像数据的指针
		PIXELS** allData;//bmp image matrix
		RGBQUAD* pColorTable;//颜色表指针
		BITMAPFILEHEADER head;
		BITMAPINFOHEADER infohead;
	public:
		~Rbmp();
		Rbmp(const char* bmpname);
		//get a point info.
		void get_image_msg();
		PIXELS get_pix(int x,int y);
		PIXELS get_pix(PIXELS pixel);
		//get the 8 point info. around a point
		PIXPOT get_pot(PIXELS pixel);
	public:
		bool deal_image();
		PIXELS** readIline(int beginY=0,int rows=0);
		void delReadIline(PIXELS** lineppoti,int rows);
		bool isBoundary(PIXELS* lineppot);
		bool write_image(const char* outpath);
		void show_allData();
	private:
		bool init_image();
		bool initWimage();
		bool out_range_error(PIXELS pixel);
		void show_bmp_head(BITMAPFILEHEADER &head);
		void show_info_head(BITMAPINFOHEADER &infohead);
};

#endif
