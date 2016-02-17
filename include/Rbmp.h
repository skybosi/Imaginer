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
		int bmpWidth;//图像的宽
		int bmpHeight;//图像的高
		int biBitCount;//图像类型，每像素位数
		FILE* fp;
		FILE* fpo;
		string bmppath;//图像的路径
		unsigned char *pBmpBuf;//读入图像数据的指针
		RGBQUAD *pColorTable;//颜色表指针
		int bfOffBits;
		BITMAPFILEHEADER head;
		BITMAPINFOHEADER infohead;
		U8* allhead;
	public:
		Rbmp(const char* bmpname);
		~Rbmp();
		//get a point info.
		PIXELS get_pix(PIXELS pixel);
		PIXELS get_pix(int x,int y);
		//get the 8 point info. around a point
		PIXPOT get_pot(PIXELS pixel);
		void get_image_msg();
		bool read_image();
		PIXELS* readIline(int beginY=0,int rows=0);
		void delReadIline(PIXELS* lineppot);
		bool isBoundary(PIXELS* lineppot);
		bool deal_image();
		bool write_image(const char* outpath);
		bool save_image(char *bmppath,unsigned char *imgBuf,int width,int height, int biBitCount, RGBQUAD *pColorTable);
	private:
		bool initRimage();
		bool initWimage();
		bool out_range_error(PIXELS pixel);
		void show_bmp_head(BITMAPFILEHEADER &head);
		void show_info_head(BITMAPINFOHEADER &infohead);

};

#endif
